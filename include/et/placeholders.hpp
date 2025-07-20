#pragma once

#include <tuple>
#include <functional>

#include "expr.hpp"

namespace et {

namespace detail {

// template <int i, typename Arg1, typename... Args>
// decltype(auto) constexpr get(Arg1&& arg1, Args&&... args) {
//     if constexpr (i == 0) {
//         return std::forward<Arg1>(arg1);
//     }
//     else {
//         static_assert(i > 0 && i <= sizeof...(Args));
//         return get<i-1>(std::forward<Args>(args)...);
//     }
// }

template <typename T>
concept TupleLike = requires () {
    std::tuple_size<T>::value;
};

template <typename T>
concept Placeholder = (std::is_placeholder_v<T> > 0);

template <typename T>
concept PlaceholderOrRef = (std::is_placeholder_v<std::remove_cvref_t<T>> > 0);

} // namespace detail

////////////////////////////////////////////////////////////////////////////////

template <typename E, detail::TupleLike Tuple>
constexpr decltype(auto) replace_placeholders(E&& e, const Tuple& arguments) {
    return transform_matching(e, [&] (detail::PlaceholderOrRef auto&& terminal) {
        return std::get<std::is_placeholder_v<std::remove_cvref_t<decltype(terminal)>> - 1>(arguments);
    });
}

template <typename E, typename... Args>
constexpr decltype(auto) invoke(E&& e, Args&&... args) {
    return evaluate(replace_placeholders(e, std::forward_as_tuple(std::forward<Args>(args)...)));
}

template <typename E, typename Tuple>
constexpr decltype(auto) apply(E&& e, Tuple&& t) {
    return evaluate(replace_placeholders(e, std::forward<Tuple>(t)));
}

}
