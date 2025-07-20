// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov

#include "et/derivative.hpp"

#include "et/print.hpp"

#include <cassert>

using namespace autodiff;

void foo() {
    auto x = var<0>(1.0);
    auto y = var<1>(2);
    double t = 5;

    std::cout << "x = " << x << '\n';
    std::cout << "y = " << y << '\n';
    std::cout << "t = " << t << '\n';

    std::cout << "dx/dx = " << derivative(x, x) << " = " << evaluate(derivative(x, x)) << '\n';
    std::cout << "dx/dy = " << derivative(x, y) << " = " << evaluate(derivative(x, y)) << '\n';
    std::cout << "dy/dx = " << derivative(y, x) << " = " << evaluate(derivative(y, x)) << '\n';
    std::cout << "dy/dy = " << derivative(y, y) << " = " << evaluate(derivative(y, y)) << '\n';
    std::cout << '\n';

    auto z = x * t + y + x * y;
    std::cout << "z = " << z << '\n';
    std::cout << "type_name(z) = " << et::type_name<decltype(z)> << '\n';
    std::cout << '\n';

    auto dz_dx = derivative(z, x);
    std::cout << "dz/dx = " << dz_dx << '\n';
    std::cout << "type_name(dz/dx) = " << et::type_name<decltype(dz_dx)> << '\n';
    std::cout << '\n';

    auto z1 = et::transform_matching(z, propagate_const{});
    std::cout << "z1 = " << z1 << " = " << evaluate(z1) << '\n';
    std::cout << "type_name(z1) = " << et::type_name<decltype(z1)> << '\n';
    std::cout << '\n';

    auto s1 = et::transform_matching(dz_dx, propagate_const{});
    std::cout << "s1 = " << s1 /*<< " = " << evaluate(s1) */ << '\n';
    std::cout << "type_name(s1) = " << et::type_name<decltype(s1)> << '\n';
    std::cout << '\n';

    auto s2 = et::transform_matching(s1, propagate_const{});
    std::cout << "s2 = " << s2 << " = " << evaluate(s2) << '\n';
    std::cout << "type_name(s2) = " << et::type_name<decltype(s2)> << '\n';
    std::cout << '\n';

    auto s3 = et::transform_matching(s2, propagate_const{});
    std::cout << "s2 = " << s3 << " = " << evaluate(s3) << '\n';
    std::cout << "type_name(s2) = " << et::type_name<decltype(s3)> << '\n';
    std::cout << '\n';

    std::cout << "simplified = " << autodiff::transform_to_convergence(dz_dx, propagate_const{}) << '\n';
    std::cout << "type_name(simplified) = " << et::type_name<decltype(autodiff::transform_to_convergence(dz_dx, propagate_const{}))> << '\n';
    std::cout << '\n';

    auto w = x + 3;
    std::cout << "w = " << w << '\n';
    auto dw_dx = derivative(w, x);
    std::cout << "dw/dx = " << dw_dx << '\n';
    auto simp1 = autodiff::transform_to_convergence(dw_dx, propagate_const{});
    std::cout << "simplified = " << simp1 << " = " << et::evaluate(simp1) << '\n';
    auto dw_dy = derivative(w, y);
    std::cout << "dw/dy = " << dw_dy << '\n';
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

template<typename T1,
         typename T2,
         typename T3,
         typename T4,
         typename T5,
         typename T6,
         typename T7
         >
auto cons_from_prim(const T1& p, const T2& Ux, const T3& Uy, const T4& Uz, const T5& T, const T6& m, const T7& Cv) {
    auto R = 8.31;
    auto rho = p * m / (et::detail::copy(R) * T);
    auto rhoUx = rho * Ux;
    auto rhoUy = rho * Uy;
    auto rhoUz = rho * Uz;
    auto rhoE = rho * (0.5 * (Ux*Ux + Uy*Uy + Uz*Uz) + Cv*T);
    return std::make_tuple(std::move(rho), std::move(rhoUx), std::move(rhoUy), std::move(rhoUz), std::move(rhoE));
}

void test2() {
    auto p = var<0>(140);
    auto Ux = var<1>(100);
    auto Uy = var<2>(0);
    auto Uz = var<3>(0);
    auto T = var<4>(300);
    auto mu = var<5>(0.002);
    auto Cv = var<6>(1000);

    std::cout << "p =  " << p  << '\n';
    std::cout << "Ux = " << Ux << '\n';
    std::cout << "Uy = " << Uy << '\n';
    std::cout << "Uz = " << Uz << '\n';
    std::cout << "T =  " << T  << '\n';

    auto [rho, rhoUx, rhoUy, rhoUz, rhoE] = cons_from_prim(p, Ux, Uy, Uz, T, mu, Cv);

    std::cout << "rho   = " << rho   << '\n';
    std::cout << "rhoUx = " << rhoUx << '\n';
    std::cout << "rhoUy = " << rhoUy << '\n';
    std::cout << "rhoUz = " << rhoUz << '\n';
    std::cout << "rhoE  = " << rhoE  << '\n';

    auto drho_dp = autodiff::derivative(rho, p);
    std::cout << "drho/dp = " << drho_dp << '\n';

    auto drho_dp6 = transform_to_convergence(drho_dp, autodiff::propagate_const{});
    std::cout << "drho/dp = " << drho_dp6 << " = " << evaluate(drho_dp6) << '\n';

    auto drhoE_dT = autodiff::derivative(rhoE, T);
    std::cout << "drhoE/dT = " << drhoE_dT << '\n';

    auto drhoE_dT1 = transform_to_convergence(drhoE_dT, autodiff::propagate_const{});
    std::cout << "drhoE/dT = " << drhoE_dT1 << /*" = " << et::evaluate(drhoE_dT1) <<*/ '\n';

}

int main() {
    foo();
    test2();
}
