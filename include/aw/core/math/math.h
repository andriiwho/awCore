#pragma once

#include <algorithm>

#include "aw/core/primitive/numbers.h"

#include <cmath>
#include <concepts>
#include <thread>
#include <array>

namespace aw::core
{
	enum SwizzleIndex : u8
	{
		SWIZZLE_INDEX_X = 0,
		SWIZZLE_INDEX_Y = 1,
		SWIZZLE_INDEX_Z = 2,
		SWIZZLE_INDEX_W = 3,

		SWIZZLE_INDEX_R = 0,
		SWIZZLE_INDEX_G = 1,
		SWIZZLE_INDEX_B = 2,
		SWIZZLE_INDEX_A = 3,
	};

	struct Random
	{
		using State = std::array<u64, 4>;

		static inline thread_local State state = { 1, 2, 3, 4 };

		static constexpr void seed(const u64 seed) noexcept
		{
			state[0] = seed;
			state[1] = seed ^ 0x1234567890abcdef;
			state[2] = seed ^ 0xfedcba0987654321;
			state[3] = seed ^ 0x0f1e2d3c4b5a6978;
		}

		static constexpr void seed(const std::thread::id thread_id) noexcept
		{
			seed(std::hash<std::thread::id>{}(thread_id));
		}

		static constexpr u64 next_u64() noexcept
		{
			const u64 result = rotl(state[1] * 5, 7) * 9;
			const u64 t = state[1] << 17;

			state[2] ^= state[0];
			state[3] ^= state[1];
			state[1] ^= state[2];
			state[0] ^= state[3];

			state[2] ^= t;
			state[3] = rotl(state[3], 45);

			return result;
		}

		static constexpr u32 next_u32() noexcept
		{
			return static_cast<u32>(next_u64());
		}

		static constexpr f32 next_f32() noexcept
		{
			return static_cast<f32>(next_u32()) / static_cast<f32>(0xFFFFFFFF);
		}

		static constexpr f64 next_f64() noexcept
		{
			return static_cast<f64>(next_u64()) / static_cast<f64>(0xFFFFFFFFFFFFFFFF);
		}

		template <typename T>
		static constexpr T next_range(T min, T max) noexcept
		{
			if constexpr (std::is_same_v<T, f32>)
			{
				return min + next_f32() * (max - min);
			}
			else if constexpr (std::is_same_v<T, f64>)
			{
				return min + next_f64() * (max - min);
			}
			else if constexpr (std::is_integral_v<T>)
			{
				return min + static_cast<T>(next_u64() % static_cast<u64>(max - min + 1));
			}
			else
				return 0;
		}

	private:
		static constexpr u64 rotl(const u64 x, const int k) noexcept
		{
			return (x << k) | (x >> (64 - k));
		}
	};

	/**
	 *	Class with basic math operations.
	 *
	 *	TODO(andreiwho): Make non-constexpr functions constexpr if it's possible. If not, make constexpr versions of them.
	 *
	 */

	namespace detail
	{
		static constexpr f32 EPSILON = 0.00001f;
		static constexpr f64 EPSILON64 = 0.00001;

		template <Arithmetic T>
		static constexpr T constexpr_abs(T value)
		{
			return value < T(0) ? -value : value;
		}

#define USE_CONSTEXPR_SQRT 1
#if USE_CONSTEXPR_SQRT
	#define CONSTEXPR_SQRT_CONSTEXPR constexpr
#else
	#define CONSTEXPR_SQRT_CONSTEXPR
#endif

		template <std::floating_point T>
		constexpr T constexpr_sqrt(T value)
		{
			if (value < T(0))
			{
				return std::numeric_limits<T>::quiet_NaN();
			}

			if (value == T(0) || value == T(1))
			{
				return value;
			}

			T guess = value;
			T previous = 0;

			// Newton iterations
			for (i32 i = 0; i < 32 && constexpr_abs(guess - previous) > T(EPSILON64); ++i)
			{
				previous = guess;
				guess = T(0.5) * (guess + value / guess);
			}

			return guess;
		}

		/** 64-bit log table */
		inline constexpr u64 log2_table_64[64]{
			63, 0, 58, 1, 59, 47, 53, 2,
			60, 39, 48, 27, 54, 33, 42, 3,
			61, 51, 37, 40, 49, 18, 28, 20,
			55, 30, 34, 11, 43, 14, 22, 4,
			62, 57, 46, 52, 38, 26, 32, 41,
			50, 36, 17, 19, 29, 10, 13, 21,
			56, 45, 25, 31, 35, 16, 9, 12,
			44, 24, 15, 8, 23, 7, 6, 5
		};

		inline constexpr u32 log2_table_32[32]{
			0, 9, 1, 10, 13, 21, 2, 29,
			11, 14, 16, 18, 22, 25, 3, 30,
			8, 12, 20, 28, 15, 17, 24, 7,
			19, 27, 23, 6, 26, 5, 4, 31
		};
	} // namespace detail

	struct Math
	{
		static constexpr f32 PI = 3.14159265358979323846f;
		static constexpr f64 PI64 = 3.14159265358979323846;

		static constexpr f32 DEG_TO_RAD = PI / 180.0f;
		static constexpr f32 RAD_TO_DEG = 180.0f / PI;

		static constexpr f64 DEG_TO_RAD64 = PI64 / 180.0;
		static constexpr f64 RAD_TO_DEG64 = 180.0 / PI64;

		static constexpr f32 EPSILON = detail::EPSILON;
		static constexpr f64 EPSILON64 = detail::EPSILON64;

		template <std::floating_point T>
		static T sin(T value)
		{
			return std::sin(value);
		}

		template <std::floating_point T>
		static T cos(T value)
		{
			return std::cos(value);
		}

		template <std::floating_point T>
		static T tan(T value)
		{
			return std::tan(value);
		}

		template <std::floating_point T>
		static T asin(T value)
		{
			return std::asin(value);
		}

		template <std::floating_point T>
		static T acos(T value)
		{
			return std::acos(value);
		}

		template <std::floating_point T>
		static T atan(T value)
		{
			return std::atan(value);
		}

		template <std::floating_point T>
		static T atan2(T y, T x)
		{
			return std::atan2(y, x);
		}

		template <std::floating_point T>
		static CONSTEXPR_SQRT_CONSTEXPR T sqrt(T value)
		{
#if USE_CONSTEXPR_SQRT
			return detail::constexpr_sqrt(value);
#else
			return std::sqrt(value);
#endif
		}

		template <std::floating_point T>
		static T pow(T base, T exponent)
		{
			return std::pow(base, exponent);
		}

		template <Arithmetic T>
		static constexpr T abs(T value)
		{
			return detail::constexpr_abs(value);
		}

		template <std::floating_point T>
		static T floor(T value)
		{
			return std::floor(value);
		}

		template <std::floating_point T>
		static T ceil(T value)
		{
			return std::ceil(value);
		}

		template <std::floating_point T>
		static T round(T value)
		{
			return std::round(value);
		}

		template <std::floating_point T>
		static T fmod(T x, T y)
		{
			return std::fmod(x, y);
		}

		template <Arithmetic T>
		static T min(T a, T b)
		{
			return std::min(a, b);
		}

		template <Arithmetic T>
		static T max(T a, T b)
		{
			return std::max(a, b);
		}

		template <Arithmetic T>
		static constexpr T clamp(T value, T min, T max)
		{
			return std::clamp(value, min, max);
		}

		template <Arithmetic T>
		static constexpr T lerp(T a, T b, T t)
		{
			return a + (b - a) * t;
		}

		template <Arithmetic T>
		static constexpr T lerp_clamped(T a, T b, T t)
		{
			return clamp(lerp(a, b, t), a, b);
		}

		template <Arithmetic T>
		static constexpr T lerp_inverse(T a, T b, T t)
		{
			return clamp((t - a) / (b - a), 0.0f, 1.0f);
		}

		template <Arithmetic T>
		static constexpr T lerp_inverse_clamped(T a, T b, T t)
		{
			return clamp(lerp_inverse(a, b, t), 0.0f, 1.0f);
		}

		template <std::floating_point T>
		static constexpr T sign(T value)
		{
			return std::signbit(value) ? T(-1) : T(1);
		}

		template <std::floating_point T>
		static constexpr T sign_inverse(T value)
		{
			return std::signbit(value) ? T(1) : T(-1);
		}

		template <std::floating_point T>
		static constexpr bool is_nearly_equal(T a, T b)
		{
			return abs(a - b) < ((sizeof(T) == sizeof(f32) ? EPSILON : EPSILON64));
		}

		template <std::floating_point T>
		static constexpr bool is_nearly_equal(T a, T b, T epsilon)
		{
			return abs(a - b) < epsilon;
		}

		template <std::floating_point T>
		static constexpr bool is_nearly_zero(T value)
		{
			return abs(value) < ((sizeof(T) == sizeof(f32)) ? EPSILON : EPSILON64);
		}


		static constexpr u64 fast_log2(const u64 value)
		{
			u64 mod_value = value;
			mod_value |= mod_value >> 1;
			mod_value |= mod_value >> 2;
			mod_value |= mod_value >> 4;
			mod_value |= mod_value >> 8;
			mod_value |= mod_value >> 16;
			mod_value |= mod_value >> 32;
			return detail::log2_table_64[to_u64((mod_value - (mod_value >> 1)) * 0x07edd5e59a4e28c2) >> 58];
		}

		static constexpr u32 fast_log2(const u32 value)
		{
			u32 mod_value = value;
			mod_value |= mod_value >> 1;
			mod_value |= mod_value >> 2;
			mod_value |= mod_value >> 4;
			mod_value |= mod_value >> 8;
			mod_value |= mod_value >> 16;
			return detail::log2_table_32[to_u32(mod_value * 0x07c4acdd) >> 27];
		}

		static constexpr u64 align_to_pow2(u64 value)
		{
			u64 count = 0;
			if (value && !(value & (value - 1)))
				return value;

			while (value != 0)
			{
				value >>= 1;
				count++;
			}

			return 1ull << count;
		}
	};
} // namespace aw::core