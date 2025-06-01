#pragma once

#include <cstdint>
#include <concepts>

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

#define AW_CORE_DEFINE_PRIMITIVE_CONVERTER(type) \
    inline constexpr type to_##type(Arithmetic auto value) { return static_cast<type>(value); }

namespace aw::core {
    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using isize = i64;
    using usize = u64;

    using f32 = float;
    using f64 = double;

    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(i8);
    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(i16);
    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(i32);
    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(i64);

    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(u8);
    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(u16);
    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(u32);
    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(u64);

    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(isize);
    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(usize);

    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(f32);
    AW_CORE_DEFINE_PRIMITIVE_CONVERTER(f64);
}
