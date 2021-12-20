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
    {
        _value = (jl_function_t*) Proxy<State>::operator _jl_value_t *();
    }

    template<typename... Args_t>
    decltype(auto) Function::operator()(Args_t... args)
    {
        std::vector<jl_value_t*> as_args;
        (as_args.push_back(args.data()), ...);

        return jl_call(reinterpret_cast<jl_function_t*>(_value), &as_args[0], as_args.size());
    }
}