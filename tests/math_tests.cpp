#include <gtest/gtest.h>

#include "aw/core/all.h"

using namespace aw::core;

TEST(FastLog2Test, U64Values)
{
	EXPECT_EQ(Math::fast_log2(1ull), 0);
	EXPECT_EQ(Math::fast_log2(2ull), 1);
	EXPECT_EQ(Math::fast_log2(4ull), 2);
	EXPECT_EQ(Math::fast_log2(8ull), 3);
	EXPECT_EQ(Math::fast_log2(16ull), 4);
	EXPECT_EQ(Math::fast_log2(32ull), 5);
	EXPECT_EQ(Math::fast_log2(64ull), 6);
	EXPECT_EQ(Math::fast_log2(128ull), 7);
	EXPECT_EQ(Math::fast_log2(256ull), 8);
}

TEST(FastLog2Test, U32Values)
{
	EXPECT_EQ(Math::fast_log2(1u), 0);
	EXPECT_EQ(Math::fast_log2(2u), 1);
	EXPECT_EQ(Math::fast_log2(4u), 2);
	EXPECT_EQ(Math::fast_log2(8u), 3);
	EXPECT_EQ(Math::fast_log2(16u), 4);
	EXPECT_EQ(Math::fast_log2(32u), 5);
	EXPECT_EQ(Math::fast_log2(64u), 6);
	EXPECT_EQ(Math::fast_log2(128u), 7);
	EXPECT_EQ(Math::fast_log2(256u), 8);
}

TEST(SizeAlignToPow2Test, BasicValues)
{
	EXPECT_EQ(Math::align_to_pow2(Bytes(0)), 1);
	EXPECT_EQ(Math::align_to_pow2(Bytes(1)), 1);
	EXPECT_EQ(Math::align_to_pow2(Bytes(2)), 2);
	EXPECT_EQ(Math::align_to_pow2(Bytes(3)), 4);
	EXPECT_EQ(Math::align_to_pow2(Bytes(4)), 4);
	EXPECT_EQ(Math::align_to_pow2(Bytes(5)), 8);
	EXPECT_EQ(Math::align_to_pow2(Bytes(7)), 8);
	EXPECT_EQ(Math::align_to_pow2(Bytes(8)), 8);
	EXPECT_EQ(Math::align_to_pow2(Bytes(9)), 16);
}

TEST(SizeClassesTest, BytesConversion)
{
	EXPECT_EQ(Bytes(1024), 1024);
	EXPECT_EQ(Bytes(Kilobytes(1)), 1024);
	EXPECT_EQ(Bytes(Megabytes(1)), 1024 * 1024);
}

TEST(SizeClassesTest, KilobytesConversion)
{
	EXPECT_EQ(Kilobytes(1).value, 1);
	EXPECT_EQ(Kilobytes(Megabytes(1)).value, 1024);
}

TEST(SizeClassesTest, MegabytesConversion)
{
	EXPECT_EQ(Megabytes(1).value, 1);
	EXPECT_EQ(Megabytes(2).value, 2);
}

TEST(Vector2Test, BasicOperations)
{
	constexpr Vector2f v1(1.0f, 2.0f);
	constexpr Vector2f v2(2.0f, 3.0f);

	EXPECT_EQ(v1 + v2, Vector2f(3.0f, 5.0f));
	EXPECT_EQ(v1 - v2, Vector2f(-1.0f, -1.0f));
	EXPECT_EQ(v1 * 2.0f, Vector2f(2.0f, 4.0f));
	EXPECT_FLOAT_EQ(v1.length(), std::sqrt(5.0f));
}

TEST(Vector3Test, BasicOperations)
{
	constexpr Vector3f v1(1.0f, 2.0f, 3.0f);
	constexpr Vector3f v2(2.0f, 3.0f, 4.0f);

	EXPECT_EQ(v1 + v2, Vector3f(3.0f, 5.0f, 7.0f));
	EXPECT_EQ(v1 - v2, Vector3f(-1.0f, -1.0f, -1.0f));
	EXPECT_EQ(v1 * 2.0f, Vector3f(2.0f, 4.0f, 6.0f));
	EXPECT_FLOAT_EQ(v1.length(), std::sqrt(14.0f));
}

TEST(Vector4Test, BasicOperations)
{
	constexpr Vector4f v1(1.0f, 2.0f, 3.0f, 4.0f);
	constexpr Vector4f v2(2.0f, 3.0f, 4.0f, 5.0f);

	EXPECT_EQ(v1 + v2, Vector4f(3.0f, 5.0f, 7.0f, 9.0f));
	EXPECT_EQ(v1 - v2, Vector4f(-1.0f, -1.0f, -1.0f, -1.0f));
	EXPECT_EQ(v1 * 2.0f, Vector4f(2.0f, 4.0f, 6.0f, 8.0f));
	EXPECT_FLOAT_EQ(v1.length(), std::sqrt(30.0f));
}

TEST(MathTest, BasicOperations)
{
	EXPECT_FLOAT_EQ(Math::sin(Math::PI / 2.0f), 1.0f);
	EXPECT_FLOAT_EQ(Math::cos(Math::PI), -1.0f);
	EXPECT_FLOAT_EQ(Math::tan(Math::PI / 4.0f), 1.0f);
	EXPECT_FLOAT_EQ(Math::lerp(0.0f, 1.0f, 0.5f), 0.5f);
}

TEST(MatrixTest, Matrix4Transpose)
{
	constexpr Matrix4f m(
		1.0f, 2.0f, 3.0f, 4.0f,
		5.0f, 6.0f, 7.0f, 8.0f,
		9.0f, 10.0f, 11.0f, 12.0f,
		13.0f, 14.0f, 15.0f, 16.0f);

	constexpr Matrix4f m_t = m.transpose();
	constexpr Matrix4f m_t_expected(
		1.0f, 5.0f, 9.0f, 13.0f,
		2.0f, 6.0f, 10.0f, 14.0f,
		3.0f, 7.0f, 11.0f, 15.0f,
		4.0f, 8.0f, 12.0f, 16.0f);
	EXPECT_EQ(m_t, m_t_expected);
}

TEST(MatrixTest, Matrix4Inverse)
{
	constexpr Matrix4f m(
		1, 2, 3, 0,
		0, 1, 4, 0,
		5, 6, 0, 0,
		0, 0, 0, 1);

	constexpr Matrix4f m_inv = m.inverse();
	EXPECT_TRUE((m * m_inv).is_identity());

	constexpr Vector4f v(1, 2, 3, 1);
	constexpr Vector4f v_transformed = m * v;
	EXPECT_EQ(v_transformed, Vector4f(14, 14, 17, 1));
}

TEST(MatrixTest, Matrix3Transpose)
{
	constexpr Matrix3f m(
		1.0f, 2.0f, 3.0f,
		4.0f, 5.0f, 6.0f,
		7.0f, 8.0f, 9.0f);

	constexpr Matrix3f m_t = m.transpose();
	constexpr Matrix3f m_t_expected(
		1.0f, 4.0f, 7.0f,
		2.0f, 5.0f, 8.0f,
		3.0f, 6.0f, 9.0f);
	EXPECT_EQ(m_t, m_t_expected);
}

TEST(MatrixTest, Matrix3Inverse)
{
	constexpr Matrix3f m(
		1, 2, 3,
		0, 1, 4,
		5, 6, 0);

	constexpr Matrix3f m_inv = m.inverse();
	EXPECT_TRUE((m * m_inv).is_identity());

	constexpr Vector3f v(1, 2, 3);
	constexpr Vector3f v_transformed = m * v;
	EXPECT_EQ(v_transformed, Vector3f(14, 14, 17));
}

TEST(MatrixTest, Matrix2Transpose)
{
	constexpr Matrix2f m(
		1.0f, 2.0f,
		3.0f, 4.0f);

	constexpr Matrix2f m_t = m.transpose();
	constexpr Matrix2f m_t_expected(
		1.0f, 3.0f,
		2.0f, 4.0f);
	EXPECT_EQ(m_t, m_t_expected);
}

TEST(MatrixTest, Matrix2Inverse)
{
	constexpr Matrix2f m(
		4, 7,
		2, 6);

	constexpr Matrix2f m_inv = m.inverse();
	EXPECT_TRUE((m * m_inv).is_identity());

	constexpr Vector2f v(1, 2);
	constexpr Vector2f v_transformed = m * v;
	EXPECT_EQ(v_transformed, Vector2f(18, 14));
}
