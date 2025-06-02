#pragma once

#include "aw/core/math/math.h"

namespace aw::core
{
	template <std::floating_point T>
	struct Vector3Template
	{
		using ComponentType = T;
		static constexpr bool is_32_bit_component = sizeof(T) == sizeof(f32);
		static constexpr bool is_64_bit_component = sizeof(T) == sizeof(f64);

		ComponentType x;
		ComponentType y;
		ComponentType z;

		constexpr Vector3Template()
			: x(0)
			, y(0)
			, z(0)
		{
		}

		constexpr Vector3Template(ComponentType scalar)
			: x(scalar)
			, y(scalar)
			, z(scalar)
		{
		}

		constexpr Vector3Template(ComponentType x, ComponentType y, ComponentType z)
			: x(x)
			, y(y)
			, z(z)
		{
		}

		constexpr const ComponentType* data() const
		{
			return &x;
		}

		constexpr ComponentType* data()
		{
			return &x;
		}

		constexpr ComponentType& operator[](usize index)
		{
			return data()[index];
		}

		constexpr ComponentType length_squared() const
		{
			return x * x + y * y + z * z;
		}

		constexpr ComponentType length() const
		{
			return Math::sqrt(length_squared());
		}

		constexpr Vector3Template normalized() const
		{
			return *this / length();
		}

		constexpr void normalize()
		{
			*this = normalized();
		}

		constexpr bool is_zero() const
		{
			return Math::is_nearly_zero(x) && Math::is_nearly_zero(y) && Math::is_nearly_zero(z);
		}

		static ComponentType dot(const Vector3Template& a, const Vector3Template& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		static Vector3Template cross(const Vector3Template& a, const Vector3Template& b)
		{
			return Vector3Template(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		}

		static Vector3Template reflect(const Vector3Template& i, const Vector3Template& n)
		{
			return i - 2 * dot(i, n) * n;
		}

		static Vector3Template lerp(const Vector3Template& a, const Vector3Template& b, ComponentType t)
		{
			const ComponentType x_lerp = Math::lerp(a.x, b.x, t);
			const ComponentType y_lerp = Math::lerp(a.y, b.y, t);
			const ComponentType z_lerp = Math::lerp(a.z, b.z, t);
			return Vector3Template(x_lerp, y_lerp, z_lerp);
		}

		constexpr Vector3Template operator+(const Vector3Template& rhs) const
		{
			return Vector3Template(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		constexpr Vector3Template operator-(const Vector3Template& rhs) const
		{
			return Vector3Template(x - rhs.x, y - rhs.y, z - rhs.z);
		}

		constexpr Vector3Template operator*(const Vector3Template& rhs) const
		{
			return Vector3Template(x * rhs.x, y * rhs.y, z * rhs.z);
		}

		constexpr Vector3Template operator/(const Vector3Template& rhs) const
		{
			return Vector3Template(x / rhs.x, y / rhs.y, z / rhs.z);
		}

		constexpr Vector3Template& operator+=(const Vector3Template& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}

		constexpr Vector3Template& operator-=(const Vector3Template& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}

		constexpr Vector3Template& operator*=(const Vector3Template& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}

		constexpr Vector3Template& operator/=(const Vector3Template& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}

		friend constexpr Vector3Template operator*(ComponentType lhs, const Vector3Template& rhs)
		{
			return Vector3Template(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
		}

		friend constexpr Vector3Template operator*(const Vector3Template& lhs, ComponentType rhs)
		{
			return Vector3Template(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
		}

		friend constexpr Vector3Template operator/(const Vector3Template& lhs, ComponentType rhs)
		{
			return Vector3Template(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
		}

		bool operator==(const Vector3Template& rhs) const
		{
			return Math::is_nearly_equal(x, rhs.x) && Math::is_nearly_equal(y, rhs.y) && Math::is_nearly_equal(z, rhs.z);
		}

		constexpr bool operator!=(const Vector3Template& rhs) const
		{
			return !(*this == rhs);
		}
	};

	using Vector3f = Vector3Template<f32>;
	using Vector3d = Vector3Template<f64>;

#ifndef AW_DEFAULT_FLOAT_TYPE
	using Vector3 = Vector3f;
#else
	using Vector3 = Vector3Template<AW_DEFAULT_FLOAT_TYPE>;
#endif
} // namespace aw::core

#include "vector3.inl"