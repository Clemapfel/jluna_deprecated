#include "cpplibrary.hpp"

#ifdef __cplusplus

#include <iostream>
#include <jluna.hpp>

extern "C"
{
    jl_value_t* cpp_jl_to_string(jl_value_t* v)
    {
        std::cout << "cpp prints: " << jl_to_string(v) << std::endl;
        return v;
    }

    void cpp_void()
    {
        std::cout << "cpp called" << std::endl;
        jl_eval_string("println(\"void\"))");
    }
}

#endif

// g++ -fpic -shared cpplibrary.cpp -o -libcpplibrary
