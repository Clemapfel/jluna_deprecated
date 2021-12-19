// 
// Copyright 2021 Clemens Cords
// Created on 17.12.21 by clem (mail@clemens-cords.com)
//

#include <iostream>

#include <primitive_proxy.hpp>
#include <state.hpp>
#include <julia_extension.h>
#include <function_proxy.hpp>


using namespace jlwrap;

int main()
{
    State::initialize();
    State::script("x = function() print(1) end");
    auto* f = jl_eval_string("return x");
    jl_function_t* as_f = (jl_function_t*) f;

    jl_call0(as_f);
}
