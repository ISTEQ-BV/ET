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
        decltype(auto) operator()(T&& x) const {
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
            return s << "(var_" << i << " = " << v.arg1 << ")";
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

template <int i, typename V>
struct xform_derivative {

    template <int j, typename T>
    auto operator ()(const var<j, T>& /* v */) {
        return et::make_terminal(zero<derivative_type<T, V>>{});
    }

    template <typename T>
    auto operator ()(const var<i, T>& /* v */) {
        return et::make_terminal(one<derivative_type<T, V>>{});
    }

    template <typename T>
    auto operator ()(const T& /* v */) {
        return et::make_terminal(zero<derivative_type<T, V>>{});
    }

    template <typename Func, typename... Args>
    auto operator ()(const et::expr<Func, Args...>& /*e*/) {
        //static_assert(dependent_false_v<Func>, "Can not take derivative of an unknown function");
        static_assert(false, "Can not take derivative of an unknown function");
    }

    template <typename Expr>
    auto operator ()(const et::expr<et::op::identity, Expr>& e) {
        return xform_derivative{}(e.arg1);
    }

    template <typename Expr>
    auto operator ()(const et::expr<et::op::cos, Expr>& e) {
        return - et::sin(e.arg1) * xform_derivative{}(e.arg1);
    }

    template <typename Expr, int exp>
    auto operator ()(const et::expr<et::op::ipow<exp>, Expr>& e) {
        return et::make_terminal(exp) * ipow<exp-1>(e.arg1) * xform_derivative{}(e.arg1);
    }

    template <typename Expr>
    auto operator ()(const et::expr<et::op::ipow<0>, Expr>& /*e*/) {
        return zero<unknown_type>{};
    }

    template <typename Expr>
    auto operator ()(const et::expr<et::op::ipow<1>, Expr>& e) {
        return xform_derivative{}(e.arg1);
    }

    template <typename Expr>
    auto operator ()(const et::expr<et::op::tan, Expr>& e) {
        return ipow<-2>(cos(e.arg1)) * xform_derivative{}(e.arg1);
    }

    template <typename LHS, typename RHS>
    auto operator()(const et::expr<et::op::plus, LHS, RHS>& e) {
        return xform_derivative{}(e.arg1) + xform_derivative{}(e.arg2);
    }

    template <typename LHS, typename RHS>
    auto operator()(const et::expr<et::op::minus, LHS, RHS>& e) {
        return xform_derivative{}(e.arg1) - xform_derivative{}(e.arg2);
    }

    template <typename LHS, typename RHS>
    auto operator()(const et::expr<et::op::multiplies, LHS, RHS>& e) {
        return xform_derivative{}(e.arg1) * e.arg2 + e.arg1 * xform_derivative{}(e.arg2);
    }

    template <typename LHS, typename RHS>
    auto operator()(const et::expr<et::op::divides, LHS, RHS>& e) {
        return (xform_derivative{}(e.arg1) / e.arg2) - (e.arg1 * xform_derivative{}(e.arg2) * ipow<-2>(e.arg2));
    }
};

// struct propagate_const {
//     // x + 0 -> x
//     template <typename LHS, typename T>
//     auto operator()(yap::expr_tag<yap::expr_kind::plus>, const LHS& lhs, const zero<T>& /* rhs */) {
//         return yap::transform(yap::as_expr(lhs), *this);
//     }
//     // x - 0 -> x
//     template <typename LHS, typename T>
//     auto operator()(yap::expr_tag<yap::expr_kind::minus>, const LHS& lhs, const zero<T>& /* rhs */) {
//         return yap::transform(yap::as_expr(lhs), *this);
//     }
//     // 0 + x -> 0
//     template <typename RHS, typename T>
//     auto operator()(yap::expr_tag<yap::expr_kind::plus>, const zero<T>& /* lhs */, const RHS& rhs) {
//         return yap::transform(yap::as_expr(rhs), *this);
//     }
//     // 0 + 0 -> 0
//     template <typename T1, typename T2>
//     auto operator()(yap::expr_tag<yap::expr_kind::plus>, const zero<T1>& /* lhs */, const zero<T2>& /* rhs */) {
//         return yap::make_terminal(zero<decltype(std::declval<T1>() + std::declval<T2>())>{});
//     }
//     // 0 - 0 -> 0
//     template <typename T1, typename T2>
//     auto operator()(yap::expr_tag<yap::expr_kind::minus>, const zero<T1>& /* lhs */, const zero<T2>& /* rhs */) {
//         return yap::make_terminal(zero<decltype(std::declval<T1>() - std::declval<T2>())>{});
//     }
//     // 0 * 0 -> 0
//     template <typename T1, typename T2>
//     auto operator()(yap::expr_tag<yap::expr_kind::multiplies>, const zero<T1>& /* lhs */, const zero<T2>& /* rhs */) {
//         return yap::make_terminal(zero<decltype(std::declval<T1>() * std::declval<T2>())>{});
//     }
//     // x * 0 -> 0
//     template <typename LHS, typename T>
//     auto operator()(yap::expr_tag<yap::expr_kind::multiplies>, const LHS& /* lhs */, const zero<T>& /* rhs */) {
//         return yap::make_terminal(zero<unknown_type>{});
//     }
//     // x * 1 -> x
//     template <typename LHS, typename T>
//     auto operator()(yap::expr_tag<yap::expr_kind::multiplies>, const LHS& lhs, const one<T>& /* rhs */) {
//         return yap::transform(yap::as_expr(lhs), *this);
//     }
//     // 0 * x -> 0
//     template <typename RHS, typename T>
//     auto operator()(yap::expr_tag<yap::expr_kind::multiplies>, const zero<T>& /* lhs */, const RHS& /* rhs */) {
//         return yap::make_terminal(zero<unknown_type>{});
//     }
//     // 1 * x -> x
//     template <typename RHS, typename T>
//     auto operator()(yap::expr_tag<yap::expr_kind::multiplies>, const one<T>& /* lhs */, const RHS& rhs) {
//         return yap::transform(yap::as_expr(rhs), *this);
//     }
//     // 1 * 1 -> 1
//     template <typename T1, typename T2>
//     auto operator()(yap::expr_tag<yap::expr_kind::multiplies>, const one<T1>& /* lhs */, const one<T2>& /* rhs */) {
//         return yap::make_terminal(one<decltype(std::declval<T1>() * std::declval<T2>())>{});
//     }
//     // 1 / 1 -> 1
//     template <typename T1, typename T2>
//     auto operator()(yap::expr_tag<yap::expr_kind::divides>, const one<T1>& /* lhs */, const one<T2>& /* rhs */) {
//         return yap::make_terminal(one<decltype(std::declval<T1>() / std::declval<T2>())>{});
//     }
//     // 0 / x -> 0
//     template <typename RHS, typename T>
//     auto operator()(yap::expr_tag<yap::expr_kind::divides>, const zero<T>& /* lhs */, const RHS& /* rhs */) {
//         return yap::make_terminal(zero<unknown_type>{});
//     }
// };

template<int i, typename T, typename E>
auto derivative(const E& e) {
    return xform_derivative<i, T>{}(e);
}

template<int i, typename T, typename E>
auto derivative(const E& e, const var<i, T>& /*v*/) {
    return xform_derivative<i, T>{}(e);
}

template <typename Expr, typename Transform>
auto transform_to_convergence(Expr&& e, Transform&& transform)
{
    auto transformed = transform(std::move(e));
    if constexpr (std::is_same_v<std::remove_reference_t<decltype(e)>, std::remove_reference_t<decltype(transformed)>>) {
        return transformed;
    }
    else {
        return transform_to_convergence(std::move(transformed), std::move(transform));
    }
}

} // namespace autodiff
