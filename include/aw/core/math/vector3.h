#pragma once

#include "aw/core/math/math.h"
#include "aw/core/math/vector2.h"

#include <cassert>

namespace aw::core
{
	template <std::floating_point T>
	struct Vector3Template
	{
		using ComponentType = T;
		using VectorType = Vector3Template<T>;

		static constexpr bool is_32_bit_component = sizeof(T) == sizeof(f32);
		static constexpr bool is_64_bit_component = sizeof(T) == sizeof(f64);

		static constexpr VectorType zero() { return VectorType(0); }
		static constexpr VectorType one() { return VectorType(1); }

		static constexpr VectorType up() { return { 0, 1, 0 }; }
		static constexpr VectorType down() { return { 0, -1, 0 }; }
		static constexpr VectorType forward() { return { 0, 0, 1 }; }
		static constexpr VectorType backward() { return { 0, 0, -1 }; }
		static constexpr VectorType right() { return { 1, 0, 0 }; }
		static constexpr VectorType left() { return { -1, 0, 0 }; }

		ComponentType x;
		ComponentType y;
		ComponentType z;

		constexpr Vector3Template() noexcept
			: x(0)
			, y(0)
			, z(0)
		{
		}

		constexpr explicit Vector3Template(ComponentType scalar) noexcept
			: x(scalar)
			, y(scalar)
			, z(scalar)
		{
		}

		constexpr Vector3Template(ComponentType x, ComponentType y, ComponentType z) noexcept
			: x(x)
			, y(y)
			, z(z)
		{
		}

		constexpr Vector3Template(Vector2Template<ComponentType> xy, ComponentType z) noexcept
			: x(xy.x)
			, y(xy.y)
			, z(z)
		{
		}

		constexpr Vector3Template swizzle(const SwizzleIndex idx, const SwizzleIndex idy, const SwizzleIndex idz) const noexcept
		{
			return Vector3Template(
				data()[idx],
				data()[idy],
				data()[idz]);
		}

		constexpr Vector2Template<ComponentType> xy() const noexcept { return Vector2Template<ComponentType>(x, y); }

		constexpr const ComponentType* data() const noexcept { return &x; }
		constexpr ComponentType* data() noexcept { return &x; }

		constexpr ComponentType& operator[](usize index)
		{
			assert(index < 3);
			return data()[index];
		}

		constexpr const ComponentType& operator[](usize index) const
		{
			assert(index < 3);
			return data()[index];
		}

		constexpr ComponentType length_squared() const { return x * x + y * y + z * z; }
		CONSTEXPR_SQRT_CONSTEXPR ComponentType length() const noexcept { return Math::sqrt(length_squared()); }

		CONSTEXPR_SQRT_CONSTEXPR VectorType normalized() const { return *this / length(); }
		CONSTEXPR_SQRT_CONSTEXPR void normalize() { *this = normalized(); }

		constexpr bool is_zero() const
		{
			return Math::is_nearly_zero(x) && Math::is_nearly_zero(y) && Math::is_nearly_zero(z);
		}

		CONSTEXPR_SQRT_CONSTEXPR bool is_unit() const
		{
			return Math::is_nearly_equal(length(), ComponentType(1));
		}

		static constexpr ComponentType dot(const VectorType& a, const VectorType& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		static constexpr VectorType cross(const VectorType& a, const VectorType& b)
		{
			return Vector3Template(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		}

		static constexpr VectorType reflect(const VectorType& i, const VectorType& n)
		{
			return i - 2 * dot(i, n) * n;
		}

		static ComponentType angle(const VectorType& from, const VectorType& to)
		{
			return Math::acos(dot(from.normalized(), to.normalized()));
		}

		static ComponentType signed_angle(const VectorType& from, const VectorType& to, const VectorType& axis)
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
			const ComponentType z_lerp = Math::lerp(a.z, b.z, t);
			return VectorType(x_lerp, y_lerp, z_lerp);
		}

		constexpr VectorType operator+(const VectorType& rhs) const
		{
			return VectorType(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		constexpr VectorType operator-(const VectorType& rhs) const
		{
			return VectorType(x - rhs.x, y - rhs.y, z - rhs.z);
		}

		constexpr VectorType operator*(const VectorType& rhs) const
		{
			return VectorType(x * rhs.x, y * rhs.y, z * rhs.z);
		}

		constexpr VectorType operator/(const VectorType& rhs) const
		{
			return VectorType(x / rhs.x, y / rhs.y, z / rhs.z);
		}

		constexpr VectorType& operator+=(const VectorType& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		constexpr VectorType& operator-=(const VectorType& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}

		constexpr VectorType& operator*=(const VectorType& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}

		constexpr VectorType& operator/=(const VectorType& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}

		friend constexpr VectorType operator*(ComponentType lhs, const VectorType& rhs)
		{
			return VectorType(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
		}

		friend constexpr VectorType operator*(const VectorType& lhs, ComponentType rhs)
		{
			return VectorType(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
		}

		friend constexpr VectorType operator/(const VectorType& lhs, ComponentType rhs)
		{
			return VectorType(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
		}

		constexpr bool operator==(const VectorType& rhs) const
		{
			return Math::is_nearly_equal(x, rhs.x) && Math::is_nearly_equal(y, rhs.y) && Math::is_nearly_equal(z, rhs.z);
		}

		constexpr bool operator!=(const VectorType& rhs) const
		{
			return !(*this == rhs);
		}
	};

	using Vector3f = Vector3Template<f32>;
	static_assert(sizeof(Vector3f) == 12);

	using Vector3d = Vector3Template<f64>;
	static_assert(sizeof(Vector3d) == 24);

#ifndef AW_DEFAULT_FLOAT_TYPE
	using Vector3 = Vector3f;
#else
	using Vector3 = Vector3Template<AW_DEFAULT_FLOAT_TYPE>;
#endif
} // namespace aw::core