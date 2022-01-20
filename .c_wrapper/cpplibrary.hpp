// 
// Copyright 2022 Clemens Cords
// Created on 19.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#ifdef __cplusplus

#include <julia.h>

extern "C"
{
    void cpp_test();
}

#else

void cpp_test();

#endif