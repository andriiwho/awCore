#pragma once

#include "aw/core/math/math.h"

#include <cassert>

namespace aw::core
{
	template <std::floating_point T>
	struct Vector2Template
	{
		using ComponentType = T;
		using VectorType = Vector2Template<T>;

		static constexpr VectorType zero() { return VectorType(0); }
		static constexpr VectorType one() { return VectorType(1); }

		ComponentType x;
		ComponentType y;

		constexpr Vector2Template() noexcept
			: x(0)
			, y(0)
		{
		}

		constexpr explicit Vector2Template(ComponentType scalar) noexcept
			: x(scalar)
			, y(scalar)
		{
		}

		constexpr Vector2Template(ComponentType x, ComponentType y) noexcept
			: x(x)
			, y(y)
		{
		}

		constexpr const ComponentType* data() const noexcept { return &x; }
		constexpr ComponentType* data() noexcept { return &x; }

		constexpr ComponentType& operator[](usize index)
		{
			assert(index < 2);
			return data()[index];
		}

		constexpr const ComponentType& operator[](usize index) const
		{
			assert(index < 2);
			return data()[index];
		}

		constexpr ComponentType length_squared() const { return x * x + y * y; }
		CONSTEXPR_SQRT_CONSTEXPR ComponentType length() const noexcept { return Math::sqrt(length_squared()); }

		CONSTEXPR_SQRT_CONSTEXPR VectorType normalized() const { return *this / length(); }
		CONSTEXPR_SQRT_CONSTEXPR void normalize() { *this = normalized(); }

		constexpr bool is_zero() const
		{
			return Math::is_nearly_zero(x) && Math::is_nearly_zero(y);
		}

		CONSTEXPR_SQRT_CONSTEXPR bool is_unit() const
		{
			return Math::is_nearly_equal(length(), ComponentType(1));
		}

		static constexpr ComponentType dot(const VectorType& a, const VectorType& b)
		{
			return a.x * b.x + a.y * b.y;
		}

		static constexpr VectorType reflect(const VectorType& i, const VectorType& n)
		{
			return i - 2 * dot(i, n) * n;
		}

		static ComponentType angle(const VectorType& from, const VectorType& to)
		{
			return Math::acos(dot(from.normalized(), to.normalized()));
		}

		static constexpr ComponentType signed_angle(const VectorType& from, const VectorType& to, const VectorType& axis)
		{
			const ComponentType unsigned_angle = angle(from, to);
			const VectorType cross_product = cross(from, to);
			const ComponentType sign = Math::sign(dot(axis, cross_product));
			return unsigned_angle * sign;
		}

		constexpr VectorType project_onto_plane(const VectorType& normal) const
		{
			return *this - normal * dot(*this, normal);
		}

		constexpr VectorType project_onto_vector(const VectorType& direction) const
		{
			return direction * (dot(*this, direction) / direction.length_squared());
		}

		static constexpr VectorType lerp(const VectorType& a, const VectorType& b, ComponentType t)
		{
			const ComponentType x_lerp = Math::lerp(a.x, b.x, t);
			const ComponentType y_lerp = Math::lerp(a.y, b.y, t);
			return VectorType(x_lerp, y_lerp);
		}

		constexpr VectorType operator+(const VectorType& rhs) const
		{
			return VectorType(x + rhs.x, y + rhs.y);
		}

		constexpr VectorType operator-(const VectorType& rhs) const
		{
			return VectorType(x - rhs.x, y - rhs.y);
		}

		constexpr VectorType operator*(const VectorType& rhs) const
		{
			return VectorType(x * rhs.x, y * rhs.y);
		}

		constexpr VectorType operator/(const VectorType& rhs) const
		{
			return VectorType(x / rhs.x, y / rhs.y);
		}

		constexpr VectorType& operator+=(const VectorType& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		constexpr VectorType& operator-=(const VectorType& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		constexpr VectorType& operator*=(const VectorType& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}

		constexpr VectorType& operator/=(const VectorType& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}

		friend constexpr VectorType operator*(ComponentType lhs, const VectorType& rhs)
		{
			return VectorType(lhs * rhs.x, lhs * rhs.y);
		}

		friend constexpr VectorType operator*(const VectorType& lhs, ComponentType rhs)
		{
			return VectorType(lhs.x * rhs, lhs.y * rhs);
		}

		friend constexpr VectorType operator/(const VectorType& lhs, ComponentType rhs)
		{
			return VectorType(lhs.x / rhs, lhs.y / rhs);
		}

		bool operator==(const VectorType& rhs) const
		{
			return Math::is_nearly_equal(x, rhs.x) && Math::is_nearly_equal(y, rhs.y);
		}

		constexpr bool operator!=(const VectorType& rhs) const
		{
			return !(*this == rhs);
		}
	};

	
	using Vector2f = Vector2Template<f32>;
	static_assert(sizeof(Vector2f) == 8);

	using Vector2d = Vector2Template<f64>;
	static_assert(sizeof(Vector2d) == 16);

	#ifndef AW_DEFAULT_FLOAT_TYPE
	using Vector2 = Vector2f;
	#else
	using Vector2 = Vector2Template<AW_DEFAULT_FLOAT_TYPE>;
	#endif
} // namespace aw::core