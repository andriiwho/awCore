#pragma once

#include "aw/core/primitive/numbers.h"

#include <array>
#include <shared_mutex>

namespace aw::core {
#define AW_PASTE(a, b) a##b
#define AW_PASTE2(a, b) AW_PASTE(a, b)
#define AW_PASTE3(a, b, c) AW_PASTE2(AW_PASTE2(a, b), c);

    namespace detail {
        /** 64-bit log table */
        inline constexpr u64 log2_table_64[64]{
            63, 0, 58, 1, 59, 47, 53, 2,
            60, 39, 48, 27, 54, 33, 42, 3,
            61, 51, 37, 40, 49, 18, 28, 20,
            55, 30, 34, 11, 43, 14, 22, 4,
            62, 57, 46, 52, 38, 26, 32, 41,
            50, 36, 17, 19, 29, 10, 13, 21,
            56, 45, 25, 31, 35, 16, 9, 12,
            44, 24, 15, 8, 23, 7, 6, 5
        };

        inline constexpr u32 log2_table_32[32]{
            0, 9, 1, 10, 13, 21, 2, 29,
            11, 14, 16, 18, 22, 25, 3, 30,
            8, 12, 20, 28, 15, 17, 24, 7,
            19, 27, 23, 6, 26, 5, 4, 31
        };
    } // namespace detail

    constexpr u64 fast_log2(const u64 value) {
        u64 mod_value = value;
        mod_value |= mod_value >> 1;
        mod_value |= mod_value >> 2;
        mod_value |= mod_value >> 4;
        mod_value |= mod_value >> 8;
        mod_value |= mod_value >> 16;
        mod_value |= mod_value >> 32;
        return detail::log2_table_64[to_u64((mod_value - (mod_value >> 1)) * 0x07edd5e59a4e28c2) >> 58];
    }

    constexpr u32 fast_log2(const u32 value) {
        u32 mod_value = value;
        mod_value |= mod_value >> 1;
        mod_value |= mod_value >> 2;
        mod_value |= mod_value >> 4;
        mod_value |= mod_value >> 8;
        mod_value |= mod_value >> 16;
        return detail::log2_table_32[to_u32(mod_value * 0x07c4acdd) >> 27];
    }
}

namespace aw::core {
    struct Megabytes {
        constexpr Megabytes() = default;

        constexpr Megabytes(const u64 in_value)
            : value(in_value) {
        }

        u64 value{};
    };

    struct Kilobytes {
        constexpr Kilobytes() = default;

        constexpr Kilobytes(const u64 in_value)
            : value(in_value) {
        }

        constexpr Kilobytes(Megabytes mb)
            : value(mb.value * 1024) {
        }

        u64 value{};
    };

    struct Bytes {
        constexpr Bytes() = default;

        constexpr Bytes(const u64 in_value)
            : value(in_value) {
        }

        constexpr Bytes(const Kilobytes kb)
            : value(kb.value * 1024) {
        }

        constexpr Bytes(const Megabytes mb)
            : Bytes(Kilobytes(mb)) {
        }

        inline constexpr operator u64() const noexcept { return value; }

        u64 value{};
    };

    constexpr u64 size_align_to_pow2(Bytes value) {
        u64 count = 0;
        if (value && !(value & (value - 1)))
            return value;

        while (value != 0) {
            value.value >>= 1;
            count++;
        }

        return 1ull << count;
    }

    struct AllocationHeader {
        class MemoryPage *page{};
        u32 index{};
        u32 alloc_size{};
    };

    struct PageFreeList {
        static constexpr u64 DEFAULT_PREALLOCATED_CAPACITY = 1024;
        static constexpr double CAPACITY_INCREMENT = 1.5;

        PageFreeList(u32 preallocated_capacity = DEFAULT_PREALLOCATED_CAPACITY);

        ~PageFreeList();

        void push(u32 index);

        u32 peek() const;

        u32 pop();

        std::atomic<u32 *> data{};
        std::atomic<u32> tail{};
        std::atomic<u32> capacity{};
        std::atomic<u32> size{};
        std::mutex m_Mutex{};
    };

    static constexpr Bytes DEFAULT_PAGE_SIZE = Megabytes(4);

    class MemoryPage {
    public:
        MemoryPage(const Bytes aligned_alloc_size)
            : m_AlignedAllocSize(aligned_alloc_size)
              , m_MaxAllocations(DEFAULT_PAGE_SIZE / aligned_alloc_size) {
        }

        void set_root() { m_Root = true; }

        void *allocate_block(u64 requested_size);

        static void free_block(void *block);

        void shutdown();

    private:
        void alloc_next();

        void dealloc_next();

        void *initialize_header(u64 index, u64 requested_size);

        void *get_block_at_index(u64 index);

    private:
        std::array<u8, DEFAULT_PAGE_SIZE> m_Memory{};
        MemoryPage *m_Prev{};
        MemoryPage *m_Next{};
        PageFreeList m_FreeList{};

        // Aligned size of the allocation. This is the allocation step.
        Bytes m_AlignedAllocSize{};

        const u64 m_MaxAllocations{};
        std::atomic<u64> m_NumAliveAllocations{0};

        u64 m_Tail{};
        bool m_Root{};
        std::mutex m_Mutex{};
    };

    class PagedMemoryPool {
    public:
        static PagedMemoryPool &get();

        ~PagedMemoryPool();

        void *allocate_memory(Bytes size);

        static void free_memory(void *memory);

        static u64 get_allocation_size(const void *const data);

    private:
        MemoryPage *get_page_tree_for_allocation(Bytes allocation_size);

        std::array<MemoryPage *, 64> m_PageTrees{};
        std::shared_mutex m_PageTreesMutex{};
    };
} // namespace AW::core::memory

#ifdef KP_NEVER_CLEAR_EMPTY_MEMORY_PAGES
inline constexpr bool CLEAR_EMPTY_PAGES = false;
#else
inline constexpr bool CLEAR_EMPTY_PAGES = true;
#endif

inline void *operator new(const std::size_t size, aw::core::PagedMemoryPool &pool) {
    return pool.allocate_memory(aw::core::Bytes(size));
}

inline void operator delete(void *ptr, aw::core::PagedMemoryPool &pool) {
    pool.free_memory(ptr);
}

#define aw_new new(aw::core::PagedMemoryPool::get())
#define aw_delete(val) std::destroy_at(val); aw::core::PagedMemoryPool::get().free_memory(val)
