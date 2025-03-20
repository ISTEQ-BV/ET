#include "et/expr.hpp"
#include "et/print.hpp"
#include "et/math.hpp"
#include "et/graphviz.hpp"

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
constexpr auto e = (et::make_terminal(1) + 2) * 3;
constexpr int v [[maybe_unused]] = evaluate(e);

void test_placeholders() {

    using namespace std::placeholders;

    static_assert(et::detail::Placeholder<decltype(_1)>);

    auto e1 = et::make_terminal(_1) * _2 - _3;

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
}

int main() {
    test_print_eval(et::make_terminal(3), "identity(3)", 3);

    test_print_eval(et::make_terminal(3) + 7, "3 + 7", 10);

    test_print_eval((et::make_terminal(8) + 9) + 1, "(8 + 9) + 1", 18);

    test_print_eval(1 + (et::make_terminal(8) + 9), "1 + (8 + 9)", 18);

    test_print(1 * et::make_terminal(5) + "foo", "1 * 5 + foo");

    test_print(1 * (et::make_terminal(5) + "foo"), "1 * (5 + foo)");

    test_print_eval(select(true, et::make_terminal(5) + 3, 35), "select(1, 5 + 3, 35)", 8);

    test_print_eval(sqrt(et::make_terminal(4.0)), "sqrt(4)", 2.0);

    test_placeholders();

    using namespace std::placeholders;
    std::ofstream dot("expr.dot");
    et::write_dot_graph(dot, (et::make_terminal(_1) + _2) * _3 + _4);
}
