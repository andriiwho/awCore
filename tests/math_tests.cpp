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
