// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

//
// this file adds functionality to julia.h, however it is not part of
// the julia C-API and has no official affiliation with julias creators or rights holders
//

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

/// @brief check if variable is of given type
bool jl_isa(jl_value_t* variable, jl_datatype_t* type)
{
    return (bool) jl_types_equal(jl_typeof(variable), (jl_value_t*) type);
}




