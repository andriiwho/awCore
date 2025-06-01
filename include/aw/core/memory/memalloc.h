#pragma once

#include "aw/core/primitive/numbers.h"

namespace aw::core {
    /** Allocates memory on the heap */
    extern void* allocate_memory(usize size);

    /** Frees memory on the heap */
    extern void free_memory(void* ptr);

    /** Returns the size of the allocation */
    extern usize get_allocation_size(void* ptr);

    /** Returns the next allocation in the linked list */
    extern void* get_next_allocation(void* ptr);

    /** Returns the previous allocation in the linked list */
    extern void* get_prev_allocation(void* ptr);

    /** Returns the number of allocations */
    extern usize get_allocation_count();
}