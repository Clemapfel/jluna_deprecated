// 
// Copyright 2021 Clemens Cords
// Created on 19.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <exception>
#include <string>
#include <iostream>
#include <vector>

namespace jlwrap
{
    /// @brief wrapper for julia exceptions
    class JuliaException : public std::exception
    {
        public:
            JuliaException() = default;

            JuliaException(jl_value_t* exception, std::string stacktrace)
                : _value(exception), _message(stacktrace)
            {}

            virtual const char* what() const noexcept override final
            {
                return _message.c_str();
            }

        protected:
            jl_value_t* _value;
            std::string _message;
    };

    /*

    /// @brief checks if a julia-side exception was thrown and if it was, propagates it into the corresponding C++ exception
    void check_for_exceptions()
    {
        auto* exception = jl_exception_occurred();
        if (exception == nullptr)
            return;

        static jl_function_t* sprint = jl_get_function(jl_base_module, "sprint");
        static jl_value_t* showerror = jl_eval_string("return Base.showerror");//jl_get_function(jl_base_module, "showerror");
        static jl_function_t* current_exceptions = jl_get_function(jl_base_module, "current_exceptions");
        static jl_function_t* backtrace = jl_get_function(jl_base_module, "backtrace");

        auto res = std::string(jl_string_data(jl_call3(
                sprint,
                showerror,
                jl_call0(current_exceptions),
                jl_call0(backtrace)
                )));

        std::cout << res << std::endl;

        //std::cout << std::string(jl_string_data(jl_eval_string("return sprint(Base.showerror, current_exceptions(), backtrace())"))) << std::endl;
    }

    /// @brief exception thrown in safe mode when a value is undefined
    class UndefVarException : public Exception
    {
        public:
            explicit UndefVarException(std::string var_name, jl_module_t* module)
                : Exception()
            {
                auto* name = jl_symbol_name_(module->name);
                std::stringstream str;
                str << "[JULIA] UndefVarError:" << var_name << " is undefined in module " << name << std::endl;
                _message = str.str();
            }

            explicit UndefVarException(std::string var_name, std::string module)
                : Exception()
            {
                std::stringstream str;
                str << "[JULIA] UndefVarError:" << var_name << " is undefined in module " << module << std::endl;
                _message = str.str();
            }
    };

    /// @brief exception thrown in safe mode type does not match expected type
    class TypeException : public Exception
    {
        public:
            explicit TypeException(std::string in, std::string expected, std::string actual)
                : Exception()
            {
                std::stringstream str;
                str << "[JULIA] TypeError: in " << in << ", expected " << expected << ", got a value of type " << actual << std::endl;
                _message = str.str();
            }
    };

    /// @brief exception thrown in safe mode if there is no method of a function for the given argument types
    class MethodException : public Exception
    {
        public:
            explicit MethodException(std::string function_name, std::vector<std::string> types)
                : Exception()
            {
                std::stringstream str;
                str << "[JULIA] MethodError: no method matching " << function_name << "(";

                for (size_t i = 0; i < types.size(); ++i)
                    str << "::" << types.at(i) << (i == types.size() - 1 ? "" : ",");

                str << ")" << std::endl;
                _message = str.str();
            }
    };
     */
}