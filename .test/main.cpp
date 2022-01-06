// 
// Copyright 2021 Clemens Cords
// Created on 17.12.21 by clem (mail@clemens-cords.com)
//

#include <iostream>
#include <jluna.hpp>

using namespace jluna;

int main()
{
    State::initialize();

    jluna::State::script("println(\"hello luna\")");

    auto returned_value = jluna::State::script("return 123");
    int as_int = returned_value;
    as_int += 1;
    std::cout << as_int << std::endl;

    /*
    auto proxy = Proxy<State>(jl_eval_string("return Base"));
    Module module = proxy;

    assert(module["println"] != nullptr);

    //Module module = State::safe_script("return Base");
    //assert(module["println"] != nullptr);

    //f("test");
    return 0;
    /*
    SafeFunction f = State::get_function("+", "Base");
    int what = f(1, 2);
    std::cout << what << std::endl;

    //auto* first = State::get_function("asdas", "Main.jluna");
    /*

    jl_eval_string(R"(
        mutable struct InnerStruct
           field::Int64
        end

        mutable struct OuterStruct
           field::InnerStruct
        end
    )");

    jl_function_t* throw_unsafe = jl_eval_string("unsafe() = throw(AssertionError(\"test\"))");
    State::call(jl_get_function(jl_main_module, "unsafe"));

    auto* instance = State::safe_script("return OuterStruct(InnerStruct(1))");
    auto outer = Proxy<State>(instance);
    auto inner = outer["field"];

    /*
    inner = State::safe_script("return InnerStruct(99)");

    State::safe_script("println(string(jluna.memory_handler._refs))");

    inner = outer["field"];

    State::safe_script("println(string(jluna.memory_handler._refs))");


    outer = Proxy<State>(State::safe_script("return OuterStruct(InnerStruct(123))"));
    inner = outer["field"];

    State::safe_script("println(string(jluna.memory_handler._refs))");;

    /*
    auto* struct_type = (jl_datatype_t*) jl_typeof(struct_instance);
    auto* field_names = jl_field_names(struct_type);

    std::cout << jl_typeof_str(struct_instance) << std::endl;
    return 0;


    jl_function_t* fieldcount = jl_get_function(jl_base_module, "fieldcount");

    for (size_t i = 0; i < jl_unbox_int64(jl_call1(fieldcount, (jl_value_t*) struct_type)); ++i)
        std::cout << jl_symbol_name((jl_sym_t*) jl_svecref(field_names, i)) << std::endl;

    return 0;

    /*
    auto* exception = jl_exception_occurred();
    std::cout << jl_typeof_str(exception) << std::endl;

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
