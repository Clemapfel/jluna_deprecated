// 
// Copyright 2021 Clemens Cords
// Created on 17.12.21 by clem (mail@clemens-cords.com)
//

#include <iostream>

#include <primitive_proxy.hpp>
#include <state.hpp>
#include <julia_extension.h>
#include <function_proxy.hpp>
#include <array_proxy.hpp>


using namespace jlwrap;

int main()
{
    State::initialize();

    auto* res = State::script("return Main");
    auto* type = jl_typeof(res);

    return 0;

    {
        auto* res = State::script(R"(return "abcdef")");

        auto* as_string = jl_string_ptr(res);

        for (size_t i = 0; i < jl_string_len(res); ++i)
            std::cout << as_string[i] << std::endl;
    }

    std::cout << std::endl;

    {
        auto* res = (jl_array_t*) State::script("Array{Float32, 1}([1, 2, 3, 4])");
        jl_value_t* data = (jl_value_t*) res->data;
        //jl_arrayset(res, jl_box_int64(10), 2);

        for (size_t i = 0; i < res->length; ++i)
        {
            std::cout << jl_unbox_float32(jl_arrayref(res, i)) << std::endl;
        }
    }
}
