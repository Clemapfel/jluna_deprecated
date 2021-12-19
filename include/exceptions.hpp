// 
// Copyright 2021 Clemens Cords
// Created on 19.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <exception>
#include <string>
#include <iostream>

namespace jlwrap
{
    /// @brief exception thrown in safe mode when a value is undefined
    class UndefinedBindingException : public std::exception
    {
        public:
            explicit UndefinedBindingException(std::string var_name, jl_module_t* module)
                : _message()
            {
                auto* name = jl_symbol_name_(module->name);
                std::stringstream str;
                str << "[JULIA] Symbol :" << var_name << " is undefined in module " << name << std::endl;
                _message = str.str();
            }

            explicit UndefinedBindingException(std::string var_name, std::string module)
                : _message()
            {
                std::stringstream str;
                str << "[JULIA] Symbol :" << var_name << " is undefined in module " << module << std::endl;
                _message = str.str();
            }

            const char* what() const noexcept override
            {
                return _message.c_str();
            }

        private:
            std::string _message;
    };

    /// @brief exception thrown in safe mode when a value is undefined
    class TypeError : public std::exception
    {
        public:
            explicit TypeError(std::string& in, std::string& expected, std::string& actual)
                : _message()
            {
                std::stringstream str;
                str << "[JULIA] TypeError: in " << in << ", expected " << expected << ", got a value of type " << actual << std::endl;
                _message = str.str();
            }

            const char* what() const noexcept override
            {
                return _message.c_str();
            }

        private:
            std::string _message;
    };


}