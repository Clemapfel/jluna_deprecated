#include "cpplibrary.hpp"

#ifdef __cplusplus

#include <iostream>
#include <jluna.hpp>

extern "C"
{
    void cpp_test()
    {
        jluna::test();
    }
}

#endif

// g++ -fpic -shared cpplibrary.cpp -o -libcpplibrary
