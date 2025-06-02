#pragma once

#include "aw/core/math/math.h"

#include <cstdlib>

namespace aw::core
{
	/**
	 *
	 * Row major matrix
	 *
	 */
	template <std::floating_point T>
	struct MatrixTemplate
	{
		using ComponentType = T;
		using MatrixType = MatrixTemplate<T>;

		ComponentType m[4 * 4];

		constexpr MatrixTemplate() noexcept
			: m()
		{
		}

		constexpr MatrixTemplate(ComponentType m00, ComponentType m01, ComponentType m02, ComponentType m03,
			ComponentType m10, ComponentType m11, ComponentType m12, ComponentType m13,
			ComponentType m20, ComponentType m21, ComponentType m22, ComponentType m23,
			ComponentType m30, ComponentType m31, ComponentType m32, ComponentType m33) noexcept
			: m{ m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33 }
		{
		}

		constexpr explicit MatrixTemplate(ComponentType scalar) noexcept
			: m()
		{
			m[0] = m[5] = m[10] = m[15] = scalar;
		}

		static constexpr MatrixType identity()
		{
			return MatrixType(ComponentType(1));
		}

		constexpr bool is_identity() const
		{
			return Math::is_nearly_equal(m[0], ComponentType(1)) && Math::is_nearly_equal(m[1], ComponentType(0)) && Math::is_nearly_equal(m[2], ComponentType(0)) && Math::is_nearly_equal(m[3], ComponentType(0))
				&& Math::is_nearly_equal(m[4], ComponentType(0)) && Math::is_nearly_equal(m[5], ComponentType(1)) && Math::is_nearly_equal(m[6], ComponentType(0)) && Math::is_nearly_equal(m[7], ComponentType(0))
				&& Math::is_nearly_equal(m[8], ComponentType(0)) && Math::is_nearly_equal(m[9], ComponentType(0)) && Math::is_nearly_equal(m[10], ComponentType(1)) && Math::is_nearly_equal(m[11], ComponentType(0))
				&& Math::is_nearly_equal(m[12], ComponentType(0)) && Math::is_nearly_equal(m[13], ComponentType(0)) && Math::is_nearly_equal(m[14], ComponentType(0)) && Math::is_nearly_equal(m[15], ComponentType(1));
		}

		constexpr MatrixType transpose() const
		{
			return MatrixTemplate(
				m[0], m[4], m[8], m[12],
				m[1], m[5], m[9], m[13],
				m[2], m[6], m[10], m[14],
				m[3], m[7], m[11], m[15]);
		}

		constexpr MatrixType inverse() const
		{
			const auto& m00 = m[0];

			const ComponentType* a = &m[0];
			MatrixType inv{};
			ComponentType* o = &inv.m[0];

			// CSE
			const auto a0_a5 = a[0] * a[5];
			const auto a0_a6 = a[0] * a[6];
			const auto a0_a7 = a[0] * a[7];
			const auto a0_a9 = a[0] * a[9];
			const auto a0_a10 = a[0] * a[10];
			const auto a0_a11 = a[0] * a[11];
			const auto a4_a9 = a[4] * a[9];
			const auto a4_a10 = a[4] * a[10];
			const auto a8_a5 = a[8] * a[5];
			const auto a8_a6 = a[8] * a[6];
			const auto a12_a5 = a[12] * a[5];
			const auto a12_a6 = a[12] * a[6];

			const auto a5_a10 = a[5] * a[10];
			const auto a5_a11 = a[5] * a[11];
			const auto a9_a6 = a[9] * a[6];
			const auto a9_a7 = a[9] * a[7];
			const auto a13_a6 = a[13] * a[6];
			const auto a13_a7 = a[13] * a[7];
			const auto a13_a10 = a[13] * a[10];
			const auto a13_a11 = a[13] * a[11];
			const auto a8_a7 = a[8] * a[7];
			const auto a12_a7 = a[12] * a[7];
			const auto a12_a11 = a[12] * a[11];

			o[0] = a5_a10 * a[15] - a5_a11 * a[14] - a9_a6 * a[15] + a9_a7 * a[14] + a13_a6 * a[11] - a13_a7 * a[10];
			o[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] - a[9] * a[3] * a[14] - a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
			o[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] + a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
			o[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] - a[5] * a[3] * a[10] - a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
			o[4] = -a4_a10 * a[15] + a[4] * a[11] * a[14] + a8_a6 * a[15] - a8_a7 * a[14] - a12_a6 * a[11] + a12_a7 * a[10];
			o[5] = a0_a10 * a[15] - a0_a11 * a[14] - a[8] * a[2] * a[15] + a[8] * a[3] * a[14] + a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
			o[6] = -a0_a6 * a[15] + a0_a7 * a[14] + a[4] * a[2] * a[15] - a[4] * a[3] * a[14] - a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
			o[7] = a0_a6 * a[11] - a0_a7 * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] + a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
			o[8] = a4_a9 * a[15] - a[4] * a[11] * a[13] - a8_a5 * a[15] + a8_a7 * a[13] + a12_a5 * a[11] - a12_a7 * a[9];
			o[9] = -a0_a9 * a[15] + a0_a11 * a[13] + a[8] * a[1] * a[15] - a[8] * a[3] * a[13] - a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
			o[10] = a0_a5 * a[15] - a0_a7 * a[13] - a[4] * a[1] * a[15] + a[4] * a[3] * a[13] + a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
			o[11] = -a0_a5 * a[11] + a0_a7 * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] - a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
			o[12] = -a4_a9 * a[14] + a4_a10 * a[13] + a8_a5 * a[14] - a8_a6 * a[13] - a12_a5 * a[10] + a12_a6 * a[9];
			o[13] = a0_a9 * a[14] - a0_a10 * a[13] - a[8] * a[1] * a[14] + a[8] * a[2] * a[13] + a[12] * a[1] * a[10] - a[12] * a[2] * a[9];
			o[14] = -a0_a5 * a[14] + a0_a6 * a[13] + a[4] * a[1] * a[14] - a[4] * a[2] * a[13] - a[12] * a[1] * a[6] + a[12] * a[2] * a[5];
			o[15] = a0_a5 * a[10] - a0_a6 * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] + a[8] * a[1] * a[6] - a[8] * a[2] * a[5];

			T det = a[0] * o[0] + a[1] * o[4] + a[2] * o[8] + a[3] * o[12];
			if (Math::is_nearly_zero(det))
				return identity();

			T inv_det = T(1) / det;
			for (int i = 0; i < 16; ++i)
				o[i] *= inv_det;

			return inv;
		}

		constexpr MatrixType operator*(const MatrixType& rhs) const
		{
			return MatrixTemplate(
				m[0] * rhs.m[0] + m[1] * rhs.m[4] + m[2] * rhs.m[8] + m[3] * rhs.m[12],
				m[0] * rhs.m[1] + m[1] * rhs.m[5] + m[2] * rhs.m[9] + m[3] * rhs.m[13],
				m[0] * rhs.m[2] + m[1] * rhs.m[6] + m[2] * rhs.m[10] + m[3] * rhs.m[14],
				m[0] * rhs.m[3] + m[1] * rhs.m[7] + m[2] * rhs.m[11] + m[3] * rhs.m[15],

				m[4] * rhs.m[0] + m[5] * rhs.m[4] + m[6] * rhs.m[8] + m[7] * rhs.m[12],
				m[4] * rhs.m[1] + m[5] * rhs.m[5] + m[6] * rhs.m[9] + m[7] * rhs.m[13],
				m[4] * rhs.m[2] + m[5] * rhs.m[6] + m[6] * rhs.m[10] + m[7] * rhs.m[14],
				m[4] * rhs.m[3] + m[5] * rhs.m[7] + m[6] * rhs.m[11] + m[7] * rhs.m[15],

				m[8] * rhs.m[0] + m[9] * rhs.m[4] + m[10] * rhs.m[8] + m[11] * rhs.m[12],
				m[8] * rhs.m[1] + m[9] * rhs.m[5] + m[10] * rhs.m[9] + m[11] * rhs.m[13],
				m[8] * rhs.m[2] + m[9] * rhs.m[6] + m[10] * rhs.m[10] + m[11] * rhs.m[14],
				m[8] * rhs.m[3] + m[9] * rhs.m[7] + m[10] * rhs.m[11] + m[11] * rhs.m[15],

				m[12] * rhs.m[0] + m[13] * rhs.m[4] + m[14] * rhs.m[8] + m[15] * rhs.m[12],
				m[12] * rhs.m[1] + m[13] * rhs.m[5] + m[14] * rhs.m[9] + m[15] * rhs.m[13],
				m[12] * rhs.m[2] + m[13] * rhs.m[6] + m[14] * rhs.m[10] + m[15] * rhs.m[14],
				m[12] * rhs.m[3] + m[13] * rhs.m[7] + m[14] * rhs.m[11] + m[15] * rhs.m[15]);
		}

		constexpr Vector4Template<ComponentType> operator*(const Vector4Template<ComponentType>& rhs) const
		{
			return Vector4Template<ComponentType>(
				m[0] * rhs.x + m[1] * rhs.y + m[2] * rhs.z + m[3] * rhs.w,
				m[4] * rhs.x + m[5] * rhs.y + m[6] * rhs.z + m[7] * rhs.w,
				m[8] * rhs.x + m[9] * rhs.y + m[10] * rhs.z + m[11] * rhs.w,
				m[12] * rhs.x + m[13] * rhs.y + m[14] * rhs.z + m[15] * rhs.w);
		}

		// This is a slow operation. Should be avoided in performance-critical code.
		constexpr bool operator==(const MatrixType& rhs) const
		{
			return Math::is_nearly_equal(m[0], rhs.m[0]) && Math::is_nearly_equal(m[1], rhs.m[1]) && Math::is_nearly_equal(m[2], rhs.m[2]) && Math::is_nearly_equal(m[3], rhs.m[3])
				&& Math::is_nearly_equal(m[4], rhs.m[4]) && Math::is_nearly_equal(m[5], rhs.m[5]) && Math::is_nearly_equal(m[6], rhs.m[6]) && Math::is_nearly_equal(m[7], rhs.m[7])
				&& Math::is_nearly_equal(m[8], rhs.m[8]) && Math::is_nearly_equal(m[9], rhs.m[9]) && Math::is_nearly_equal(m[10], rhs.m[10]) && Math::is_nearly_equal(m[11], rhs.m[11])
				&& Math::is_nearly_equal(m[12], rhs.m[12]) && Math::is_nearly_equal(m[13], rhs.m[13]) && Math::is_nearly_equal(m[14], rhs.m[14]) && Math::is_nearly_equal(m[15], rhs.m[15]);
		}
	};

	template <std::floating_point T>
	struct Matrix3Template
	{
		using ComponentType = T;
		using MatrixType = Matrix3Template<T>;

		ComponentType m[3 * 3];

		constexpr Matrix3Template() noexcept
			: m()
		{
		}

		constexpr Matrix3Template(ComponentType m00, ComponentType m01, ComponentType m02,
			ComponentType m10, ComponentType m11, ComponentType m12,
			ComponentType m20, ComponentType m21, ComponentType m22) noexcept
			: m{ m00, m01, m02, m10, m11, m12, m20, m21, m22 }
		{
		}

		constexpr explicit Matrix3Template(ComponentType scalar) noexcept
			: m()
		{
			m[0] = m[4] = m[8] = scalar;
		}

		static constexpr MatrixType identity()
		{
			return MatrixType(ComponentType(1));
		}

		constexpr bool is_identity() const
		{
			return Math::is_nearly_equal(m[0], ComponentType(1)) && Math::is_nearly_equal(m[1], ComponentType(0)) && Math::is_nearly_equal(m[2], ComponentType(0))
				&& Math::is_nearly_equal(m[3], ComponentType(0)) && Math::is_nearly_equal(m[4], ComponentType(1)) && Math::is_nearly_equal(m[5], ComponentType(0))
				&& Math::is_nearly_equal(m[6], ComponentType(0)) && Math::is_nearly_equal(m[7], ComponentType(0)) && Math::is_nearly_equal(m[8], ComponentType(1));
		}

		constexpr MatrixType transpose() const
		{
			return Matrix3Template(
				m[0], m[3], m[6],
				m[1], m[4], m[7],
				m[2], m[5], m[8]);
		}

		constexpr MatrixType inverse() const
		{
			T det = m[0] * (m[4] * m[8] - m[5] * m[7])
				- m[1] * (m[3] * m[8] - m[5] * m[6])
				+ m[2] * (m[3] * m[7] - m[4] * m[6]);

			if (Math::is_nearly_zero(det))
				return identity();

			T inv_det = T(1) / det;

			return Matrix3Template(
				(m[4] * m[8] - m[5] * m[7]) * inv_det,
				(m[2] * m[7] - m[1] * m[8]) * inv_det,
				(m[1] * m[5] - m[2] * m[4]) * inv_det,
				(m[5] * m[6] - m[3] * m[8]) * inv_det,
				(m[0] * m[8] - m[2] * m[6]) * inv_det,
				(m[2] * m[3] - m[0] * m[5]) * inv_det,
				(m[3] * m[7] - m[4] * m[6]) * inv_det,
				(m[1] * m[6] - m[0] * m[7]) * inv_det,
				(m[0] * m[4] - m[1] * m[3]) * inv_det);
		}

		constexpr MatrixType operator*(const MatrixType& rhs) const
		{
			return Matrix3Template(
				m[0] * rhs.m[0] + m[1] * rhs.m[3] + m[2] * rhs.m[6],
				m[0] * rhs.m[1] + m[1] * rhs.m[4] + m[2] * rhs.m[7],
				m[0] * rhs.m[2] + m[1] * rhs.m[5] + m[2] * rhs.m[8],

				m[3] * rhs.m[0] + m[4] * rhs.m[3] + m[5] * rhs.m[6],
				m[3] * rhs.m[1] + m[4] * rhs.m[4] + m[5] * rhs.m[7],
				m[3] * rhs.m[2] + m[4] * rhs.m[5] + m[5] * rhs.m[8],

				m[6] * rhs.m[0] + m[7] * rhs.m[3] + m[8] * rhs.m[6],
				m[6] * rhs.m[1] + m[7] * rhs.m[4] + m[8] * rhs.m[7],
				m[6] * rhs.m[2] + m[7] * rhs.m[5] + m[8] * rhs.m[8]);
		}

		constexpr Vector3Template<ComponentType> operator*(const Vector3Template<ComponentType>& rhs) const
		{
			return Vector3Template<ComponentType>(
				m[0] * rhs.x + m[1] * rhs.y + m[2] * rhs.z,
				m[3] * rhs.x + m[4] * rhs.y + m[5] * rhs.z,
				m[6] * rhs.x + m[7] * rhs.y + m[8] * rhs.z);
		}

		constexpr bool operator==(const MatrixType& rhs) const
		{
			return Math::is_nearly_equal(m[0], rhs.m[0]) && Math::is_nearly_equal(m[1], rhs.m[1]) && Math::is_nearly_equal(m[2], rhs.m[2])
				&& Math::is_nearly_equal(m[3], rhs.m[3]) && Math::is_nearly_equal(m[4], rhs.m[4]) && Math::is_nearly_equal(m[5], rhs.m[5])
				&& Math::is_nearly_equal(m[6], rhs.m[6]) && Math::is_nearly_equal(m[7], rhs.m[7]) && Math::is_nearly_equal(m[8], rhs.m[8]);
		}
	};

	template <std::floating_point T>
	struct Matrix2Template
	{
		using ComponentType = T;
		using MatrixType = Matrix2Template<T>;

		ComponentType m[2 * 2];

		constexpr Matrix2Template() noexcept
			: m()
		{
		}

		constexpr Matrix2Template(ComponentType m00, ComponentType m01,
			ComponentType m10, ComponentType m11) noexcept
			: m{ m00, m01, m10, m11 }
		{
		}

		constexpr explicit Matrix2Template(ComponentType scalar) noexcept
			: m()
		{
			m[0] = m[3] = scalar;
		}

		static constexpr MatrixType identity()
		{
			return MatrixType(ComponentType(1));
		}

		constexpr bool is_identity() const
		{
			return Math::is_nearly_equal(m[0], ComponentType(1)) && Math::is_nearly_equal(m[1], ComponentType(0))
				&& Math::is_nearly_equal(m[2], ComponentType(0)) && Math::is_nearly_equal(m[3], ComponentType(1));
		}

		constexpr MatrixType transpose() const
		{
			return Matrix2Template(
				m[0], m[2],
				m[1], m[3]);
		}

		constexpr MatrixType inverse() const
		{
			T det = m[0] * m[3] - m[1] * m[2];

			if (Math::is_nearly_zero(det))
				return identity();

			T inv_det = T(1) / det;

			return Matrix2Template(
				m[3] * inv_det, -m[1] * inv_det,
				-m[2] * inv_det, m[0] * inv_det);
		}

		constexpr MatrixType operator*(const MatrixType& rhs) const
		{
			return Matrix2Template(
				m[0] * rhs.m[0] + m[1] * rhs.m[2],
				m[0] * rhs.m[1] + m[1] * rhs.m[3],
				m[2] * rhs.m[0] + m[3] * rhs.m[2],
				m[2] * rhs.m[1] + m[3] * rhs.m[3]);
		}

		constexpr Vector2Template<ComponentType> operator*(const Vector2Template<ComponentType>& rhs) const
		{
			return Vector2Template<ComponentType>(
				m[0] * rhs.x + m[1] * rhs.y,
				m[2] * rhs.x + m[3] * rhs.y);
		}

		constexpr bool operator==(const MatrixType& rhs) const
		{
			return Math::is_nearly_equal(m[0], rhs.m[0]) && Math::is_nearly_equal(m[1], rhs.m[1])
				&& Math::is_nearly_equal(m[2], rhs.m[2]) && Math::is_nearly_equal(m[3], rhs.m[3]);
		}
	};

	using Matrix4f = MatrixTemplate<f32>;
	static_assert(sizeof(Matrix4f) == 64);

	using Matrix4d = MatrixTemplate<f64>;
	static_assert(sizeof(Matrix4d) == 128);

#ifndef AW_DEFAULT_FLOAT_TYPE
	using Matrix4 = Matrix4f;
#else
	using Matrix4 = MatrixTemplate<AW_DEFAULT_FLOAT_TYPE>;
#endif

	using Matrix3f = Matrix3Template<f32>;
	static_assert(sizeof(Matrix3f) == 36);

	using Matrix3d = Matrix3Template<f64>;
	static_assert(sizeof(Matrix3d) == 72);

#ifndef AW_DEFAULT_FLOAT_TYPE
	using Matrix3 = Matrix3f;
#else
	using Matrix3 = Matrix3Template<AW_DEFAULT_FLOAT_TYPE>;
#endif

	using Matrix2f = Matrix2Template<f32>;
	static_assert(sizeof(Matrix2f) == 16);

	using Matrix2d = Matrix2Template<f64>;
	static_assert(sizeof(Matrix2d) == 32);

#ifndef AW_DEFAULT_FLOAT_TYPE
	using Matrix2 = Matrix2f;
#else
	using Matrix2 = Matrix2Template<AW_DEFAULT_FLOAT_TYPE>;
#endif
} // namespace aw::core
