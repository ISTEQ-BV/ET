// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov
#ifndef TYPE_NAME_HPP
#define TYPE_NAME_HPP

#include <array>
#include <string_view>
// #if __cplusplus >= 202002L
// #  include <source_location>
// #endif

namespace et {

namespace detail {

template <typename T>
constexpr std::string_view wrapped_type_name()
{
// #if __cplusplus >= 202002L
//     return std::source_location::current().function_name();
// #else
#  if defined(__clang__) || defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#  elif defined(_MSC_VER)
    return __FUNCSIG__;
#  else
#    error "Unsupported compiler"
#  endif
// #endif // __cplusplus >= 202002L
}

constexpr inline std::size_t wrapped_type_prefix_length
    = wrapped_type_name<int>().find("int");

constexpr inline std::size_t wrapped_type_excess_length
    = wrapped_type_name<int>().length() - 3;

// make an array containing type name, not null-terminated
template<typename T>
constexpr inline auto make_type_name() {
    constexpr auto wrapped = wrapped_type_name<T>();
    constexpr size_t len = wrapped.length() - detail::wrapped_type_excess_length;
    std::array<char, len> ret;
    for (size_t i = 0; i < len; ++i) {
        ret[i] = wrapped[detail::wrapped_type_prefix_length + i];
    }
    return ret;
}

template <typename T>
constexpr inline auto type_name_array = detail::make_type_name<T>();

} // namespace detail

template <typename T>
constexpr inline std::string_view get_type_name() {
    return {detail::type_name_array<T>.data(), detail::type_name_array<T>.size()};
}

template <typename T>
constexpr inline std::string_view get_type_name(T) {
    using T1 = std::remove_cvref_t<T>;
    return {detail::type_name_array<T1>.data(), detail::type_name_array<T1>.size()};
}

} // namespace et

#endif // TYPE_NAME_HPP
