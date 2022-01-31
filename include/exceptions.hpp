// 
// Copyright 2021 Clemens Cords
// Created on 19.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <exception>
#include <string>
#include <iostream>
#include <vector>

namespace jluna
{
    /// @brief wrapper for julia exceptions
    class JuliaException : public std::exception
    {
        public:
            /// @brief default ctor
            JuliaException() = default;

            /// @brief ctor
            /// @param exception: value pointing to a julia-side instance of the exception
            /// @param stacktrace: string describing the exception and the stacktrace
            JuliaException(jl_value_t* exception, std::string stacktrace);

            /// @brief get description
            /// @returns c-string
            virtual const char* what() const noexcept override final;

            /// @brief decay to jl value
            /// @returns jl_value_t*
            operator jl_value_t*();

        protected:
            jl_value_t* _value;
            std::string _message;
    };

    /// @brief if julia exception occurred, forward it to C++
    extern void forward_last_exception();

    /// @brief exception raised when trying to mutate a proxy pointing to an immutable object
    class ImmutableVariableException : public std::exception
    {
        public:
            /// @brief ctor
            /// @param variable
            ImmutableVariableException(jl_value_t* variable);

            /// @brief get description
            /// @returns c-string
            virtual const char* what() const noexcept override final;

        private:
            std::string message;
    };

    /// @brief throws an exception if State::initialize was not called yet
    #define THROW_IF_UNINITIALIZED assert(jl_is_initialized() && "initiate the state via jluna::State::initialize() before trying to interact with julia or jluna")
}

#include ".src/exceptions.inl"