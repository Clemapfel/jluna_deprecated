// 
// Copyright 2021 Clemens Cords
// Created on 15.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <string>
#include <deque>
#include <map>
#include <set>

#include <primitive_proxy.hpp>
#include <function_proxy.hpp>

// https://docs.julialang.org/en/v1/manual/calling-c-and-fortran-code/
// https://docs.julialang.org/en/v1/devdocs/init/

namespace jlwrap
{
    template<typename>
    class Proxy;

    union State
    {
        public:
            State();
            ~State();

            /// @brief init environment
            static void initialize();

            /// @brief execute line of code
            /// @exceptions if an error occurs julia-side it will be ignore and the result of the call will be undefined
            static auto script(const std::string&) noexcept;

            /// @brief execute line of code with exception handling
            /// @exceptions if an error occurs julia-side a JuliaException will be thrown
            static auto safe_script(const std::string&);

            /// @brief call julia function without exception forwarding
            /// @param function
            /// @param arguments
            /// @returns function result as jl_value_t*
            template<typename... Args_t>
            static auto call(jl_function_t*, Args_t...);

            /// @brief call julia function with exception forwarding
            /// @param function
            /// @param arguments
            /// @returns function result as jl_value_t*
            template<typename... Args_t>
            static auto safe_call(jl_function_t*, Args_t...);

            /// @brief add a value to be safeguarded from the garbage collector
            /// @param pointer to value
            /// @note point is used as indexing, therefore it should never be reassigned or a dangling "reference" will be produced
            static jl_value_t* create_reference(jl_value_t*);

            /// @brief remove a value from the safeguard, after the call the garbage collector is free to collect it at any point
            /// @param pointer to value
            static void free_reference(jl_value_t*);

            static jlwrap::Function get_function(const std::string& function_name);
            static jlwrap::Function get_function(const std::string& function_name, const std::string& module_name);

        //protected:
            template<typename T>
            static T* get(std::string&) noexcept;

            static bool is_defined(std::string var_name, std::string module_name);
            static void throw_if_undefined(std::string& var_name, std::string& module_name);

        private:
            static void forward_last_exception();

            static inline jl_module_t* _jlwrap_module = nullptr;

            // memory handler interface
            static inline jl_function_t* _create_reference = nullptr;
            static inline jl_function_t* _free_reference = nullptr;
            static inline jl_function_t* _force_free = nullptr;
            static inline jl_function_t* _get_value = nullptr;
            static inline jl_function_t* _get_reference = nullptr;
    };
}

#include ".src/state.inl"