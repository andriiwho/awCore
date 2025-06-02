#pragma once

#include "aw/core/primitive/numbers.h"
#include "aw/core/math/math.h"

#include <array>
#include <shared_mutex>
#include <atomic>

namespace aw::core
{
#define AW_PASTE(a, b) a##b
#define AW_PASTE2(a, b) AW_PASTE(a, b)
#define AW_PASTE3(a, b, c) AW_PASTE2(AW_PASTE2(a, b), c);
} // namespace aw::core

namespace aw::core
{
	struct Megabytes
	{
		constexpr Megabytes() = default;

		constexpr Megabytes(const u64 in_value)
			: value(in_value)
		{
		}

		u64 value{};
	};

	struct Kilobytes
	{
		constexpr Kilobytes() = default;

		constexpr Kilobytes(const u64 in_value)
			: value(in_value)
		{
		}

		constexpr Kilobytes(Megabytes mb)
			: value(mb.value * 1024)
		{
		}

		u64 value{};
	};

	struct Bytes
	{
		constexpr Bytes() = default;

		constexpr Bytes(const u64 in_value)
			: value(in_value)
		{
		}

		constexpr Bytes(const Kilobytes kb)
			: value(kb.value * 1024)
		{
		}

		constexpr Bytes(const Megabytes mb)
			: Bytes(Kilobytes(mb))
		{
		}

		inline constexpr operator u64() const noexcept { return value; }

		u64 value{};
	};

	struct AllocationHeader
	{
		class MemoryPage* page{};
		u32				  index{};
		u32				  alloc_size{};
	};

	struct PageFreeList
	{
		static constexpr u64	DEFAULT_PREALLOCATED_CAPACITY = 1024;
		static constexpr double CAPACITY_INCREMENT = 1.5;

		PageFreeList(u32 preallocated_capacity = DEFAULT_PREALLOCATED_CAPACITY);

		~PageFreeList();

		void push(u32 index);

		u32 peek() const;

		u32 pop();

		std::atomic<u32*> data{};
		std::atomic<u32>  tail{};
		std::atomic<u32>  capacity{};
		std::atomic<u32>  size{};
		std::mutex		  m_Mutex{};
	};

	static constexpr Bytes DEFAULT_PAGE_SIZE = Megabytes(4);

	class MemoryPage
	{
	public:
		MemoryPage(const Bytes aligned_alloc_size)
			: m_AlignedAllocSize(aligned_alloc_size)
			, m_MaxAllocations(DEFAULT_PAGE_SIZE / aligned_alloc_size)
		{
		}

		void set_root() { m_Root = true; }

		void* allocate_block(u64 requested_size);

		static void free_block(void* block);

		void shutdown();

	private:
		void alloc_next();

		void dealloc_next();

		void* initialize_header(u64 index, u64 requested_size);

		void* get_block_at_index(u64 index);

	private:
		std::array<u8, DEFAULT_PAGE_SIZE> m_Memory{};
		MemoryPage*						  m_Prev{};
		MemoryPage*						  m_Next{};
		PageFreeList					  m_FreeList{};

		// Aligned size of the allocation. This is the allocation step.
		Bytes m_AlignedAllocSize{};

		const u64		 m_MaxAllocations{};
		std::atomic<u64> m_NumAliveAllocations{ 0 };

		u64		   m_Tail{};
		bool	   m_Root{};
		std::mutex m_Mutex{};
	};

	class PagedMemoryPool
	{
	public:
		static PagedMemoryPool& get();

		~PagedMemoryPool();

		void* allocate_memory(Bytes size);

		static void free_memory(void* memory);

		static u64 get_allocation_size(const void* const data);

	private:
		MemoryPage* get_page_tree_for_allocation(Bytes allocation_size);

		std::array<MemoryPage*, 64> m_PageTrees{};
		std::shared_mutex			m_PageTreesMutex{};
	};
} // namespace aw::core

#ifdef KP_NEVER_CLEAR_EMPTY_MEMORY_PAGES
inline constexpr bool CLEAR_EMPTY_PAGES = false;
#else
inline constexpr bool CLEAR_EMPTY_PAGES = true;
#endif

inline void* operator new(const std::size_t size, aw::core::PagedMemoryPool& pool)
{
	return pool.allocate_memory(aw::core::Bytes(size));
}

inline void operator delete(void* ptr, aw::core::PagedMemoryPool& pool)
{
	pool.free_memory(ptr);
}

#define aw_new new (aw::core::PagedMemoryPool::get())
#define aw_delete(val)    \
	std::destroy_at(val); \
	aw::core::PagedMemoryPool::get().free_memory(val)
