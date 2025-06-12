#pragma once

#include "aw/core/memory/allocators.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <queue>

namespace aw::core
{
	template <typename T, typename Allocator = DefaultAllocator<T>>
	using Vector = std::vector<T, Allocator>;

	template <typename Key, typename T, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>, typename Allocator = DefaultAllocator<std::pair<const Key, T>>>
	using HashMap = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

	template <typename T, typename Allocator = DefaultAllocator<T>>
	using Queue = std::queue<T, std::deque<T, Allocator>>;
} // namespace aw::core