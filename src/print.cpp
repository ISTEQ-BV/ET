// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov

#include "et/print.hpp"

std::ostream &et::tr::detail::print_expr_header_impl(std::ostream &s, int indent, std::string_view symbol, std::string_view type_name) {
    s << std::string(indent, ' ')
    << "expr \"" << symbol << "\" "
    << '<' << type_name << '>'
    << '\n';
    return s;
}

std::ostream &et::tr::detail::print_terminal_impl(std::ostream &stream, int indent, std::string_view type_name)
{
    return stream << std::string(indent, ' ') << "terminal <" << type_name << ">";
}
