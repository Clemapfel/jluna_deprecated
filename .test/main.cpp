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
    State::script("function x(a::Int64) print(a) end");
    auto f = Function(jl_eval_string("return x"));

    auto* res = State::script("return typeof(x)");

    f(State::wrap_primitive<int32_t>(12));
}
