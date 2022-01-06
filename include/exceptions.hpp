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

    /// @brief if julia exception occurred, forward it to C++
    extern void forward_last_exception();

    /// @brief exception raised if result of querying for a variable returned multiple ambiguous results
    class AmbiguousCandidateException : public std::exception
    {
        public:
            /// @brief ctor
            /// @param symbol_name: name of the symbol for which multiple results exist
            AmbiguousCandidateException(const std::string& symbol_name, const std::vector<std::string>& candidates);

            /// @brief get description
            /// @returns c-string
            virtual const char* what() const noexcept override final;

        private:
            std::string message;
    };

    /// @brief exception raised if a julia-side computation is requested before the state is initialized
    class UninitializedStateException : public std::exception
    {
        public:
            /// @brief ctor
            UninitializedStateException();

            /// @brief get description
            /// @returns c-string
            virtual const char* what() const noexcept override final;

        private:
            std::string message;
    };

    /// @brief throws an exception if State::initialize was not called yet
    extern void throw_if_uninitialized();
}

#include ".src/exceptions.inl"