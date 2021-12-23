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
#include <unbox_any.hpp>

using namespace jlwrap;

int main()
{
    State::initialize();
    State::script("include(\"/home/clem/Workspace/jlwrap/.src/common.jl\")");

    std::cout << unbox<std::string>(State::script("return [1, 2, 3, 4]")) << std::endl;
    std::cout << unbox<std::string>(State::script("return \"abcdef\"")) << std::endl;


    auto arr = Array<int64_t, 1>(State::script("return Array{Int64}([1, 2, 3, 4, 5])"));

    for (size_t i = 0; i < arr.length(); ++i)
    {
        std::cout << arr.get(i) << std::endl;
    }

    /*
    auto array = Array<int64_t, 1>(State::script("return [1, 2, 3, 4, 5]"));
    unbox<int64_t>(nullptr);
    std::cout << array.at(2) << std::endl;*/

    return 0;

    /*
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
     */
}
