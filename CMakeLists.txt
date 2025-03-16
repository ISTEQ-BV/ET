cmake_minimum_required(VERSION 3.10.0)

project(et VERSION 0.1.0 LANGUAGES CXX)

add_library(et
    include/et/derivative.hpp
    include/et/expr.hpp
    include/et/math.hpp
    include/et/print.hpp
    include/et/type_name.hpp

    src/print.cpp
)
target_include_directories(et PUBLIC
    include/
)

target_compile_features(et PUBLIC cxx_std_20)
target_compile_options(et PRIVATE -Wall -Wextra)

add_executable(et_test
    test/et_test.cpp
)
target_link_libraries(et_test
    PRIVATE et
)
target_compile_options(et_test PRIVATE -g3 -Wall -Wextra)

add_executable(derivative_test
    test/derivative_test.cpp
)
target_link_libraries(derivative_test
    PRIVATE et
)
target_compile_options(derivative_test PRIVATE -g3 -Wall -Wextra)
