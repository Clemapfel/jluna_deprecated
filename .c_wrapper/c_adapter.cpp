#include "c_adapter.hpp"

#ifdef __cplusplus

#include <jluna.hpp>
#include <include/cppcall.hpp>
#include <iostream>

extern "C"
{
    void call_cpp(size_t id)
    {
        std::cout << jluna::cppcall::_functions.size()+1 << std::endl;

        //jluna::cppcall::call_from_julia(id);
    }
}

#endif

// g++ -fpic -shared cpplibrary.cpp -o -libcpplibrary
