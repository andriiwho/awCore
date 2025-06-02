#pragma once

#include "aw/core/primitive/numbers.h"

namespace aw::core
{
	/** Allocates memory on the heap */
	extern void* allocate_memory(usize size);

	/** Frees memory on the heap */
	extern void free_memory(void* ptr);

	/** Returns the size of the allocation */
	extern usize get_allocation_size(void* ptr);
} // namespace aw::core