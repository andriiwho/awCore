#pragma once

#include "numbers.h"

namespace aw::core
{
	constexpr u32 hash_combine(u32 seed, u32 value) noexcept
	{
		seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}

	constexpr u64 hash_combine(u64 seed, u64 value) noexcept
	{
		seed ^= value + 0x9e3779b97f4a7c15 + (seed << 12) + (seed >> 4);
		return seed;
	}

	template <typename T>
	constexpr auto hash_combine_values(T first) noexcept
	{
		return first;
	}

	template <typename T, typename... Rest>
	constexpr auto hash_combine_values(T first, Rest... rest) noexcept
	{
		return hash_combine(first, hash_combine_values(rest...));
	}
} // namespace aw::core