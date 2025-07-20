// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov

#include "et/expr.hpp"
#include "et/print.hpp"
#include "et/math.hpp"
#include "et/graphviz.hpp"
#include "et/placeholders.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

bool verify(bool x) {
    if (!x) {
        std::cerr << "Fatal error\n";
        std::exit(1);
    }
    return x;
}

template<typename E>
bool test_print(const E& e, char const* repr)
{
    std::ostringstream oss;
    verify(et::tr::print{oss}(e));
    std::cout << "Expression: " << oss.str() << '\n';
    std::cout << "Type:       " << et::type_name<E> << '\n';
    verify(oss.str() == repr);
    std::cout << "Tree:\n";
    verify((bool)et::tr::debug(std::cout, e));
    return true;
}

template<typename E, typename V>
bool test_print_eval(const E& e, char const* repr, const V& v)
{
    test_print(e, repr);
    verify(evaluate(e) == v);
    return true;
}

//test that expr can be used in consexpr context
constexpr auto e = (et::expr(1) + 2) * 3;
constexpr int v [[maybe_unused]] = evaluate(e);

int three = 3;
const int four = 4;

static_assert(std::is_same_v<decltype(et::expr(1)), et::expr<int>>);
static_assert(std::is_same_v<decltype(et::expr(1)), et::expr<int>>);
static_assert(std::is_same_v<decltype(et::expr(three)), et::expr<int&>>);
static_assert(std::is_same_v<decltype(et::expr(four)), et::expr<const int&>>);
static_assert(std::is_same_v<decltype(et::expr(std::move(four))), et::expr<const int>>);
static_assert(std::is_same_v<decltype(et::expr(et::detail::copy(four))), et::expr<int>>);

static_assert(std::is_same_v<decltype(et::unwrap(std::declval<et::expr<int>>())), int>);
static_assert(std::is_same_v<decltype(et::unwrap(std::declval<et::expr<int&>>())), int&>);
static_assert(std::is_same_v<decltype(et::unwrap(std::declval<et::expr<et::op::plus, int, int>>())), et::expr<et::op::plus, int, int>>);
static_assert(std::is_same_v<decltype(et::unwrap(std::declval<et::expr<et::op::plus, int, int>&>())), et::expr<et::op::plus, int, int>>);

static_assert(std::is_same_v<decltype(et::as_expr(7)), et::expr<int>>);
static_assert(std::is_same_v<decltype(et::as_expr(three)), et::expr<int&>>);
static_assert(std::is_same_v<decltype(et::as_expr(std::declval<et::expr<int>>())), et::expr<int>>);
static_assert(std::is_same_v<decltype(et::as_expr(std::declval<et::expr<int&>>())), et::expr<int&>>);
static_assert(std::is_same_v<decltype(et::as_expr(std::declval<et::expr<int>&>())), et::expr<int>>);
static_assert(std::is_same_v<decltype(et::as_expr(std::declval<et::expr<int>&&>())), et::expr<int>>);

static_assert(std::is_same_v<decltype(et::make_expr(1)), et::expr<int>>);
static_assert(std::is_same_v<decltype(et::make_expr(1) + 2), et::expr<et::op::plus, int, int>>);
static_assert(std::is_same_v<decltype(et::make_expr(three)), et::expr<int&>>);
static_assert(std::is_same_v<decltype(et::make_expr(std::move(three))), et::expr<int>>);
static_assert(std::is_same_v<decltype(et::make_expr(1) + three), et::expr<et::op::plus, int, int&>>);
static_assert(std::is_same_v<decltype(et::make_expr(1) + std::move(three)), et::expr<et::op::plus, int, int>>);

void test_placeholders() {

    using namespace std::placeholders;

    static_assert(et::detail::Placeholder<decltype(_1)>);

    auto e1 = et::expr(_1) * _2 - _3;

    et::tr::print{std::cout}(e1);
    std::cout << '\n';

    auto e2 = replace_placeholders(e1, std::make_tuple(2, 3, 5));

    std::cout << et::type_name<decltype(e2)> << std::endl;
    et::tr::print{std::cout}(e2);
    std::cout << '\n';

    auto f = et::apply(e1, std::make_tuple(2, 3, 5));

    if (f != 1) {
        std::cout << "Error\n";
    }

    auto g = invoke(e1, 2, 3, 5);

    if (g != 1) {
        std::cout << "Error\n";
    }

    std::ofstream dot("expr.dot");
    et::write_dot_graph(dot, (et::expr(_1) + _2) * _3 + _4);
}

int main() {
    test_print_eval(et::expr(3), "3", 3);

    test_print_eval(et::expr(3) + 7, "3 + 7", 10);

    test_print_eval((et::expr(8) + 9) + 1, "(8 + 9) + 1", 18);

    test_print_eval(1 + (et::expr(8) + 9), "1 + (8 + 9)", 18);

    test_print(1 * et::expr(5) + "foo", "1 * 5 + foo");

    test_print(1 * (et::expr(5) + "foo"), "1 * (5 + foo)");

    test_print_eval(select(true, et::expr(5) + 3, 35), "select(1, 5 + 3, 35)", 8);

    test_print_eval(sqrt(et::expr(4.0)), "sqrt(4)", 2.0);

    test_placeholders();
}
