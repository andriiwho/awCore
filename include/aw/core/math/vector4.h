#pragma once

#include "aw/core/math/math.h"
#include "aw/core/math/vector3.h"

#include <cassert>

namespace aw::core
{
	template <std::floating_point T>
	class Vector4Template
	{
		using ComponentType = T;
		using VectorType = Vector4Template<T>;

		static constexpr VectorType zero() { return VectorType(0); }
		static constexpr VectorType one() { return VectorType(1); }

		static constexpr VectorType up() { return { 0, 1, 0, 1 }; }
		static constexpr VectorType down() { return { 0, -1, 0, 1 }; }
		static constexpr VectorType left() { return { -1, 0, 0, 1 }; }
		static constexpr VectorType right() { return { 1, 0, 0, 1 }; }
		static constexpr VectorType forward() { return { 0, 0, 1, 1 }; }
		static constexpr VectorType backward() { return { 0, 0, -1, 1 }; }

		ComponentType x;
		ComponentType y;
		ComponentType z;
		ComponentType w;

		constexpr Vector4Template() noexcept
			: x(0)
			, y(0)
			, z(0)
			, w(0)
		{
		}

		constexpr explicit Vector4Template(ComponentType scalar) noexcept
			: x(scalar)
			, y(scalar)
			, z(scalar)
			, w(scalar)
		{
		}

		constexpr Vector4Template(ComponentType x, ComponentType y, ComponentType z, ComponentType w) noexcept
			: x(x)
			, y(y)
			, z(z)
			, w(w)
		{
		}

		constexpr Vector4Template(const Vector3Template<ComponentType>& xyz, ComponentType w) noexcept
			: x(xyz.x)
			, y(xyz.y)
			, z(xyz.z)
			, w(w)
		{
		}

		constexpr Vector3Template<ComponentType> xyz() const noexcept { return Vector3Template<ComponentType>(x, y, z); }
		constexpr Vector3Template<ComponentType> homogenized() const noexcept { return Vector3Template<ComponentType>(x, y, z) / w; }

		constexpr const ComponentType* data() const noexcept { return &x; }
		constexpr ComponentType* data() noexcept { return &x; }

		constexpr ComponentType& operator[](usize index)
		{
			assert(index < 4);
			return data()[index];
		}

		constexpr const ComponentType& operator[](usize index) const
		{
			assert(index < 4);
			return data()[index];
		}

		constexpr ComponentType length_squared() const { return x * x + y * y + z * z + w * w; }
		CONSTEXPR_SQRT_CONSTEXPR ComponentType length() const noexcept { return Math::sqrt(length_squared()); }

		CONSTEXPR_SQRT_CONSTEXPR VectorType normalized() const
		{
			const auto my_length = length();
			auto out_x = x / my_length;
			auto out_y = y / my_length;
			auto out_z = z / my_length;
			auto out_w = w / my_length;
			return VectorType(out_x, out_y, out_z, out_w);
		}

		CONSTEXPR_SQRT_CONSTEXPR void normalize() { *this = normalized(); }

		constexpr bool is_zero() const
		{
			return Math::is_nearly_zero(x) && Math::is_nearly_zero(y) && Math::is_nearly_zero(z) && Math::is_nearly_zero(w);
		}

		CONSTEXPR_SQRT_CONSTEXPR bool is_unit() const
		{
			return Math::is_nearly_equal(length(), ComponentType(1));
		}

		static constexpr ComponentType dot(const VectorType& a, const VectorType& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}

		static constexpr VectorType lerp(const VectorType& a, const VectorType& b, ComponentType t)
		{
			const ComponentType x_lerp = Math::lerp(a.x, b.x, t);
			const ComponentType y_lerp = Math::lerp(a.y, b.y, t);
			const ComponentType z_lerp = Math::lerp(a.z, b.z, t);
			const ComponentType w_lerp = Math::lerp(a.w, b.w, t);
			return VectorType(x_lerp, y_lerp, z_lerp, w_lerp);
		}

		constexpr VectorType operator+(const VectorType& rhs) const
		{
			return VectorType(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}

		constexpr VectorType operator-(const VectorType& rhs) const
		{
			return VectorType(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
		}

		constexpr VectorType operator*(const VectorType& rhs) const
		{
			return VectorType(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
		}

		constexpr VectorType operator/(const VectorType& rhs) const
		{
			return VectorType(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
		}

		constexpr VectorType& operator+=(const VectorType& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}

		constexpr VectorType& operator-=(const VectorType& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}

		constexpr VectorType& operator*=(const VectorType& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w *= rhs.w;
			return *this;
		}

		constexpr VectorType& operator/=(const VectorType& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}

		friend constexpr VectorType operator*(ComponentType lhs, const VectorType& rhs)
		{
			return VectorType(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
		}

		friend constexpr VectorType operator*(const VectorType& lhs, ComponentType rhs)
		{
			return VectorType(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
		}

		friend constexpr VectorType operator/(const VectorType& lhs, ComponentType rhs)
		{
			return VectorType(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
		}

		constexpr bool operator==(const VectorType& lhs, const VectorType& rhs)
		{
			return Math::is_nearly_equal(x, rhs.x) && Math::is_nearly_equal(y, rhs.y) && Math::is_nearly_equal(z, rhs.z) && Math::is_nearly_equal(w, rhs.w);
		}

		constexpr bool operator!=(const VectorType& lhs, const VectorType& rhs)
		{
			return !(lhs == rhs);
		}
	};

	using Vector4f = Vector4Template<f32>;
	static_assert(sizeof(Vector4f) == 16);

	using Vector4d = Vector4Template<f64>;
	static_assert(sizeof(Vector4d) == 32);

#ifndef AW_DEFAULT_FLOAT_TYPE
	using Vector4 = Vector4f;
#else
	using Vector4 = Vector4Template<AW_DEFAULT_FLOAT_TYPE>;
#endif

} // namespace aw::core