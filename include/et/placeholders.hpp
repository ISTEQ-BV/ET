// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov

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

////////////////////////////////////////////////////////////////////////////////

template <typename E, typename... Args>
constexpr decltype(auto) invoke(E&& e, Args&&... args) {
    return evaluate(replace_placeholders(e, std::forward_as_tuple(std::forward<Args>(args)...)));
}

template <typename E, typename Tuple>
constexpr decltype(auto) apply(E&& e, Tuple&& t) {
    return evaluate(replace_placeholders(e, std::forward<Tuple>(t)));
}

////////////////////////////////////////////////////////////////////////////////

namespace tr {

struct terminals {

template <typename E>
constexpr decltype(auto) operator()(E&& e) {
    using E1 = std::remove_cvref_t<E>;
    if constexpr (Expr<E1>) {
        if constexpr (et::detail::arity<E1> == 0) {
            return std::tuple<>();
        }
        else if constexpr (et::detail::arity<E1> == 1) {
            return terminals{}(e.arg);
        }
        else if constexpr (et::detail::arity<E1> == 2) {
            return std::tuple_cat(terminals{}(e.arg1), terminals{}(e.arg2));
        }
        else if constexpr (et::detail::arity<E1> == 3) {
            return std::tuple_cat(terminals{}(e.arg1), terminals{}(e.arg2), terminals{}(e.arg3));
        }
        else {
            static_assert(false, "Unknown arity");
        }
    }
    else {
        return std::forward_as_tuple(std::forward<E>(e));
    }
}

};

} // namespace tr

}
