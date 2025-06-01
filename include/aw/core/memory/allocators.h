#pragma once

#include "memalloc.h"

#include <stdexcept>
#include <limits>

namespace aw::core {
    template<typename T>
    class DefaultAllocator {
    public:
        using value_type = T;
        using size_type = usize;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;

        using is_always_equal = std::true_type;

        // Constructor
        DefaultAllocator() noexcept = default;

        // Copy constructor that allows conversion between different types
        template<typename U>
        explicit DefaultAllocator(const DefaultAllocator<U> &) noexcept {
        }

        template<typename U>
        struct rebind {
            using other = DefaultAllocator<U>;
        };

        // Allocation
        [[nodiscard]] T *allocate(usize n) {
            if (n > std::numeric_limits<usize>::max() / sizeof(T))
                throw std::bad_array_new_length();

            if (auto p = static_cast<T *>(allocate_memory(n * sizeof(T)))) {
                return p;
            }

            throw std::bad_alloc();
        }

        // Deallocation
        void deallocate(T *p, usize n) noexcept {
            free_memory(p);
        }
    };

    template<typename T1, typename T2>
    bool operator==(const DefaultAllocator<T1> &, const DefaultAllocator<T2> &) noexcept {
        return true;
    }
}