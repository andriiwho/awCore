#pragma once

#include <type_traits>
#include <utility>

namespace aw::core
{
	template <typename T>
	concept ScopedEnum = std::is_enum_v<T> && std::is_scoped_enum_v<T>;

	template <ScopedEnum T>
	struct EnumMask
	{
		using UnderlyingType = std::underlying_type_t<T>;

		constexpr EnumMask() = default;

		constexpr EnumMask(UnderlyingType value) noexcept
			: mask(value)
		{
		}

		constexpr EnumMask(T value) noexcept
			: mask(std::to_underlying(value))
		{
		}

		constexpr EnumMask& operator|=(T value) noexcept
		{
			mask |= std::to_underlying(value);
			return *this;
		}

		constexpr EnumMask& operator&=(T value) noexcept
		{
			mask &= std::to_underlying(value);
			return *this;
		}

		constexpr EnumMask& operator^=(T value) noexcept
		{
			mask ^= std::to_underlying(value);
			return *this;
		}

		constexpr bool contains(T value) const noexcept
		{
			return (mask & std::to_underlying(value)) != 0;
		}

		constexpr void remove(T value) noexcept
		{
			mask &= ~std::to_underlying(value);
		}

		constexpr bool operator==(EnumMask other) const noexcept
		{
			return mask == other.mask;
		}

		UnderlyingType mask{};
	};
} // namespace aw::core

#define aw_enum_flags(T)                                                             \
	constexpr aw::core::EnumMask<T> operator|(T a, T b) noexcept                     \
	{                                                                                \
		return aw::core::EnumMask<T>(std::to_underlying(a) | std::to_underlying(b)); \
	}                                                                                \
	constexpr aw::core::EnumMask<T> operator&(T a, T b) noexcept                     \
	{                                                                                \
		return aw::core::EnumMask<T>(std::to_underlying(a) & std::to_underlying(b)); \
	}                                                                                \
	constexpr aw::core::EnumMask<T> operator|(aw::core::EnumMask<T> a, T b) noexcept \
	{                                                                                \
		return { a.mask | b };                                                       \
	}                                                                                \
	constexpr aw::core::EnumMask<T> operator|(T a, aw::core::EnumMask<T> b) noexcept \
	{                                                                                \
		return { a | b.mask };                                                       \
	}                                                                                \
	template <aw::core::ScopedEnum T>                                                \
	constexpr T operator~(T value) noexcept                                          \
	{                                                                                \
		return static_cast<T>(~std::to_underlying(value));                           \
	}
