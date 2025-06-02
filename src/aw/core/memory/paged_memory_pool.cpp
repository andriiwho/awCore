#include "aw/core/memory/paged_memory_pool.h"

#include <cassert>
#include <cstring>
#include <mutex>

namespace aw::core
{
	/**
	 * TODO: If memory leaks, dump the allocations
	 */

	void MemoryPage::alloc_next()
	{
		std::lock_guard lock(m_Mutex);
		if (!m_Next)
		{
			m_Next = static_cast<MemoryPage*>(malloc(sizeof(MemoryPage)));
			std::construct_at(m_Next, m_AlignedAllocSize);
			assert(m_Next);
			m_Next->m_Prev = this;
		}
	}

	void MemoryPage::dealloc_next()
	{
		std::lock_guard lock(m_Mutex);
		if (!m_Next)
			return;

		MemoryPage* next_next = m_Next->m_Next;
		std::destroy_at(m_Next);
		free(m_Next);
		m_Next = next_next;
	}

	void* MemoryPage::allocate_block(const u64 requested_size)
	{
		if (m_NumAliveAllocations.load(std::memory_order::acquire) == m_MaxAllocations)
		{
			std::lock_guard lock(m_Mutex);
			if (!m_Next)
			{
				alloc_next();
			}
			return m_Next->allocate_block(requested_size);
		}

		std::lock_guard lock(m_Mutex);
		// First, try to allocate from a free list
		if (m_FreeList.size > 0)
			return initialize_header(m_FreeList.pop(), requested_size);

		// Try to allocate from the tail
		if (m_Tail != m_MaxAllocations)
		{
			return initialize_header(m_Tail++, requested_size);
		}

		return nullptr;
	}

	void* MemoryPage::initialize_header(const u64 index, const u64 requested_size)
	{
		void*	   block = get_block_at_index(index);
		const auto header = static_cast<AllocationHeader*>(block);
		header->page = this;
		header->index = to_u32(index);
		header->alloc_size = to_u32(requested_size);
		m_NumAliveAllocations.fetch_add(1, std::memory_order::acquire);
		return header + 1;
	}

	void MemoryPage::free_block(void* block)
	{
		if (!block)
			return;

		AllocationHeader* header = static_cast<AllocationHeader*>(block) - 1;
		// If it is a paged allocation, go through the page-wise deallocation process
		if (MemoryPage* page = header->page)
		{
			std::lock_guard lock(page->m_Mutex);
			page->m_FreeList.push(header->index);
			--page->m_NumAliveAllocations;

			// If the page is not root
			if constexpr (CLEAR_EMPTY_PAGES)
			{
				if (page->m_NumAliveAllocations.load(std::memory_order::acquire) == 0 && !page->m_Root)
				{
					assert(page->m_Prev);
					page->m_Prev->dealloc_next();
				}
			}
		}
		// Otherwise clear the block
		else
		{
			free(header);
		}
	}

	void* MemoryPage::get_block_at_index(const u64 index)
	{
		return m_Memory.data() + (index * m_AlignedAllocSize);
	}

	void MemoryPage::shutdown()
	{
		// assert_msg(m_num_alive_allocations == 0, "It seems like there is a memory leak.");
		if (m_Next)
		{
			m_Next->shutdown();
			dealloc_next();
		}
	}

	void* PagedMemoryPool::allocate_memory(const Bytes size)
	{
		if (size == 0)
			return nullptr;

		// If the size is bigger than the page, allocate it just using malloc
		if (size > DEFAULT_PAGE_SIZE)
		{
			const auto header = static_cast<AllocationHeader*>(malloc(size + sizeof(AllocationHeader)));
			assert(header);
			std::construct_at(header);
			header->page = nullptr;
			header->index = std::numeric_limits<u32>::max();
			header->alloc_size = to_u32(size.value);
			return header + 1;
		}

		MemoryPage* tree_root = get_page_tree_for_allocation(size);
		return tree_root->allocate_block(size);
	}

	void PagedMemoryPool::free_memory(void* memory)
	{
		MemoryPage::free_block(memory);
	}

	MemoryPage* PagedMemoryPool::get_page_tree_for_allocation(const Bytes allocation_size)
	{
		// Check if the allocation size is more than DEFAULT_PAGE_SIZE.
		// We shouldn't get here if everything is correct
		assert(allocation_size <= DEFAULT_PAGE_SIZE);

		const Bytes aligned_size = Math::align_to_pow2(allocation_size + sizeof(AllocationHeader));
		const u64	page_index = Math::fast_log2(aligned_size);
		// Check if a page tree at the index exists, if not, create it
		MemoryPage*&	 found_page_tree = m_PageTrees.at(page_index);
		std::shared_lock read_lock(m_PageTreesMutex);
		if (found_page_tree)
		{
			return found_page_tree;
		}
		read_lock.unlock();

		std::unique_lock write_lock(m_PageTreesMutex);
		if (!found_page_tree)
		{
			found_page_tree = static_cast<MemoryPage*>(malloc(sizeof(MemoryPage)));
			std::construct_at(found_page_tree, aligned_size);
			found_page_tree->set_root();
		}

		return found_page_tree;
	}

	PagedMemoryPool::~PagedMemoryPool()
	{
		for (const auto& page : m_PageTrees)
		{
			if (page)
			{
				page->shutdown();
				std::destroy_at(page);
				free(page);
			}
		}
	}

	PagedMemoryPool& PagedMemoryPool::get()
	{
		static PagedMemoryPool instance;
		return instance;
	}

	u64 PagedMemoryPool::get_allocation_size(const void* const data)
	{
		if (!data)
		{
			return 0;
		}

		const AllocationHeader* header = static_cast<const AllocationHeader*>(data) - 1;
		return header->alloc_size;
	}

	PageFreeList::PageFreeList(const u32 preallocated_capacity /*= {}*/)
		: capacity(preallocated_capacity)
	{
		if (preallocated_capacity > 0)
		{
			data = static_cast<u32*>(malloc(sizeof(u32) * capacity));
		}
	}

	PageFreeList::~PageFreeList()
	{
		if (data)
			free(data);
	}

	void PageFreeList::push(const u32 index)
	{
		std::lock_guard lock(m_Mutex);
		if (tail == capacity)
		{
			const u64 old_capacity = capacity;
			capacity = to_u32(CAPACITY_INCREMENT * capacity);
			const auto new_block = static_cast<u32*>(malloc(capacity * sizeof(u32)));
			assert(new_block);
			std::memcpy(new_block, data, old_capacity * sizeof(u32));
			free(data);
			data = new_block;
		}

		data[tail++] = index;
		++size;
	}

	u32 PageFreeList::peek() const
	{
		if (size == 0)
			return std::numeric_limits<u32>::max();

		return data[tail - 1];
	}

	u32 PageFreeList::pop()
	{
		std::lock_guard lock(m_Mutex);
		if (size == 0)
			return std::numeric_limits<u32>::max();

		const u32 value = peek();
		--tail;
		--size;

		return value;
	}
} // namespace aw::core
