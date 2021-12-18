// 
// Copyright 2021 Clemens Cords
// Created on 17.12.21 by clem (mail@clemens-cords.com)
//

#include <iostream>

#include <primitive_proxy.hpp>
#include <state.hpp>
#include <julia_extension.h>


using namespace jlwrap;

int main()
{
    auto state = State();
    state.initialize();

    jl_value_t* float16 = state.script("return Char(123)");
    auto proxy = PrimitiveProxy(nullptr, float16);

    //State::script("print(abc);");
    proxy.assign<float>(1.0);
    //State::script("print(abc);");
}
