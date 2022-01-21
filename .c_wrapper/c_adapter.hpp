// 
// Copyright 2022 Clemens Cords
// Created on 19.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <stddef.h>

#ifdef __cplusplus

extern "C"
{
    void call_cpp(size_t);
}

#elif

void call_cpp(size_t);

#endif