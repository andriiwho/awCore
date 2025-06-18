#include <gtest/gtest.h>

#include "aw/core/all.h"
#include "aw/core/primitive/enum_flags.h"

using namespace aw::core;

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


enum class Flags : aw::core::u32
{
	a = 1 << 0,
	b = 1 << 1,
	c = 1 << 2,
	d = 1 << 3,
	e = 1 << 4,
	f = 1 << 5,
	g = 1 << 6,
	h = 1 << 7,
	i = 1 << 8,
	j = 1 << 9,
};
aw_enum_flags(Flags);

TEST(CoreTests, TestEnumFlags)
{
	EnumMask<Flags> mask = Flags::a | Flags::b;
	EXPECT_TRUE(mask.contains(Flags::a));
	EXPECT_TRUE(mask.contains(Flags::b));

	mask.remove(Flags::a);
	EXPECT_FALSE(mask.contains(Flags::a));

	mask |= Flags::d;
	EXPECT_EQ(mask, Flags::b | Flags::d);

	mask |= Flags::e;
	EXPECT_TRUE(mask.contains(Flags::e));
}

TEST(CoreTests, TestDelegates)
{
	static DelegateVoid delegate;
	static bool called = false;

	class MockClass
	{
	public:
		MockClass()
		{
			delegate.bind(this, &MockClass::called_from_delegate);
		}

		void called_from_delegate()
		{
			called = !called;
		}
	};
	MockClass mock;
	delegate.execute_safe();

	EXPECT_TRUE(called);

	using MulticastDelegateVoid = MulticastDelegate<>;
	MulticastDelegateVoid multicast_delegate;
	multicast_delegate.add(&mock, &MockClass::called_from_delegate);
	multicast_delegate.execute_safe();

	EXPECT_FALSE(called);
}
