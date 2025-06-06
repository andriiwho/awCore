#include <gtest/gtest.h>

#include "aw/core/all.h"

TEST(CoreTests, TestBasicTest)
{
	EXPECT_EQ(69, 69);
}

TEST(CoreTests, TestMemAllocFree)
{
	void* allocation = aw::core::allocate_memory(100);
	defer[allocation]
	{
		aw::core::free_memory(allocation);
	};
	EXPECT_EQ(aw::core::get_allocation_size(allocation), 100);
}

TEST(CoreTests, TestDefaultAllocator)
{
	aw::core::DefaultAllocator<int> alloc;
	int* a = alloc.allocate(10);
	defer[a, &alloc]
	{
		alloc.deallocate(a, 10);
	};
	EXPECT_EQ(aw::core::get_allocation_size(a), 10 * sizeof(int));
}

TEST(CoreTests, TestAWNewDelete)
{
	const auto a = aw_new int(35);
	defer[a]
	{
		aw_delete(a);
	};
	EXPECT_EQ(*a, 35);
}

TEST(CoreTests, TestRefCounted)
{
	class S : public aw::core::IntrusiveRefCounted
	{

	};

	try
	{
		aw::core::RefPtr<S> s = aw::core::new_ref<S>();
	}
	catch (const std::exception& e)
	{
		FAIL() << e.what() << std::endl;
	}
}
