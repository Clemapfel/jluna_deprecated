// 
// Copyright 2021 Clemens Cords
// Created on 26.12.21 by clem (mail@clemens-cords.com)
//

#include <exceptions.hpp>
#include <julia.h>

namespace jlwrap
{
    JuliaException::JuliaException(jl_value_t* exception, std::string stacktrace)
                : _value(exception), _message(stacktrace)
            {}

    const char* JuliaException::what() const noexcept
    {
        return _message.c_str();
    }

    JuliaExceptionType JuliaException::get_type()
    {
        return _type;
    }
}