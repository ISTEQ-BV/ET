// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov

#pragma once

#include "type_name.hpp"

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace et {

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

template <typename Op>
struct expr<Op> {
    [[no_unique_address]] Op op;

    static constexpr int arity = 0;
};

template <typename Op, typename Arg1>
struct expr<Op, Arg1> {
    [[no_unique_address]] Op op;
    [[no_unique_address]] Arg1 arg1;

    static constexpr int arity = 1;
};

template <typename Op, typename Arg1, typename Arg2>
struct expr<Op, Arg1, Arg2> {
    [[no_unique_address]] Op op;
    [[no_unique_address]] Arg1 arg1;
    [[no_unique_address]] Arg2 arg2;

    static constexpr int arity = 2;
};

template <typename Op, typename Arg1, typename Arg2, typename Arg3>
struct expr<Op, Arg1, Arg2, Arg3> {
    [[no_unique_address]] Op op;
    [[no_unique_address]] Arg1 arg1;
    [[no_unique_address]] Arg2 arg2;
    [[no_unique_address]] Arg3 arg3;

    static constexpr int arity = 3;
};

// CTAD
template <typename Op, typename... Args>
expr(Op&& op, Args&&... args) -> expr<Op, Args...>;

////////////////////////////////////////////////////////////////////////////////

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

template <typename T>
constexpr decltype(auto) unwrap(T&& val) {
    if constexpr (detail::is_expr<std::remove_cvref_t<T>>) {
        if constexpr (detail::is_expr_kind<op::identity, std::remove_cvref_t<T>>) {
            return std::forward<decltype(std::remove_cvref_t<T>::arg1)>(val.arg1);
        }
        else {
            // expressions are returned by copy
            return static_cast<std::remove_cvref_t<T>>(val);
        }
    }
    else if constexpr (std::is_reference_v<T> || !std::is_const_v<T>) {
        return std::forward<T>(val);
    }
    else {
        // for const rvalue ref (const T&&) we make a copy
        return static_cast<std::remove_const_t<T>>(val);
    }
}

template<typename Op, typename... Args>
constexpr auto make_expr(Op&& op, Args&&... args)
{
    return expr{unwrap(std::forward<Op>(op)), unwrap(std::forward<Args>(args))...};
}

template<typename T>
constexpr auto make_terminal(T&& val)
{
    return expr{op::identity(), std::forward<T>(val)};
}

template<typename T>
constexpr auto as_expr(T&& val)
{
    if constexpr(detail::is_expr<std::remove_cvref_t<T>>) {
        return val;
    }
    else {
        return expr{op::identity(), std::forward<T>(val)};
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
    return make_expr(fn(), std::forward<A>(a)); \
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
    return make_expr(fn(), std::forward<A>(a), std::forward<B>(b)); \
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
    return make_expr(op::select{}, std::forward<A>(a), std::forward<B>(b), std::forward<C>(c));
}
template <> inline constexpr std::string_view symbol_v<op::select> = "select";

template <> inline constexpr std::string_view symbol_v<op::identity> = "identity";

////////////////////////////////////////////////////////////////////////////////

template<typename Arg>
    requires (!Expr<Arg>)
constexpr decltype(auto) evaluate(Arg&& arg) {
    return std::forward<Arg>(arg);
}

template<typename Op>
constexpr decltype(auto) evaluate(const expr<Op> &e) {
    return e.op();
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
auto copy(T&& x) {
    return x;
}

template <typename T>
concept Terminal = !Expr<T>;

} // namespace detail

////////////////////////////////////////////////////////////////////////////////

/// Value is passed as `const T&&`, reference is passed as `const T&`
//#define PASS(x) static_cast<std::conditional_t<std::is_reference_v<decltype(x)>, decltype(x), const decltype(x)&&>>(x)

#define PASS(x) x

template <typename E, typename Tr>
decltype(auto) constexpr transform_matching(E&& e, Tr&& tr) {
    if constexpr (std::is_invocable_v<Tr, E>) {
        return std::forward<Tr>(tr)(std::forward<E>(e));
    }
    else {
        using E1 = std::remove_cvref_t<E>;
        if constexpr (Expr<E1>) {
            if constexpr (E1::arity == 0) {
                return detail::copy(e);
            }
            else if constexpr (E1::arity == 1) {
                auto ret = make_expr(detail::copy(e.op),
                                 transform_matching(PASS(e.arg1), std::forward<Tr>(tr))
                                 );
                return ret;
            }
            else if constexpr (E1::arity == 2) {
                auto ret = make_expr(detail::copy(e.op),
                                 transform_matching(PASS(e.arg1), std::forward<Tr>(tr)),
                                 transform_matching(PASS(e.arg2), std::forward<Tr>(tr))
                                 );
                return ret;
            }
            else if constexpr (E1::arity == 3) {
                auto ret = make_expr(detail::copy(e.op),
                                 transform_matching(PASS(e.arg1), std::forward<Tr>(tr)),
                                 transform_matching(PASS(e.arg2), std::forward<Tr>(tr)),
                                 transform_matching(PASS(e.arg3), std::forward<Tr>(tr))
                                 );
                return ret;
            }
        }
        else {
            return std::forward<E>(e);
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

} // namespace ET
