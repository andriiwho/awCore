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

TEST(aw_core_tests, test_strings) {
    using namespace aw::core;

    const String str = "Hello World!";
    EXPECT_EQ(str.get_length(), 12);
    EXPECT_STREQ(*str, "Hello World!");
    EXPECT_EQ(str.get_capacity(), 13);
    EXPECT_EQ(str.get_allocated_size(), 13 * sizeof(char));
    EXPECT_EQ(str.is_empty(), false);
    EXPECT_EQ(str.get_front(), 'H');
    EXPECT_EQ(str.get_back(), '!');

    String str_copied = str;
    EXPECT_EQ(str_copied.get_length(), 12);
    EXPECT_STREQ(*str_copied, "Hello World!");

    const String str_moved = std::move(str_copied);
    EXPECT_EQ(str_moved.get_length(), 12);
    EXPECT_STREQ(*str_moved, "Hello World!");
    EXPECT_EQ(str_copied.get_length(), 0);
    EXPECT_EQ(str_copied.get_capacity(), 0);
    EXPECT_EQ(str_copied.get_allocated_size(), 0);
    EXPECT_EQ(str_copied.is_empty(), true);
}

TEST(aw_core_tests, test_string_view) {
    using namespace aw::core;

    const String str = "Hello World!";
    const StringView str_view = str;
    EXPECT_EQ(str_view.get_length(), 12);
    EXPECT_STREQ(*str_view, "Hello World!");
    EXPECT_EQ(str_view.get_size(), 13 * sizeof(char));
    EXPECT_EQ(str_view.is_empty(), false);
    EXPECT_EQ(str_view.get_front(), 'H');
    EXPECT_EQ(str_view.get_back(), '!');
}