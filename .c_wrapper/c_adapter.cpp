#include "c_adapter.hpp"

#ifdef __cplusplus

#include <iostream>
#include <jluna.hpp>

extern "C"
{
    auto test = std::make_unique<std::set<size_t>>();

    void call_cpp(long unsigned int id)
    {
        jluna::State::call_function(id);
    }
}

#endif

// g++ -fpic -shared cpplibrary.cpp -o -libcpplibrary
