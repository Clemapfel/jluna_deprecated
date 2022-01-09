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

        protected:
            jl_value_t* _value;
            std::string _message;
    };

    extern bool expression_occurred();

    /// @brief if julia exception occurred, forward it to C++
    extern void forward_last_exception();

    /// @brief exception raised if result of querying for a variable returned multiple ambiguous results
    class AmbiguousCandidateException : public std::exception
    {
        public:
            /// @brief ctor
            AmbiguousCandidateException(const std::string& symbol_name, const std::vector<std::string>& candidates);

            /// @brief get description
            /// @returns c-string
            virtual const char* what() const noexcept override final;

        private:
            std::string message;
    };

    /// @brief exception raised if result of querying for a variable returned multiple ambiguous results
    class ImmutableVariableException : public std::exception
    {
        public:
            /// @brief ctor
            /// @param symbol_name: name of the symbol for which multiple results exist
            ImmutableVariableException(jl_value_t* variable);

            /// @brief get description
            /// @returns c-string
            virtual const char* what() const noexcept override final;

        private:
            std::string message;
    };

    /// @brief exception raised if trying to declare a proxy holding a variable that does not have a name associated with it mutating
    class UnnamedVariableException : public std::exception
    {
        public:
            /// @brief ctor
            UnnamedVariableException(jl_value_t* value);

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