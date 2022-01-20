// 
// Copyright 2022 Clemens Cords
// Created on 19.01.22 by clem (mail@clemens-cords.com)
//

#ifdef __cplusplus

#include <include/state.hpp>

extern "C"
{
    void cpp_test()
    {
        jluna::test();
    }
}

#else

void cpp_test();

#endif