#ifndef TYPE_NAME_HPP
#define TYPE_NAME_HPP

// inspired by https://stackoverflow.com/a/64490578/4451432
// answer by https://stackoverflow.com/users/1593077/einpoklum
// CC-BY-SA 4.0 https://stackoverflow.com/help/licensing

// This next line assumes C++17; otherwise, replace it with
// your own string view implementation
#include <string_view>
#if __cplusplus >= 202002L
#  include <source_location>
#endif

namespace et {

namespace detail {

template <typename T>
constexpr std::string_view wrapped_type_name()
{
#if __cplusplus >= 202002L
    return std::source_location::current().function_name();
#else
#  if defined(__clang__) || defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#  elif defined(_MSC_VER)
    return __FUNCSIG__;
#  else
#    error "Unsupported compiler"
#  endif
#endif // __cplusplus >= 202002L
}

template <typename T>
constexpr inline std::string_view wrapped = wrapped_type_name<T>();

constexpr inline std::size_t wrapped_type_prefix_length
    = wrapped<int>.find("int");

constexpr inline std::size_t wrapped_type_excess_length
    = wrapped<int>.length() - 3;

} // namespace detail

template <typename T>
constexpr inline std::string_view type_name =
    detail::wrapped<T>.substr(
        detail::wrapped_type_prefix_length,
        detail::wrapped<T>.length() - detail::wrapped_type_excess_length
    );

} // namespace et

#endif // TYPE_NAME_HPP
