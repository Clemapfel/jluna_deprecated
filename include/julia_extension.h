// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

/// this file adds functionality to julia.h, however it is not part of the julia C-API

/// @brief unbox float16 by converting it to float32 first
float jl_unbox_float16(jl_value_t* v)
{
    jl_function_t* convert = jl_get_function(jl_main_module, "convert");
    jl_value_t* res = jl_call2(convert, (jl_value_t*) (jl_float32_type), v);
    return jl_unbox_float32(res);
}


