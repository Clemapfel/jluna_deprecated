// 
// Copyright 2022 Clemens Cords
// Created on 19.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

#ifdef __cplusplus

#include <jluna.hpp>

extern "C"
{
    jl_value_t* cpp_jl_to_string(jl_value_t*);

    void cpp_void();
}

#endif