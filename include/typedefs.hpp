// 
// Copyright 2022 Clemens Cords
// Created on 12.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <complex>

namespace jluna
{
    // typedefs for prettier code

    using Bool = bool;
    using Char = uint8_t;

    using Int8 = int8_t;
    using Int16 = int16_t;
    using Int32 = int32_t;
    using Int64 = int64_t;

    using UInt8 = uint8_t;
    using UInt16 = uint16_t;
    using UInt32 = uint32_t;
    using UInt64 = uint64_t;

    using Float32 = float;
    using Float64 = double;

    using Any = jl_value_t*;
    using Symbol_t = jl_value_t*;
}