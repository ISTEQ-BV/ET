#pragma once

#include "expr.hpp"

#include <cmath>

#define ET_UNARY_STD_FUNC(fn) \
namespace op { \
struct fn { \
    template<typename Arg> \
    decltype(auto) operator()(Arg&& arg) const \
    { \
        using std::fn; \
        return fn(std::forward<Arg>(arg)); \
    } \
}; \
} \
template<Expr Arg> \
    constexpr auto fn( Arg&& arg ) \
{ \
    return make_expr(op::fn(), std::forward<Arg>(arg)); \
} \
template <> inline constexpr std::string_view symbol_v<op::fn> = #fn;

#define ET_BINARY_STD_FUNC(fn) \
namespace op { \
struct fn { \
    template<typename Arg1, typename Arg2> \
    decltype(auto) operator()(Arg1&& arg1, Arg2&& arg2) const \
    { \
        using std::fn; \
        return fn(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2)); \
    } \
}; \
} \
template<class Arg1, class Arg2> \
requires Expr<Arg1> || Expr<Arg2> \
    constexpr auto fn( Arg1&& arg1, Arg2&& arg2) \
{ \
        return make_expr(op::fn(), std::forward<Arg1>(arg1), std::forward<Arg2>(arg2)); \
} \
template <> inline constexpr std::string_view symbol_v<op::fn> = #fn;

#define ET_TERNARY_STD_FUNC(fn) \
namespace op { \
struct fn { \
    template<typename Arg1, typename Arg2, typename Arg3> \
    decltype(auto) operator()(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3) const \
    { \
        using std::fn; \
        return fn(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3)); \
    } \
}; \
} \
template<class Arg1, class Arg2, class Arg3> \
requires Expr<Arg1> || Expr<Arg2> || Expr<Arg3> \
    constexpr auto fn( Arg1&& arg1, Arg2&& arg2, Arg3&& arg3) \
{ \
        return make_expr(op::fn(), std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3)); \
} \
template <> inline constexpr std::string_view symbol_v<op::fn> = #fn;

namespace et {

ET_UNARY_STD_FUNC(abs);
ET_UNARY_STD_FUNC(fabs);
ET_BINARY_STD_FUNC(fmod);
ET_BINARY_STD_FUNC(remainder);
ET_BINARY_STD_FUNC(remquo);
ET_TERNARY_STD_FUNC(fma);
ET_BINARY_STD_FUNC(fmax);
ET_BINARY_STD_FUNC(fmin);
ET_BINARY_STD_FUNC(fdim);

ET_UNARY_STD_FUNC(exp);
ET_UNARY_STD_FUNC(exp2);
ET_UNARY_STD_FUNC(expm1);
ET_UNARY_STD_FUNC(log);
ET_UNARY_STD_FUNC(log10);
ET_UNARY_STD_FUNC(log2);
ET_UNARY_STD_FUNC(log1p);

ET_BINARY_STD_FUNC(pow);
ET_UNARY_STD_FUNC(sqrt);
ET_UNARY_STD_FUNC(cbrt);
ET_BINARY_STD_FUNC(hypot);

ET_UNARY_STD_FUNC(sin);
ET_UNARY_STD_FUNC(cos);
ET_UNARY_STD_FUNC(tan);
ET_UNARY_STD_FUNC(asin);
ET_UNARY_STD_FUNC(acos);
ET_UNARY_STD_FUNC(atan);
ET_BINARY_STD_FUNC(atan2);

ET_UNARY_STD_FUNC(sinh);
ET_UNARY_STD_FUNC(cosh);
ET_UNARY_STD_FUNC(tanh);
ET_UNARY_STD_FUNC(asinh);
ET_UNARY_STD_FUNC(acosh);
ET_UNARY_STD_FUNC(atanh);

ET_UNARY_STD_FUNC(erf);
ET_UNARY_STD_FUNC(erfc);
ET_UNARY_STD_FUNC(tgamma);
ET_UNARY_STD_FUNC(lgamma);

ET_UNARY_STD_FUNC(ceil);
ET_UNARY_STD_FUNC(floor);
ET_UNARY_STD_FUNC(round);
ET_UNARY_STD_FUNC(lround);
ET_UNARY_STD_FUNC(llround);
ET_UNARY_STD_FUNC(nearbyint);
ET_UNARY_STD_FUNC(rint);
ET_UNARY_STD_FUNC(lrint);
ET_UNARY_STD_FUNC(llrint);

ET_UNARY_STD_FUNC(logb);
ET_UNARY_STD_FUNC(ilogb);
ET_BINARY_STD_FUNC(copysign);

ET_UNARY_STD_FUNC(fpclassify);
ET_UNARY_STD_FUNC(isfinite);
ET_UNARY_STD_FUNC(isinf);
ET_UNARY_STD_FUNC(isnan);
ET_UNARY_STD_FUNC(isnormal);
ET_UNARY_STD_FUNC(signbit);

} // namespace et
