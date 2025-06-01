#include <gtest/gtest.h>

#include "aw/core/all.h"

TEST(aw_core_tests, test_tests_are_working) {
    EXPECT_EQ(69, 69);
}

TEST(aw_core_tests, test_memalloc_alloc_free) {
    void *allocation = aw::core::allocate_memory(100);
    defer [allocation] { aw::core::free_memory(allocation); };
    EXPECT_EQ(aw::core::get_allocation_size(allocation), 100);
}

TEST(aw_core_tests, test_default_alloc) {
    aw::core::DefaultAllocator<int> alloc;
    int *a = alloc.allocate(10);
    defer [a, &alloc] { alloc.deallocate(a, 10); };
    EXPECT_EQ(aw::core::get_allocation_size(a), 10 * sizeof(int));
}

TEST(aw_core_tests, test_aw_new_delete) {
    const auto a = aw_new int(35);
    defer [a] { aw_delete(a); };
    EXPECT_EQ(*a, 35);
}
