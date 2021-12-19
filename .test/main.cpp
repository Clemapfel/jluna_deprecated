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
    State::script("x = function(a::Int64) print(a) end");
    auto f = Function(jl_eval_string("return x"));


    f(Proxy<State>(jl_box_int32(12)));
}
