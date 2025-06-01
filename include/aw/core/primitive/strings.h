#pragma once

#include "aw/core/memory/allocators.h"

namespace aw::core {
    template<typename CharType>
    class CharTraits {
    public:
        static usize length(const CharType* str, const CharType null_terminator) {
            usize out = 0;
            while (str[out] != null_terminator) {
                out++;
            }
            return out;
        }
    };

    template<typename CharType, typename Traits = CharTraits<CharType>, typename Allocator = DefaultAllocator<CharType>>
    class StringTemplate {
    public:
        static constexpr CharType NullTerminator = CharType(0);

        StringTemplate() = default;
        StringTemplate(const CharType* str, Allocator allocator = Allocator());
        StringTemplate(const StringTemplate& other);
        StringTemplate(StringTemplate&& other) noexcept;

        StringTemplate& operator=(const StringTemplate& other);
        StringTemplate& operator=(StringTemplate&& other) noexcept;

        ~StringTemplate();

        inline const CharType* operator*() const { return m_Data; }
        inline const CharType* data() const { return m_Data; }
        inline usize get_allocated_size() const { return m_Size; }
        inline usize get_length() const { return m_Size - 1; }
        inline usize get_capacity() const { return m_Capacity; }
        inline bool is_empty() const { return m_Size == 0; }
        inline CharType& operator[](usize index) { return m_Data[index]; }
        inline CharType operator[](usize index) const { return m_Data[index]; }
        inline CharType& at(usize index) { return m_Data[index]; }
        inline CharType at(usize index) const { return m_Data[index]; }
        inline CharType get_front() const { return m_Data[0]; }
        inline CharType get_back() const { return m_Data[get_length() - 1]; }

    private:
        Allocator m_Allocator{};
        usize m_Capacity{};
        CharType* m_Data{};
        usize m_Size{};
    };
}

#include "strings.inl"