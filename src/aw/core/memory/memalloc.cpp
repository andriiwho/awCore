#include "aw/core/memory/memalloc.h"
#include "aw/core/memory/paged_memory_pool.h"

namespace aw::core {
    // We are wasting space here, but it's going to be fine for now.
    // Don't listen to clang-tidy. We don't leak memory here. We're just wasting space.
    void *allocate_memory(const usize size) {
        if (size == 0) {
            return nullptr;
        }

        return PagedMemoryPool::get().allocate_memory(Bytes(size));
    }

    void free_memory(void *ptr) {
        if (!ptr) {
            return;
        }

        PagedMemoryPool::get().free_memory(ptr);
    }

    usize get_allocation_size(void *ptr) {
        if (!ptr) {
            return 0;
        }

        return PagedMemoryPool::get().get_allocation_size(ptr);
    }
}
