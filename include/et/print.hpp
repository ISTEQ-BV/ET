// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov

#pragma once

#include "expr.hpp"

#include <ostream>

namespace et {

namespace detail {

template <typename T>
inline constexpr int op_priority = 17;

template <> inline constexpr int op_priority<op::negate> = 3;
template <> inline constexpr int op_priority<op::logical_not> = 3;
template <> inline constexpr int op_priority<op::bit_not> = 3;

template <> inline constexpr int op_priority<op::multiplies> = 5;
template <> inline constexpr int op_priority<op::divides> = 5;
template <> inline constexpr int op_priority<op::modulus> = 5;

template <> inline constexpr int op_priority<op::plus> = 6;
template <> inline constexpr int op_priority<op::minus> = 6;

template <> inline constexpr int op_priority<op::greater> = 9;
template <> inline constexpr int op_priority<op::less> = 9;
template <> inline constexpr int op_priority<op::greater_equal> = 9;
template <> inline constexpr int op_priority<op::less_equal> = 9;

template <> inline constexpr int op_priority<op::equal_to> = 10;
template <> inline constexpr int op_priority<op::not_equal_to> = 10;

template <> inline constexpr int op_priority<op::bit_and> = 11;
template <> inline constexpr int op_priority<op::bit_xor> = 12;
template <> inline constexpr int op_priority<op::bit_or> = 13;
template <> inline constexpr int op_priority<op::logical_and> = 14;
template <> inline constexpr int op_priority<op::logical_or> = 15;

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

template <typename T>
concept Streamable = requires (T&& x, std::ostream& s) {
    s << x;
};

} // namespace detail

////////////////////////////////////////////////////////////////////////////////

namespace tr {

struct print {
    std::ostream& stream;
    int priority = 17;

    template<typename Op, typename Arg1, typename Arg2, typename Arg3>
    bool operator()(const expr<Op, Arg1, Arg2, Arg3> &e) const {
        return  (stream << symbol_v<Op> << '(')
               && print{stream, 17}(e.arg1)
               && (stream << ", ")
               && print{stream, 17}(e.arg2)
               && (stream << ", ")
               && print{stream, 17}(e.arg3)
               && (stream << ')');
    }

    template<typename Op, typename Arg1, typename Arg2>
    bool operator()(const expr<Op, Arg1, Arg2>& e) const {
        if constexpr (is_infix_op_v<std::remove_cvref_t<Op>>) {
            constexpr int new_prio = detail::op_priority<std::remove_cvref_t<Op>>;
            if (new_prio >= priority) {
                stream << '(';
            }

            bool ok = print{stream, new_prio}(e.arg1)
                   && (stream << ' ' << symbol_v<Op> << ' ')
                   && print{stream, new_prio}(e.arg2);

            if (ok && new_prio >= priority) {
                return static_cast<bool>(stream << ')');
            }
            return static_cast<bool>(stream);
        }
        else {
            return  (stream << symbol_v<Op> << '(')
                   && print{stream, 17}(e.arg1)
                   && (stream << ", ")
                   && print{stream, 17}(e.arg2)
                   && (stream << ')');
        }
    }

    template<typename Op, typename Arg1>
    bool operator()(const expr<Op, Arg1>& e) const {
        if constexpr (is_prefix_op_v<std::remove_cvref_t<Op>>) {
            return (stream << symbol_v<Op>)
                && print{stream, detail::op_priority<Op>}(e.arg1);
        }
        else {
            return  (stream << symbol_v<Op> << '(')
                   && print{stream, 17}(e.arg1)
                   && (stream << ')');
        }
    }

    template<typename Arg>
    bool operator()(const expr<Arg>& e) const {
        if constexpr (detail::Streamable<Arg>) {
            return static_cast<bool>(stream << e.arg);
        }
        else {
            return static_cast<bool>(stream << "unprintable<" << type_name<Arg> << ">");
        }
    }

    template<typename T>
    bool operator()(T&& value) const {
        if constexpr (detail::Streamable<T>) {
            return static_cast<bool>(stream << value);
        }
        else {
            return static_cast<bool>(stream << "unprintable<" << type_name<T> << ">");
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

namespace detail {

std::ostream& print_expr_header_impl(std::ostream& s, int indent, std::string_view symbol, std::string_view type_name);

template<typename Op>
inline std::ostream& print_expr_header(std::ostream& s, int indent) {
    return print_expr_header_impl(s, indent, et::symbol_v<Op>, type_name<Op>);
}

std::ostream& print_terminal_impl(std::ostream& stream, int indent, std::string_view type_name);

} // namespace detail

////////////////////////////////////////////////////////////////////////////////

template<typename T>
    requires (!Expr<T>)
inline std::ostream& debug(std::ostream& stream, const T& value, int indent = 0) {
    detail::print_terminal_impl(stream, indent, type_name<T>);
    if constexpr (et::detail::Streamable<T>) {
        return stream << " ( " << value << " )\n";
    }
    else {
        return stream << '\n';
    }
}

template<typename Op, typename... Args>
inline std::ostream& debug(std::ostream& stream, const expr<Op, Args...>& e, int indent = 0) {
    detail::print_expr_header<Op>(stream, indent);
    using et::tr::debug;
    if constexpr (sizeof...(Args) >= 1) {
        debug(stream, e.arg1, indent + 4);
    }
    if constexpr (sizeof...(Args) >= 2) {
        debug(stream, e.arg2, indent + 4);
    }
    if constexpr (sizeof...(Args) >= 3) {
        debug(stream, e.arg3, indent + 4);
    }
    return stream;
}

} // namespace tr

////////////////////////////////////////////////////////////////////////////////

template<Expr E>
inline std::ostream& operator<<(std::ostream& s, const E& e) {
    tr::print{s, 17}(e);
    return s;
}

} // namespace et
