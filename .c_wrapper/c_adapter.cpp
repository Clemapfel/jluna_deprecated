#include "c_adapter.hpp"

#ifdef __cplusplus

#include <jluna.hpp>
#include <include/cppcall.hpp>
#include <iostream>

extern "C"
{
    void call_cpp(size_t id)
    {
        jluna::cppcall::call_from_julia(1443994487737173028);
    }
}

#endif

// g++ -fpic -shared cpplibrary.cpp -o -libcpplibrary
