#include "aw/core/memory/memalloc.h"

#include <cstdlib>
#include <mutex>

namespace aw::core {
    // We are wasting space here, but it's going to be fine for now.
    struct MemoryAllocationHeader {
        usize size;
        void *next;
        void *prev;
    };

    static void *last_allocation = nullptr;
    static std::mutex last_alloc_mutex = {};
    static usize total_allocations = 0;

    static MemoryAllocationHeader *get_allocation_header(void *ptr) {
        return static_cast<MemoryAllocationHeader *>(ptr) - 1;
    }

    // Don't listen to clang-tidy. We don't leak memory here. We're just wasting space.
    void *allocate_memory(const usize size) {
        if (size == 0) {
            return nullptr;
        }

        MemoryAllocationHeader *header{};
        void* allocation = std::malloc(size + sizeof(MemoryAllocationHeader));
        header = static_cast<MemoryAllocationHeader *>(allocation);
        header->prev = nullptr;

        {
            std::lock_guard lock(last_alloc_mutex);
            if (last_allocation) {
                header->prev = last_allocation;
                get_allocation_header(last_allocation)->next = header + 1;
                last_allocation = header + 1;
            }
            else {
                last_allocation = header + 1;
            }
        }

        header->size = size;
        header->next = nullptr;
        total_allocations++;
        return header + 1;
    }

    void free_memory(void *ptr) {
        if (!ptr) {
            return;
        }

        MemoryAllocationHeader *header = get_allocation_header(ptr);
        std::lock_guard lock(last_alloc_mutex);
        if (header->prev) {
            get_allocation_header(header->prev)->next = header->next;
        }

        if (header->next) {
            get_allocation_header(header->next)->prev = header->prev;
        }

        if (last_allocation == ptr) {
            last_allocation = header->prev ? header->prev : header->next;
        }

        total_allocations--;
        std::free(header);
    }

    usize get_allocation_size(void *ptr) {
        if (!ptr) {
            return 0;
        }

        return get_allocation_header(ptr)->size;
    }

    void* get_next_allocation(void *ptr) {
        if (!ptr) {
            return nullptr;
        }

        return get_allocation_header(ptr)->next;
    }

    void* get_prev_allocation(void *ptr) {
        if (!ptr) {
            return nullptr;
        }

        return get_allocation_header(ptr)->prev;
    }

    usize get_allocation_count() {
        return total_allocations;
    }
}
