#include <gtest/gtest.h>

#include "aw/core/all.h"

using namespace aw::core;

TEST(aw_core_math_tests, test_vector3)
{
	// Test vector +=
	Vector3 vec1 = {0.0f, 0.0f, 0.0f};
	vec1 += Vector3(1.0f, 2.0f, 3.0f);
	EXPECT_EQ(vec1, Vector3(1.0f, 2.0f, 3.0f));

	// Test vector math operations
	Vector3 vec2 = vec1 + Vector3(2.0f, 3.0f, 4.0f);
	EXPECT_EQ(vec2, Vector3(3.0f, 5.0f, 7.0f));

	Vector3 vec3 = vec2 - Vector3(1.0f, 2.0f, 3.0f);
	EXPECT_EQ(vec3, Vector3(2.0f, 3.0f, 4.0f));

	const Vector3 normalized = vec1.normalized();
	EXPECT_EQ(normalized, Vector3(0.26726124f, 0.53452248f, 0.80178372f));
	EXPECT_TRUE(Math::is_nearly_equal(normalized.length(), 1.0f));
}