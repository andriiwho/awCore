#include <gtest/gtest.h>

#include "aw/core/all.h"

TEST(aw_core_tests, test_tests_are_working) {
    EXPECT_EQ(69, 69);
}

TEST(aw_core_tests, test_memalloc_alloc_free) {
    void* allocation = aw::core::allocate_memory(100);
    EXPECT_EQ(aw::core::get_allocation_size(allocation), 100);
    aw::core::free_memory(allocation);
}

TEST(aw_core_tests, test_memalloc_linked_list) {
    void* a = aw::core::allocate_memory(100);
    void* b = aw::core::allocate_memory(100);
    void* c = aw::core::allocate_memory(100);

    EXPECT_EQ(aw::core::get_allocation_count(), 3);

    EXPECT_EQ(aw::core::get_next_allocation(a), b);
    EXPECT_EQ(aw::core::get_prev_allocation(b), a);
    EXPECT_EQ(aw::core::get_next_allocation(b), c);
    EXPECT_EQ(aw::core::get_prev_allocation(c), b);
    EXPECT_EQ(aw::core::get_next_allocation(c), nullptr);
    EXPECT_EQ(aw::core::get_prev_allocation(nullptr), nullptr);

    aw::core::free_memory(c);
    EXPECT_EQ(aw::core::get_next_allocation(a), b);
    EXPECT_EQ(aw::core::get_prev_allocation(b), a);
    EXPECT_EQ(aw::core::get_next_allocation(b), nullptr);
}

TEST(aw_core_tests, test_default_alloc) {
    aw::core::DefaultAllocator<int> alloc;
    int* a = alloc.allocate(10);
    EXPECT_EQ(aw::core::get_allocation_size(a), 10 * sizeof(int));
    alloc.deallocate(a, 10);
}