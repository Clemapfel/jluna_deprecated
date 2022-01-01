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

    AmbiguousCandidateException::AmbiguousCandidateException(const std::string& symbol_name, const std::vector<std::string>& candidate_modules)
    {
        std::stringstream str;
        str << "querying the julia state for variable with symbol \":" << symbol_name << "\" returns more than one result. Please manually specify which module the variable is defined in.\n";
        str << "candidates are:\n";

        for (auto s : candidate_modules)
            str << "\t" << s << "." << symbol_name << "\n";

        message = str.str();
    }

    const char * AmbiguousCandidateException::what() const noexcept
    {
        return message.c_str();
    }
}