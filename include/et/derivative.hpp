// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov

#pragma once

#include "expr.hpp"
#include "math.hpp"

#include <type_traits>
#include <iostream>

namespace autodiff {

namespace op {
    template <int i>
    struct var {
        template <typename T>
        constexpr decltype(auto) operator()(T&& x) const {
            return std::forward<T>(x);
        }
    };
}

template<int i, typename T = double>
struct var : et::expr<op::var<i>, T> {
    var(const T& x)
        : et::expr<op::var<i>, T>({}, x)
    {}

private:
    friend std::ostream& operator<<(std::ostream& s, const var& v)
    {
        return s << "var<" << i << ">(" << v.arg1 << ")";
    }
};

} // namespace autodiff

template <int i, typename T>
inline constexpr bool et::detail::is_expr<autodiff::var<i, T>> = true;

namespace autodiff {

struct unknown_type {};

inline unknown_type operator+(const unknown_type&, const unknown_type&);
inline unknown_type operator-(const unknown_type&, const unknown_type&);
inline unknown_type operator*(const unknown_type&, const unknown_type&);
inline unknown_type operator/(const unknown_type&, const unknown_type&);

template<typename T>
struct zero {
    operator T() const {
        return T{0};
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& s, const zero<T>&)
{
    return s << "0";
}

template<typename T>
struct one {
    operator T() const {
        return T{1};
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& s, const one<T>&)
{
    return s << "1";
}

template<typename T1, typename T2>
using derivative_type = decltype (std::declval<T1>() / std::declval<T2>());

struct not_dependent{
private:
    friend std::ostream& operator<<(std::ostream& s, not_dependent) {
        return s << "not_dependent";
    }
};

template <typename T>
constexpr inline bool is_dependent = true;

template <>
constexpr inline bool is_dependent<not_dependent> = false;

// template <typename Fn, typename Arg>
// auto unary_function_derivative(Fn&& fn, Arg&& arg) {
// }

template <typename Arg>
constexpr auto unary_function_derivative(const et::op::sin& /*fn*/, Arg&& arg) {
    return et::make_expr(et::op::cos{}, std::forward<Arg>(arg));
}

template <typename Arg>
constexpr auto unary_function_derivative(const et::op::cos& /*fn*/, Arg&& arg) {
    return et::make_expr(et::op::negate{}, et::make_expr(et::op::sin{}, std::forward<Arg>(arg)));
}

template <typename Arg>
constexpr auto unary_function_derivative(const et::op::tan& /*fn*/, Arg&& arg) {
    return et::make_expr(et::op::negate{}, et::make_expr(et::op::sin{}, std::forward<Arg>(arg)));
}

template <int i, typename V>
struct xform_derivative {

    template <int j, typename T>
    constexpr auto operator ()(const var<j, T>& /* v */) const {
        return not_dependent{};
    }

    template <typename T>
    constexpr auto operator ()(const var<i, T>& /* v */) const {
        return et::make_terminal(one<derivative_type<T, V>>{});
    }

    template <typename T>
    constexpr auto operator ()(const T& /* v */) const {
        return not_dependent{};
    }

    template <typename Func, typename Arg1, typename Arg2, typename... Args>
    constexpr auto operator ()(const et::expr<Func, Arg1, Arg2, Args...>& /*e*/) const {
        static_assert(false, "Can not take derivative of an unknown function");
    }

    template <typename Func>
    constexpr auto operator ()(const et::expr<Func>& /*e*/) const {
        return not_dependent{};
    }

    // Generic implementation for unary functions
    template <typename Func, typename Arg>
    constexpr auto operator ()(const et::expr<Func, Arg>& e) const {
        auto d = xform_derivative{}(e.arg1);
        if constexpr(is_dependent<decltype(d)>){
            return et::make_expr(et::op::multiplies{}, unary_function_derivative(e.op, e.arg1), d);
        }
        else {
            return not_dependent{};
        }

    }

    // The following are special implementations for unary functions:

    template <typename Expr>
    constexpr auto operator ()(const et::expr<et::op::identity, Expr>& e) const {
        return xform_derivative{}(e.arg1);
    }

    template <typename Expr>
    constexpr auto operator ()(const et::expr<et::op::ipow<0>, Expr>& /*e*/) const {
        return not_dependent{};
    }

    template <typename Expr>
    constexpr auto operator ()(const et::expr<et::op::ipow<1>, Expr>& e) const {
        return xform_derivative{}(e.arg1);
    }

    template <typename Expr, int exp>
    constexpr auto operator ()(const et::expr<et::op::ipow<exp>, Expr>& e) const {
        auto d = xform_derivative{}(e.arg1);
        if constexpr(is_dependent<decltype(d)>){
            return et::make_terminal(exp) * ipow<exp-1>(e.arg1) * d;
        }
        else {
            return not_dependent{};
        }
    }

    template <typename Expr>
    constexpr auto operator ()(const et::expr<et::op::negate, Expr>& e) const {
        auto d = xform_derivative{}(e.arg1);
        if constexpr(is_dependent<decltype(d)>){
            return et::make_expr(et::op::negate{}, d);
        }
        else {
            return not_dependent{};
        }
    }

    template <typename LHS, typename RHS>
    constexpr auto operator()(const et::expr<et::op::plus, LHS, RHS>& e) const {
        auto d1 = xform_derivative{}(e.arg1);
        auto d2 = xform_derivative{}(e.arg2);
        if constexpr (is_dependent<decltype(d1)>) {
            if constexpr (is_dependent<decltype(d2)>) {
                return d1 + d2;
            }
            else {
                return d1;
            }
        }
        else {
            if constexpr (is_dependent<decltype(d2)>) {
                return d2;
            }
            else {
                return not_dependent{};
            }
        }
    }

    template <typename LHS, typename RHS>
    constexpr auto operator()(const et::expr<et::op::minus, LHS, RHS>& e) const {
        auto d1 = xform_derivative{}(e.arg1);
        auto d2 = xform_derivative{}(e.arg2);
        if constexpr (is_dependent<decltype(d1)>) {
            if constexpr (is_dependent<decltype(d2)>) {
                return d1 - d2;
            }
            else {
                return d1;
            }
        }
        else {
            if constexpr (is_dependent<decltype(d2)>) {
                return - d2;
            }
            else {
                return not_dependent{};
            }
        }
    }

    template <typename LHS, typename RHS>
    constexpr auto operator()(const et::expr<et::op::multiplies, LHS, RHS>& e) const {
        auto d1 = xform_derivative{}(e.arg1);
        auto d2 = xform_derivative{}(e.arg2);
        if constexpr (is_dependent<decltype(d1)>) {
            if constexpr (is_dependent<decltype(d2)>) {
                return d1 * e.arg2 + e.arg1 * d2;
            }
            else {
                return d1 * e.arg2;
            }
        }
        else {
            if constexpr (is_dependent<decltype(d2)>) {
                return e.arg1 * d2;
            }
            else {
                return not_dependent{};
            }
        }
    }

    template <typename LHS, typename RHS>
    constexpr auto operator()(const et::expr<et::op::divides, LHS, RHS>& e) const {
        auto d1 = xform_derivative{}(e.arg1);
        auto d2 = xform_derivative{}(e.arg2);
        if constexpr (is_dependent<decltype(d1)>) {
            if constexpr (is_dependent<decltype(d2)>) {
                return (d1 / e.arg2) - (e.arg1 * d2 * ipow<-2>(e.arg2));
            }
            else {
                return d1 / e.arg2;
            }
        }
        else {
            if constexpr (is_dependent<decltype(d2)>) {
                return - (e.arg1 * d2 * ipow<-2>(e.arg2));
            }
            else {
                return not_dependent{};
            }
        }
    }

    template <typename Pred, typename LHS, typename RHS>
    constexpr auto operator()(const et::expr<et::op::select, Pred, LHS, RHS>& e) const {
        return et::make_expr(et::op::select{}, e.arg1, xform_derivative{}(e.arg2), xform_derivative{}(e.arg3));
    }

};

struct propagate_const {
    // x + 0 -> x
    template <typename LHS, typename T>
    constexpr decltype(auto) operator()(const et::expr<et::op::plus, LHS, zero<T>>& e) const {
        return et::transform_matching(e.arg1, *this);
    }
    // x - 0 -> x
    template <typename LHS, typename T>
    constexpr decltype(auto) operator()(const et::expr<et::op::minus, LHS, zero<T>>& e) const {
        return et::transform_matching(e.arg1, *this);
    }
    // 0 + x -> x
    template <typename RHS, typename T>
    constexpr decltype(auto) operator()(const et::expr<et::op::plus, zero<T>, RHS>& e) const {
        return et::transform_matching(e.arg2, *this);
    }
    // 0 + 0 -> 0
    template <typename T1, typename T2>
    constexpr decltype(auto) operator()(const et::expr<et::op::plus, zero<T1>, zero<T2>>& /* e */) const {
        return zero<decltype(std::declval<T1>() + std::declval<T2>())>();
    }
    // 0 - 0 -> 0
    template <typename T1, typename T2>
    constexpr decltype(auto) operator()(const et::expr<et::op::minus, zero<T1>, zero<T2>>& /* e */) const {
        return zero<decltype(std::declval<T1>() - std::declval<T2>())>();
    }
    // 0 * 0 -> 0
    template <typename T1, typename T2>
    constexpr decltype(auto) operator()(const et::expr<et::op::multiplies, zero<T1>, zero<T2>>& /* e */) const {
        return zero<decltype(std::declval<T1>() * std::declval<T2>())>();
    }
    // x * 0 -> 0
    template <typename LHS, typename T>
    constexpr decltype(auto) operator()(const et::expr<et::op::multiplies, LHS, zero<T>>& /* e */) const {
        return zero<unknown_type>();
    }
    // x * 1 -> x
    template <typename LHS, typename T>
    constexpr decltype(auto) operator()(const et::expr<et::op::multiplies, LHS, one<T>>& e) const {
        return et::transform_matching(e.arg1, *this);
    }
    // 0 * x -> 0
    template <typename RHS, typename T>
    constexpr decltype(auto) operator()(const et::expr<et::op::multiplies, zero<T>, RHS>& /* e */) const {
        return zero<unknown_type>();
    }
    // 1 * x -> x
    template <typename RHS, typename T>
    constexpr decltype(auto) operator()(const et::expr<et::op::multiplies, one<T>, RHS>& e) const {
        return et::transform_matching(e.arg2, *this);
    }
    // 1 * 1 -> 1
    template <typename T1, typename T2>
    constexpr decltype(auto) operator()(const et::expr<et::op::multiplies, one<T1>, one<T2>>& /* e */) const {
        return one<decltype(std::declval<T1>() * std::declval<T2>())>();
    }
    // 1 / 1 -> 1
    template <typename T1, typename T2>
    constexpr decltype(auto) operator()(const et::expr<et::op::divides, one<T1>, one<T2>>& /* e */) const {
        return one<decltype(std::declval<T1>() / std::declval<T2>())>();
    }
    // 0 / x -> 0
    template <typename RHS, typename T>
    constexpr decltype(auto) operator()(const et::expr<et::op::divides, zero<T>, RHS>& /* e */) const {
        return zero<unknown_type>();
    }
};

template<int i, typename T, typename E>
constexpr auto derivative(const E& e) {
    return et::as_expr(xform_derivative<i, T>{}(e));
}

template<int i, typename T, typename E>
constexpr auto derivative(const E& e, const var<i, T>& /*v*/) {
    return et::as_expr(xform_derivative<i, T>{}(e));
}

template <typename E, typename Transform>
constexpr auto transform_to_convergence(const E& e, Transform&& transform)
{
    auto transformed = et::transform_matching(std::move(e), transform);
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(e)>, std::remove_cvref_t<decltype(transformed)>>) {
        return transformed;
    }
    else {
        return transform_to_convergence(std::move(transformed), std::move(transform));
    }
}

} // namespace autodiff
