#pragma once
#include <algorithm>

namespace aw::core {
    template<typename CharType, typename Traits, typename Allocator>
    StringTemplate<CharType, Traits, Allocator>::StringTemplate(const CharType *str, Allocator allocator)
        : m_Allocator(allocator),
          m_Capacity(Traits::length(str, NullTerminator) + 1),
          m_Data(allocator.allocate(m_Capacity)),
          m_Size(m_Capacity) {
        std::copy(str, str + m_Capacity, m_Data);
    }

    template<typename CharType, typename Traits, typename Allocator>
    StringTemplate<CharType, Traits, Allocator>::StringTemplate(const StringTemplate &other)
        : m_Allocator(other.m_Allocator),
          m_Capacity(other.m_Capacity),
          m_Data(m_Allocator.allocate(m_Capacity)),
          m_Size(m_Capacity)
    {
        std::copy(other.m_Data, other.m_Data + m_Capacity, m_Data);
    }

    template<typename CharType, typename Traits, typename Allocator>
    StringTemplate<CharType, Traits, Allocator>::StringTemplate(StringTemplate &&other) noexcept
        : m_Allocator(std::move(other.m_Allocator)),
          m_Capacity(other.m_Capacity),
          m_Data(other.m_Data),
          m_Size(other.m_Size)
    {
        other.m_Data = nullptr;
        other.m_Capacity = 0;
        other.m_Size = 0;
    }

    template<typename CharType, typename Traits, typename Allocator>
    StringTemplate<CharType, Traits, Allocator> & StringTemplate<CharType, Traits, Allocator>::operator=(
        const StringTemplate &other) {
        m_Allocator.deallocate(m_Data, m_Capacity);
        m_Capacity = other.m_Capacity;
        m_Data = m_Allocator.allocate(m_Capacity);
        m_Size = m_Capacity;
        std::copy(other.m_Data, other.m_Data + m_Capacity, m_Data);
        return *this;
    }

    template<typename CharType, typename Traits, typename Allocator>
    StringTemplate<CharType, Traits, Allocator> & StringTemplate<CharType, Traits, Allocator>::operator=(
        StringTemplate &&other) noexcept {
        m_Allocator.deallocate(m_Data, m_Capacity);
        m_Allocator = std::move(other.m_Allocator);
        m_Capacity = other.m_Capacity;
        m_Data = other.m_Data;
        m_Size = other.m_Size;
        other.m_Data = nullptr;
        other.m_Capacity = 0;
        other.m_Size = 0;
        return *this;
    }

    template<typename CharType, typename Traits, typename Allocator>
    StringTemplate<CharType, Traits, Allocator>::~StringTemplate() {
        m_Allocator.deallocate(m_Data, m_Capacity);
        m_Data = nullptr;
        m_Capacity = 0;
        m_Size = 0;
    }

    using String = StringTemplate<char>;
}
