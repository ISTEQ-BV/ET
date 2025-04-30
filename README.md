ET
==

General purpose expression templates library for C++20.

Intended mostly for CFD and otgher HPC applications. 

Building
--------

    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build

Run the examples:

    ./build/et_test
    ./build/derivative_test


Including into your project
---------------------------

Option one: use cmake `add_subdirectory` assuming you have downloaded ET source code into 
`external/et`:

    add_subdirectory(external/et)

    add_executable(my_executable)
    target_link_library(my_executable PRIVATE et)


Compatibility and requirements
------------------------------

Library requires C++20, tested with GCC 14,15 and Clang 19,20

See also
------------

Boost.YAP https://www.boost.org/doc/libs/1_88_0/doc/html/yap.html

License
-------

MIT license

Copyright (c) 2025 Ilya Popov, ISTEQ BV
