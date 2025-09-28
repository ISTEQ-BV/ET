// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov

#pragma once

#include "type_name.hpp"

#include <functional>
#include <type_traits>
#include <utility>

namespace et {

////////////////////////////////////////////////////////////////////////////////

namespace op {

using plus = std::plus<>;
using minus = std::minus<>;
using multiplies = std::multiplies<>;
using divides = std::divides<>;
using modulus = std::modulus<>;
using negate = std::negate<>;
using logical_and = std::logical_and<>;
using logical_or = std::logical_or<>;
using logical_not = std::logical_not<>;
using bit_and = std::bit_and<>;
using bit_or = std::bit_or<>;
using bit_xor = std::bit_xor<>;
using bit_not = std::bit_not<>;

using equal_to = std::equal_to<>;
using not_equal_to = std::not_equal_to<>;
using greater = std::greater<>;
using less = std::less<>;
using greater_equal = std::greater_equal<>;
using less_equal = std::less_equal<>;

using identity = std::identity;

struct select {
    template <typename Cond, typename Arg1, typename Arg2>
    constexpr decltype(auto) operator()(Cond&& cond, Arg1&& arg1, Arg2&& arg2) const {
        return cond ? std::forward<Arg1>(arg1) : std::forward<Arg2>(arg2);
    }
};

} // namespace op;

////////////////////////////////////////////////////////////////////////////////

template <typename Op, typename... Args>
struct expr;

////////////////////////////////////////////////////////////////////////////////

namespace detail {

template <typename T>
inline constexpr bool is_expr = false;

template <typename Op, typename... Args>
inline constexpr bool is_expr<expr<Op, Args...>> = true;

template <typename Op, typename T>
inline constexpr bool is_expr_kind = false;

template <typename Op, typename... Args>
inline constexpr bool is_expr_kind<Op, expr<Op, Args...>> = true;

} // namespace detail

template <typename T>
concept Expr = detail::is_expr<std::remove_cvref_t<T>>;

////////////////////////////////////////////////////////////////////////////////

// nullary expression represents a terminal expression
template <typename Arg>
struct expr<Arg> {
    [[no_unique_address]] Arg arg;
};

template <typename Op, typename Arg1>
struct expr<Op, Arg1> {
    [[no_unique_address]] Op op;
    [[no_unique_address]] Arg1 arg1;
};

template <typename Op, typename Arg1, typename Arg2>
struct expr<Op, Arg1, Arg2> {
    [[no_unique_address]] Op op;
    [[no_unique_address]] Arg1 arg1;
    [[no_unique_address]] Arg2 arg2;
};

template <typename Op, typename Arg1, typename Arg2, typename Arg3>
struct expr<Op, Arg1, Arg2, Arg3> {
    [[no_unique_address]] Op op;
    [[no_unique_address]] Arg1 arg1;
    [[no_unique_address]] Arg2 arg2;
    [[no_unique_address]] Arg3 arg3;
};

// CTAD
template <typename Op, typename... Args>
expr(Op&& op, Args&&... args) -> expr<Op, Args...>;

////////////////////////////////////////////////////////////////////////////////

namespace detail {

template <typename T>
constexpr auto copy(T&& x) {
    return std::forward<T>(x);
}

} // manespace detail

////////////////////////////////////////////////////////////////////////////////

template <typename T>
constexpr decltype(auto) unwrap(T&& val) {
    if constexpr (detail::is_expr<std::remove_cvref_t<T>>) {
        // expressions are returned by copy
        return detail::copy(std::forward<T>(val));
    }
    else {
        return std::forward<T>(val);
    }
}

// terminals are unwrapped
// references are passed as is
// values are copied
template <typename Arg>
constexpr decltype(auto) unwrap(const expr<Arg>& e) {
    if constexpr (std::is_reference_v<Arg>) {
        return e.arg;
    }
    else {
        return detail::copy(e.arg);
    }
};
template <typename Arg>
constexpr decltype(auto) unwrap(expr<Arg>&& val) {
    return std::move(val).arg;
};

////////////////////////////////////////////////////////////////////////////////

template<typename Op, typename... Args>
constexpr auto make_expr(Op&& op, Args&&... args)
{
    return expr{unwrap(std::forward<Op>(op)), unwrap(std::forward<Args>(args))...};
}

template<typename T>
constexpr auto as_expr(T&& val)
{
    if constexpr (detail::is_expr<std::remove_cvref_t<T>>) {
        return std::forward<T>(val);
    }
    else {
        return expr{std::forward<T>(val)};
    }
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline constexpr std::string_view symbol_v = type_name<T>;

template <typename T>
inline constexpr bool is_prefix_op_v = false;

#define ET_UNARY_OP(op, fn) \
template<Expr A> \
constexpr auto operator op( A&& a ) \
{ \
    return expr(fn{}, unwrap(std::forward<A>(a))); \
} \
template <> inline constexpr bool is_prefix_op_v<fn> = true; \
template <> inline constexpr std::string_view symbol_v<fn> = #op;

template <typename T>
inline constexpr bool is_infix_op_v = false;

#define ET_BINARY_OP(op, fn) \
template<class A, class B> \
requires Expr<A> || Expr<B> \
constexpr auto operator op( A&& a, B&& b ) \
{ \
    return expr(fn{}, unwrap(std::forward<A>(a)), unwrap(std::forward<B>(b))); \
} \
template <> inline constexpr bool is_infix_op_v<fn> = true; \
template <> inline constexpr std::string_view symbol_v<fn> = #op;

ET_UNARY_OP(-, op::negate);
ET_UNARY_OP(!, op::logical_not);
ET_UNARY_OP(~, op::bit_not);

ET_BINARY_OP(+, op::plus);
ET_BINARY_OP(-, op::minus);
ET_BINARY_OP(*, op::multiplies);
ET_BINARY_OP(/, op::divides);
ET_BINARY_OP(%, op::modulus);
ET_BINARY_OP(&&, op::logical_and);
ET_BINARY_OP(||, op::logical_or);
ET_BINARY_OP(&, op::bit_and);
ET_BINARY_OP(|, op::bit_or);
ET_BINARY_OP(^, op::bit_xor);

ET_BINARY_OP(==, op::equal_to);
ET_BINARY_OP(!=, op::not_equal_to);
ET_BINARY_OP(>, op::greater);
ET_BINARY_OP(<, op::less);
ET_BINARY_OP(>=, op::greater_equal);
ET_BINARY_OP(<=, op::less_equal);

#undef ET_UNARY_OP
#undef ET_BINARY_OP

template<class A, class B, class C>
    requires Expr<A> || Expr<B> || Expr<C>
constexpr auto select(A&& a, B&& b, C&& c)
{
    return expr(op::select{}, unwrap(std::forward<A>(a)), unwrap(std::forward<B>(b)), unwrap(std::forward<C>(c)));
}
template <> inline constexpr std::string_view symbol_v<op::select> = "select";

template <> inline constexpr std::string_view symbol_v<op::identity> = "identity";

////////////////////////////////////////////////////////////////////////////////

template<typename Arg>
    requires (!Expr<Arg>)
constexpr decltype(auto) evaluate(Arg&& arg) {
    return std::forward<Arg>(arg);
}

template<typename Arg>
constexpr decltype(auto) evaluate(const expr<Arg> &e) {
    return e.arg;
}

template<typename Op, typename Arg1>
constexpr decltype(auto) evaluate(const expr<Op, Arg1> &e) {
    return e.op(evaluate(e.arg1));
}

template<typename Op, typename Arg1, typename Arg2>
constexpr decltype(auto) evaluate(const expr<Op, Arg1, Arg2> &e) {
    return e.op(evaluate(e.arg1), evaluate(e.arg2));
}

template<typename Op, typename Arg1, typename Arg2, typename Arg3>
constexpr decltype(auto) evaluate(const expr<Op, Arg1, Arg2, Arg3> &e) {
    return e.op(evaluate(e.arg1), evaluate(e.arg2), evaluate(e.arg3));
}

////////////////////////////////////////////////////////////////////////////////

template<typename E>
using evaluation_result_t = decltype(evaluate(std::declval<const E&>()));

////////////////////////////////////////////////////////////////////////////////

namespace detail {

template <typename T>
concept Terminal = !Expr<T>;

} // namespace detail

////////////////////////////////////////////////////////////////////////////////

namespace detail {

template<typename T>
constexpr inline int arity = -1;

template<typename Op, typename... Args>
constexpr inline int arity<expr<Op, Args...>> = sizeof...(Args);

} // namespace detail

template <typename E, typename Tr>
decltype(auto) constexpr transform_matching(const E& e, Tr&& tr) {
    if constexpr (std::is_invocable_v<Tr&, const E&>) {
        return tr(e);
    }
    else {
        using E1 = std::remove_cvref_t<E>;
        if constexpr (Expr<E1>) {
            if constexpr (detail::arity<E1> == 0) {
                return detail::copy(e);
            }
            else if constexpr (detail::arity<E1> == 1) {
                return expr(detail::copy(e.op),
                            transform_matching(e.arg1, tr)
                            );
            }
            else if constexpr (detail::arity<E1> == 2) {
                return expr(detail::copy(e.op),
                            transform_matching(e.arg1, tr),
                            transform_matching(e.arg2, tr)
                            );
            }
            else if constexpr (detail::arity<E1> == 3) {
                return expr(detail::copy(e.op),
                            transform_matching(e.arg1, tr),
                            transform_matching(e.arg2, tr),
                            transform_matching(e.arg3, tr)
                            );
            }
            else {
                static_assert(false, "Unknown arity");
            }
        }
        else {
            return e;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

template <typename E, typename Tr>
decltype(auto) constexpr transform_terminals(E&& e, Tr&& tr) {
    return transform_matching(std::forward<E>(e), [&tr] (detail::Terminal auto&& t) {
        return std::forward<Tr>(tr)(std::forward<decltype(t)>(t));
    });
}

////////////////////////////////////////////////////////////////////////////////

} // namespace ET
