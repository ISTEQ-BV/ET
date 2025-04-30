// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Ilya Popov

#pragma once

#include "expr.hpp"

#include <ostream>

namespace et {

template <typename T>
std::ostream& write_dot_nodes(std::ostream& s, T&& /*e*/, const std::string& prefix) {
    s << "    " << prefix << " [label=\"" << et::symbol_v<T&&> << "\",shape=box];\n";
    return s;
}

template <typename Op, typename... Args>
std::ostream& write_dot_nodes(std::ostream& s, const et::expr<Op, Args...>& e, const std::string& prefix) {

    s << "    " << prefix << " [label=\"" << et::symbol_v<Op> << "\"];\n";

    if constexpr (sizeof...(Args) > 0) {
        s << "    " << prefix << " -> " << (prefix + "_1") << " ;\n";
        write_dot_nodes(s, e.arg1, prefix + "_1");
    }
    if constexpr (sizeof...(Args) > 1) {
        s << "    " << prefix << " -> " << (prefix + "_2") << " ;\n";
        write_dot_nodes(s, e.arg2, prefix + "_2");
    }
    if constexpr (sizeof...(Args) > 2) {
        s << "    " << prefix << " -> " << (prefix + "_3") << " ;\n";
        write_dot_nodes(s, e.arg3, prefix + "_3");
    }

    return s;
}

template <et::Expr E>
std::ostream& write_dot_graph(std::ostream& s, const E& e) {
    s << "digraph \"\" {\n";
    s << "    node [fontname=\"monospace\"];\n";

    write_dot_nodes(s, e, "root");

    s << "}\n";

    return s;
}

} // namespace et
