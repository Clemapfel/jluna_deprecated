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

    {
        auto* res = State::script(R"(return "abcdef")");

        auto* as_string = jl_string_ptr(res);

        for (size_t i = 0; i < jl_string_len(res); ++i)
            std::cout << as_string[i] << std::endl;
    }

    std::cout << std::endl;

    {
        auto* res = (jl_array_t*) State::script("return [1, 2, 3, 4]");
        jl_value_t* data = (jl_value_t*) res->data;

        for (size_t i = 0; i < res->length; ++i)
        {
            std::cout << jl_unbox_int64(data) << std::endl;
            auto* temp = (int64_t*) data;
            temp += 1;
            data = (jl_value_t*) temp;
        }


    }
}
