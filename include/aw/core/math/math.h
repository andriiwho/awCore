#pragma once

#include <algorithm>

#include "aw/core/primitive/numbers.h"

#include <cmath>
#include <concepts>

namespace aw::core {
    struct Math {
        static constexpr f32 PI = 3.14159265358979323846f;
        static constexpr f64 PI64 = 3.14159265358979323846;

        static constexpr f32 DEG_TO_RAD = PI / 180.0f;
        static constexpr f32 RAD_TO_DEG = 180.0f / PI;

        static constexpr f64 DEG_TO_RAD64 = PI64 / 180.0;
        static constexpr f64 RAD_TO_DEG64 = 180.0 / PI64;

        static constexpr f32 EPSILON = 0.00001f;
        static constexpr f64 EPSILON64 = 0.00001;

        template<std::floating_point T>
        static T sin(T value) {
            return std::sin(value);
        }

        template<std::floating_point T>
        static T cos(T value) {
            return std::cos(value);
        }

        template<std::floating_point T>
        static T tan(T value) {
            return std::tan(value);
        }

        template<std::floating_point T>
        static T asin(T value) {
            return std::asin(value);
        }

        template<std::floating_point T>
        static T acos(T value) {
            return std::acos(value);
        }

        template<std::floating_point T>
        static T atan(T value) {
            return std::atan(value);
        }

        template<std::floating_point T>
        static T atan2(T y, T x) {
            return std::atan2(y, x);
        }

        template<std::floating_point T>
        static T sqrt(T value) {
            return std::sqrt(value);
        }

        template<std::floating_point T>
        static T pow(T base, T exponent) {
            return std::pow(base, exponent);
        }

        template<std::floating_point T>
        static T abs(T value) {
            return std::abs(value);
        }

        template<std::floating_point T>
        static T floor(T value) {
            return std::floor(value);
        }

        template<std::floating_point T>
        static T ceil(T value) {
            return std::ceil(value);
        }

        template<std::floating_point T>
        static T round(T value) {
            return std::round(value);
        }

        template<std::floating_point T>
        static T fmod(T x, T y) {
            return std::fmod(x, y);
        }

        template<Arithmetic T>
        static T min(T a, T b) {
            return std::min(a, b);
        }

        template<Arithmetic T>
        static T max(T a, T b) {
            return std::max(a, b);
        }

        template<Arithmetic T>
        static T clamp(T value, T min, T max) {
            return std::clamp(value, min, max);
        }

        template<Arithmetic T>
        static T lerp(T a, T b, T t) {
            return a + (b - a) * t;
        }

        template<Arithmetic T>
        static T lerp_clamped(T a, T b, T t) {
            return clamp(lerp(a, b, t), a, b);
        }

        template<Arithmetic T>
        static T lerp_inverse(T a, T b, T t) {
            return clamp((t - a) / (b - a), 0.0f, 1.0f);
        }

        template<Arithmetic T>
        static T lerp_inverse_clamped(T a, T b, T t) {
            return clamp(lerp_inverse(a, b, t), 0.0f, 1.0f);
        }

        template<std::floating_point T>
        static T sign(T value) {
            return std::signbit(value) ? T(-1) : T(1);
        }

        template<std::floating_point T>
        static T sign_inverse(T value) {
            return std::signbit(value) ? T(1) : T(-1);
        }

        template<std::floating_point T>
        static bool is_nearly_equal(T a, T b) {
            return abs(a - b) < EPSILON;
        }

        template<std::floating_point T>
        static bool is_nearly_equal(T a, T b, T epsilon) {
            return abs(a - b) < epsilon;
        }

        template<std::floating_point T>
        static bool is_nearly_zero(T value) {
            return abs(value) < EPSILON;
        }
    };
}