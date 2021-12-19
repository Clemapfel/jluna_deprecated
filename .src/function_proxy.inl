// 
// Copyright 2021 Clemens Cords
// Created on 19.12.21 by clem (mail@clemens-cords.com)
//

#include <function_proxy.hpp>
#include <vector>

namespace jlwrap
{
    Function::Function(jl_function_t* f)
        : Proxy((jl_value_t*) f)
    {}

    template<typename... Arg_ts>
    decltype(auto) Function::operator()(Arg_ts... args)
    {
        std::vector<jl_value_t*> as_args = {static_cast<Proxy*>(&args)->_value, ...};

    }
}