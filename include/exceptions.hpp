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
    /// @brief julia exception type
    enum JuliaExceptionType
    {
        // error thrown via jl_error() c-side
        ERROR_EXCEPTION = -1,

        // no exception placeholder
        NO_EXCEPTION = NULL,

        //
        ARGUMENT_ERROR ,
        BOUNDS_ERROR,
        COMPOSITE_EXCEPTION,
        DIMENSION_MISMATCH,
        DIVIDE_ERROR,
        DOMAIN_ERROR,
        EOF_ERROR,
        INEXACT_ERROR,
        INIT_ERROR,
        INTERRUPT_EXCEPTION,
        INVALID_STATE_EXCEPTION,
        KEY_ERROR,
        LOAD_ERROR,
        OUT_OF_MEMORY_ERROR,
        READ_ONLY_MEMORY_ERROR,
        REMOTE_EXCEPTION,
        METHOD_ERROR,
        OVERFLOW_ERROR,
        META_PARSE_ERROR,
        SYSTEM_ERROR,
        TYPE_ERROR,
        UNDEF_REF_ERROR,
        UNDEF_VAR_ERROR,
        STRING_INDEX_ERROR
    };

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

            /// @brief get type
            /// @returns type
            JuliaExceptionType get_type();

        protected:
            jl_value_t* _value;
            std::string _message;
            JuliaExceptionType _type;
    };

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
}

#include ".src/exceptions.inl"