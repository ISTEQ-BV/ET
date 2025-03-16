#include "et/derivative.hpp"

#include "et/print.hpp"

#include <cassert>

using namespace autodiff;

template <typename T>
struct print;

void foo() {
    var<0> x(1.0);
    var<1> y(2);
    double t(5);

    std::cout << "x = " << x << '\n';
    std::cout << "y = " << y << '\n';
    std::cout << "t = " << t << '\n';

    std::cout << "dx/dx = " << derivative(x, x) << " = " << evaluate(derivative(x, x)) << '\n';
    std::cout << "dx/dy = " << derivative(x, y) << " = " << evaluate(derivative(x, y)) << '\n';
    std::cout << "dy/dx = " << derivative(y, x) << " = " << evaluate(derivative(y, x)) << '\n';
    std::cout << "dy/dy = " << derivative(y, y) << " = " << evaluate(derivative(y, y)) << '\n';

    auto z = x * t + y + x * y;
    std::cout << "z = " << z << '\n';

    auto dz_dx = derivative(z, x);
    std::cout << "dz/dx = " << dz_dx << '\n';
    // auto s1 = yap::transform(dz_dx, propagate_const{});
    // std::cout << "s1 = ";
    // yap::print(std::cout, s1);
    // auto s2 = yap::transform(s1, propagate_const{});
    // std::cout << "s2 = ";
    // yap::print(std::cout, s2);

    //print<typeof d> b;

    //    assert(yap::evaluate(derivative<0>(z)) == 1.0);
    //    assert(z.derivative(y).value() == 1.0);
    //    assert(z.derivative(t).value() == 0.0);

    //    return z.value();

    auto w = x + 3;
    std::cout << "w = " << w << '\n';
    auto dw_dx = derivative(w, x);
    std::cout << "dw/dx = " << dw_dx << '\n';
    auto dw_dy = derivative(w, y);
    std::cout << "dw/dy = " << dw_dy << '\n';
    // std::cout << "simplified = ";
    // yap::print(std::cout, yap::transform(dw_dx, propagate_const{}));
}

//template<
//    typename T1,
//    typename T2,
//    typename T3,
//    typename T4,
//    typename T5,
//    typename T6,
//    typename T7
//    >
//auto select(T1 S_l, T2 S_star, T3 S_r, T4 F1, T5 F2, T6 F3, T7 F4)
//{
//    return if_else(S_star > 0.0,
//                   if_else(S_l > 0.0,
//                           F1,
//                           F2
//                           ),
//                   if_else(S_r > 0.0,
//                           F3,
//                           F4
//                           )
//                   );
//}

//template<typename T1,
//         typename T2,
//         typename T3,
//         typename T4,
//         typename T5,
//         typename T6,
//         typename T7,
//         typename T8,
//         typename T9,
//         typename T10,
//         typename T11,
//         typename T12
//         >
//auto hllc_flux(
//    T1 rho_l, T2 rhoUx_l, T3 rhoUy_l, T4 rhoUz_l, T5 rhoE_l, T6 p_l,
//    T7 rho_r, T8 rhoUx_r, T9 rhoUy_r, T10 rhoUz_r, T11 rhoE_r, T12 p_r,
//    double gamma_l, double gamma_r,
//    const double (&n)[3]
//)
//{
//    auto un_l = (rhoUx_l*n[0] + rhoUy_l*n[1] + rhoUz_l*n[2]) / rho_l;
//    auto un_r = (rhoUx_r*n[0] + rhoUy_r*n[1] + rhoUz_r*n[2]) / rho_r;

//    auto a_l = sqrt(gamma_l * p_l/rho_l);
//    auto a_r = sqrt(gamma_r * p_r/rho_r);

//    auto sqrt_rho_l = sqrt(rho_l);
//    auto sqrt_rho_r = sqrt(rho_r);

//    auto w_l = sqrt_rho_l/(sqrt_rho_l + sqrt_rho_r);
//    auto w_r = 1.0 - w_l;

//    // 10.50
//    auto u_tilde = w_l*un_l + w_r*un_r;

//    // 10.53, 10.54
//    auto d = sqrt(w_l*a_l*a_l + w_r*a_r*a_r
//                  + 0.5*w_l*w_r*(un_r - un_l)*(un_r - un_l));

//    // 10.52
//    auto S_l = u_tilde - d;
//    auto S_r = u_tilde + d;

//    // 10.37
//    auto S_star = (  p_r - p_l + rho_l*un_l*(S_l - un_l) - rho_r*un_r*(S_r - un_r))
//                  /(rho_l * (S_l - un_l) - rho_r * (S_r - un_r));

//    // 10.36
//    auto p_star = p_l + rho_l * (S_l - un_l) * (S_star - un_l);
//}

// template <typename T>
// auto copy(T&& x) -> std::remove_cv_t<std::remove_reference_t<T>>
// {
//     return std::forward<T>(x);
// }

// template<typename T1,
//          typename T2,
//          typename T3,
//          typename T4,
//          typename T5
//          >
// auto cons_from_prim(const T1& p, const T2& Ux, const T3& Uy, const T4& Uz, const T5& T, const double& m, const double& Cv) {
//     auto R = 8.31;
//     auto rho = p*copy(m) / (copy(R) * T);
//     yap::print(std::cout, rho);
//     auto rhoUx = copy(rho)*Ux;
//     auto rhoUy = copy(rho)*Uy;
//     auto rhoUz = copy(rho)*Uz;
//     auto rhoE = copy(rho)*(0.5*(Ux*Ux + Uy*Uy + Uz*Uz) + Cv*T);
//     return std::make_tuple(std::move(rho), std::move(rhoUx), std::move(rhoUy), std::move(rhoUz), std::move(rhoE));
// }

// void test2() {
//     var<0> p(140);
//     var<1> Ux(100);
//     var<2> Uy(0);
//     var<3> Uz(0);
//     var<4> T(300);

//     std::cout << "p = ";
//     yap::print(std::cout, p);
//     std::cout << "Ux = ";
//     yap::print(std::cout, Ux);
//     std::cout << "Uy = ";
//     yap::print(std::cout, Uy);
//     std::cout << "Uz = ";
//     yap::print(std::cout, Uz);
//     std::cout << "T = ";
//     yap::print(std::cout, T);

//     auto [rho, rhoUx, rhoUy, rhoUz, rhoE] = cons_from_prim(p, Ux, Uy, Uz, T, 0.002, 10000);

//     std::cout << "rho = ";
//     yap::print(std::cout, rho);
//     std::cout << "rhoUx = ";
//     yap::print(std::cout, rhoUx);
//     std::cout << "rhoUy = ";
//     yap::print(std::cout, rhoUy);
//     std::cout << "rhoUz = ";
//     yap::print(std::cout, rhoUz);
//     std::cout << "rhoE = ";
//     yap::print(std::cout, rhoE);

//     auto drho_dp = autodiff::derivative(rho, p);
//     std::cout << "drho/dp = ";
//     yap::print(std::cout, drho_dp);
//     //    auto drho_dp1 = yap::transform(drho_dp, autodiff::propagate_const{});
//     //    std::cout << "drho/dp = ";
//     //    yap::print(std::cout, drho_dp1);
//     //    auto drho_dp2 = yap::transform(drho_dp1, autodiff::propagate_const{});
//     //    std::cout << "drho/dp = ";
//     //    yap::print(std::cout, drho_dp2);
//     //    auto drho_dp3 = yap::transform(drho_dp2, autodiff::propagate_const{});
//     //    std::cout << "drho/dp = ";
//     //    yap::print(std::cout, drho_dp3);
//     //    auto drho_dp4 = yap::transform(drho_dp3, autodiff::propagate_const{});
//     //    std::cout << "drho/dp = ";
//     //    yap::print(std::cout, drho_dp4);
//     //    auto drho_dp5 = yap::transform(drho_dp4, autodiff::propagate_const{});
//     //    std::cout << "drho/dp = ";
//     //    yap::print(std::cout, drho_dp5);

//     auto drho_dp6 = transform_to_convergence(drho_dp, autodiff::propagate_const{});
//     std::cout << "drho/dp = ";
//     yap::print(std::cout, drho_dp6);
// }

int main() {
    foo();
    // test2();
}
