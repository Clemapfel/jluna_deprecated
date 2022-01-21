// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

//
// this file adds functionality to julia.h, however it is not part of the julia C-API
//

extern "C"
{
    /// @brief unbox float16 by converting it to float32 first
    float jl_unbox_float16(jl_value_t* v)
    {
        jl_function_t* convert = jl_get_function(jl_main_module, "convert");
        jl_value_t* res = jl_call2(convert, (jl_value_t*) (jl_float32_type), v);
        return jl_unbox_float32(res);
    }

    /// @brief get value type of array
    jl_datatype_t* jl_array_value_t(jl_array_t* v)
    {
        jl_function_t* get_type = jl_get_function(jl_main_module, "get_value_type_of_array");
        return (jl_datatype_t*) jl_call1(get_type, (jl_value_t*) v);
    }

    /// @brief return value as string
    char* jl_to_string(jl_value_t* v)
    {
        static jl_function_t* to_string = jl_get_function(jl_main_module, "string");
        return jl_string_data(jl_call1(to_string, v));
    }

    /// @brief get value of reference
    jl_value_t* jl_ref_value(jl_value_t* reference)
    {
        static jl_function_t* get_reference_value = jl_get_function((jl_module_t*) jl_eval_string("return jluna"), "get_reference_value");
        return jl_call1(get_reference_value, reference);
    }

    /// @brief undef initializer
    jl_value_t* jl_undef_initializer()
    {
        return jl_eval_string("return undef");
    }

    /// @brief const char* to julia-side string
    jl_value_t* jl_string(const char* str)
    {
        static jl_function_t* string = jl_get_function(jl_base_module, "string");
        return jl_call1(string, (jl_value_t*) jl_symbol(str));
    }

    /// @brief get nth element of tuple
    jl_value_t* jl_tupleref(jl_value_t* tuple, size_t n)
    {
        static jl_function_t* get = jl_get_function(jl_base_module, "get");
        return jl_call3(get, tuple, jl_box_uint64(n + 1), jl_undef_initializer());
    }

    /// @brief get length of tuple
    size_t jl_tuple_len(jl_value_t* tuple)
    {
        static jl_function_t* length = jl_get_function(jl_base_module, "length");
        return jl_unbox_int64(jl_call1(length, tuple));
    }

    /// @brief hash julia-side
    size_t jl_hash(const char* str)
    {
        static jl_function_t* hash = jl_get_function(jl_base_module, "hash");
        return jl_unbox_uint64(jl_call1(hash, (jl_value_t*) jl_symbol(str)));
    }

    /// @brief get proper typeof as str
    const char* jl_verbose_typeof_str(jl_value_t* v)
    {
        static jl_function_t* type_of = jl_get_function(jl_base_module, "typeof");
        return jl_to_string(jl_call1(type_of, v));
    }
}


