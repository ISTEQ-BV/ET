cmake_minimum_required(VERSION 3.10.0)

project(et VERSION 0.1.0 LANGUAGES CXX)

add_compile_options(-Wall -Wextra -fdiagnostics-color=always)

add_library(et
    include/et/derivative.hpp
    include/et/expr.hpp
    include/et/graphviz.hpp
    include/et/math.hpp
    include/et/print.hpp
    include/et/type_name.hpp

    src/print.cpp
    include/et/placeholders.hpp
)
target_include_directories(et PUBLIC
    include/
)

target_compile_features(et PUBLIC cxx_std_20)

add_executable(et_test
    test/et_test.cpp
)
target_link_libraries(et_test
    PRIVATE et
)

add_executable(derivative_test
    test/derivative_test.cpp
)
target_link_libraries(derivative_test
    PRIVATE et
)
