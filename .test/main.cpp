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
#include <box_any.hpp>

#include <array>
using namespace jlwrap;

int main()
{
    State::initialize();
    State::script("include(\"/home/clem/Workspace/jlwrap/.src/common.jl\")");

    State::script("askhjash()");
    auto* exception = jl_exception_occurred();
    std::cout << jl_typeof_str(exception) << std::endl;
    return 0;

    State::script(R"(
    mutable struct Struct
       field_1::Float32
       field_2::Int64
       field_3
    end

    instance = Struct(1, 2, 3)
    )");

    auto* res = State::script("return instance");
    jl_datatype_t* as_type = (jl_datatype_t*) jl_typeof(res);

    //std::cout << jl_symbol_name(jl_field_names(as_type, 1)) << std::endl;
    std::cout << "n_fields: " << jl_nfields(res) << std::endl;
    std::cout << unbox<int32_t>(jl_get_field(res, "field_2")) << std::endl;

    jl_set_nth_field(res, 1, box('a'));
    std::cout << unbox<int32_t>(jl_get_field(res, "field_2")) << std::endl;
    //int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

    return 0;

    /*
    State::script("test_array = Matrix(undef, 5, 5)");
    State::script("test_array .= reshape(collect(1:25), 5, 5)");
    auto res = unbox<std::vector<std::vector<float>>>(State::script("test_array"));

    for (auto& s : res)
        for (auto& t : s)
            std::cout << t << std::endl;

    return 0;

    std::cout << unbox<std::string>(State::script("return [1, 2, 3, 4]")) << std::endl;
    std::cout << unbox<std::string>(State::script("return \"abcdef\"")) << std::endl;
    std::cout << unbox<std::string>(State::script("return Main")) << std::endl;


    auto arr = Array<int64_t, 1>(State::script("return Array{Int64}([1, 2, 3, 4, 5])"));

    for (size_t i = 0; i < arr.length(); ++i)
    {
        std::cout << arr.get(i) << std::endl;
    }


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
