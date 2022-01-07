// 
// Copyright 2021 Clemens Cords
// Created on 26.12.21 by clem (mail@clemens-cords.com)
//

#include <exceptions.hpp>
#include <julia.h>

namespace jluna
{
    void forward_last_exception()
    {
        THROW_IF_UNINITIALIZED;

        auto* maybe =jl_exception_occurred();
        if (maybe != nullptr)
        {
            static jl_function_t* tostring = jl_get_function(jl_base_module, "string");

            std::stringstream str;
            str << jl_string_data(jl_call1(tostring, jl_exception_occurred())) << "\nStacktrace: <no stacktrace available>" << std::endl;
            throw JuliaException(jl_exception_occurred(), str.str());
            return;
        }

        else if (jl_unbox_bool(jl_eval_string("return jluna.exception_handler.has_exception_occurred()")))
        {
            throw JuliaException(
                    jl_eval_string("return jluna.exception_handler.get_last_exception()"),
                    std::string(jl_string_data(jl_eval_string("return jluna.exception_handler.get_last_message()")))
            );
            return;
        }
    }

    JuliaException::JuliaException(jl_value_t* exception, std::string stacktrace)
                : _value(exception), _message("[JULIA][EXCEPTION] " + stacktrace)
            {}

    const char* JuliaException::what() const noexcept
    {
        return _message.c_str();
    }

    AmbiguousCandidateException::AmbiguousCandidateException(const std::string& symbol_name, const std::vector<std::string>& candidate_modules)
    {
        std::stringstream str;
        str << "querying the julia state for value with symbol \":" << symbol_name << "\" returns more than one result. Please manually specify which module the value is defined in.\n";
        str << "candidates are:\n";

        for (auto s : candidate_modules)
            str << "\t" << s << "." << symbol_name << "\n";

        message = str.str();
    }

    const char * AmbiguousCandidateException::what() const noexcept
    {
        return message.c_str();
    }

    ImmutableVariableException::ImmutableVariableException(jl_value_t* value)
    {
        std::stringstream str;
        str << "trying to modify value of type " << jl_typeof_str(value) << " which is immutable" << std::endl;
        message = str.str();
    }

    const char * ImmutableVariableException::what() const noexcept
    {
        return message.c_str();
    }
}