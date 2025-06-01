#pragma once

#include "memalloc.h"
#include <limits>
#include <stdexcept>
#include <array>

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
        static void deallocate(T *p, usize n) noexcept {
            free_memory(p);
        }
    };

    template<typename T1, typename T2>
    bool operator==(const DefaultAllocator<T1> &, const DefaultAllocator<T2> &) noexcept {
        return true;
    }

    template<typename T, usize InlineCapacity = 1>
    class InlineAllocator {
    public:
        using value_type = T;
        using size_type = usize;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;
        using propagate_on_container_copy_assignment = std::false_type;
        using propagate_on_container_swap = std::true_type;

        // Rebind for containers
        template<typename U>
        struct rebind {
            using other = InlineAllocator<U, InlineCapacity>;
        };

    private:
        static constexpr usize storage_size = sizeof(T) * InlineCapacity;
        alignas(T) std::array<std::byte, storage_size> m_Buffer;
        usize m_InlineCount{0};
        T *m_InlinePtr{nullptr};

        // Helper to check if we can store type U in our buffer
        template<typename U>
        static constexpr bool can_store_inline =
                sizeof(U) * InlineCapacity <= storage_size &&
                alignof(U) <= alignof(T);

    public:
        InlineAllocator() noexcept = default;

        template<typename U>
        InlineAllocator(const InlineAllocator<U, InlineCapacity> &) noexcept {
        }

        [[nodiscard]] T *allocate(const usize n) {
            if (n <= InlineCapacity && m_InlineCount == 0) {
                m_InlineCount = n;
                m_InlinePtr = reinterpret_cast<T *>(&m_Buffer[0]);
                return m_InlinePtr;
            }

            if (auto p = static_cast<T *>(allocate_memory(n * sizeof(T)))) {
                return p;
            }
            throw std::bad_alloc();
        }

        void deallocate(T *p, usize n) noexcept {
            if (p == m_InlinePtr) {
                m_InlineCount = 0;
                m_InlinePtr = nullptr;
                return;
            }
            free_memory(p);
        }

        // Selection of useful methods for debugging and optimization
        bool is_inline(const T *p) const noexcept {
            return p == m_InlinePtr;
        }

        static constexpr usize inline_capacity() noexcept {
            return InlineCapacity;
        }

        usize inline_size() const noexcept {
            return m_InlineCount;
        }

        bool has_inline_allocation() const noexcept {
            return m_InlineCount > 0;
        }
    };
    
    template<typename T, usize BufferSize>
    class StaticAllocator {
    public:
        using value_type = T;
        using size_type = usize;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;
        using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_swap = std::true_type;
        using is_always_equal = std::false_type;

        template<typename U>
        struct rebind {
            using other = StaticAllocator<U, BufferSize>;
        };

    private:
        struct Block {
            usize offset;
            usize size;
            bool used;
        };

        alignas(T) std::array<std::byte, BufferSize> m_Buffer;
        std::array<Block, BufferSize / sizeof(T)> m_Blocks;
        usize m_BlockCount{0};

        Block *find_free_block(usize size) {
            for (usize i = 0; i < m_BlockCount; ++i) {
                if (!m_Blocks[i].used && m_Blocks[i].size >= size) {
                    return &m_Blocks[i];
                }
            }
            return nullptr;
        }

    public:
        StaticAllocator() noexcept = default;

        template<typename U>
        StaticAllocator(const StaticAllocator<U, BufferSize> &) noexcept {
        }

        [[nodiscard]] T *allocate(const usize n) {
            const usize size = n * sizeof(T);
            if (size > BufferSize) {
                throw std::bad_alloc();
            }

            if (Block *block = find_free_block(size)) {
                block->used = true;
                return reinterpret_cast<T *>(&m_Buffer[block->offset]);
            }

            if (m_BlockCount >= m_Blocks.size()) {
                throw std::bad_alloc();
            }

            const usize offset = m_BlockCount > 0
                                     ? m_Blocks[m_BlockCount - 1].offset + m_Blocks[m_BlockCount - 1].size
                                     : 0;

            if (offset + size > BufferSize) {
                throw std::bad_alloc();
            }

            Block &newBlock = m_Blocks[m_BlockCount++];
            newBlock.offset = offset;
            newBlock.size = size;
            newBlock.used = true;

            return reinterpret_cast<T *>(&m_Buffer[offset]);
        }

        void deallocate(T *p, usize) noexcept {
            const usize offset = reinterpret_cast<std::byte *>(p) - m_Buffer.data();
            for (usize i = 0; i < m_BlockCount; ++i) {
                if (m_Blocks[i].offset == offset) {
                    m_Blocks[i].used = false;
                    return;
                }
            }
        }

        bool operator==(const StaticAllocator &other) const noexcept {
            return this == &other;
        }
    };
}
