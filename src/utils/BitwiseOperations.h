//
// Created by Nemesis Verstraete on 20/08/2026.
//

#pragma once
#include <type_traits>

namespace utils {
template<typename>
struct enumBitwiseOperations : std::false_type {};

template<typename T>
concept BitwiseEnum = enumBitwiseOperations<T>::value && std::is_enum_v<T>;

template<typename T>
using Underlying = std::underlying_type_t<T>;

template<BitwiseEnum T>
constexpr bool any(T value) noexcept {
    return static_cast<Underlying<T>>(value) != Underlying<T>{};
}

template<BitwiseEnum T>
constexpr T operator| (T left, T right) noexcept {
    return static_cast<T>(static_cast<Underlying<T>>(left) | static_cast<Underlying<T>>(right));
}

template<BitwiseEnum T>
constexpr T& operator|= (T& left, T right) noexcept {
    left = left | right;
    return left;
}

template<BitwiseEnum T>
constexpr T operator& (T left, T right) noexcept {
    return static_cast<T>(static_cast<Underlying<T>>(left) & static_cast<Underlying<T>>(right));
}

}

namespace renderium {
using utils::operator|;
using utils::operator|=;
using utils::operator&;
using utils::any;
}