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

namespace jlwrap
{
    template<typename>
    class Proxy;

    template<typename T>
    concept Decayable = requires(T t)
    {
        {t.operator jl_value_t*() || std::is_same_v<T, jl_value_t*>};
    };

    union State
    {
        template<typename>
        friend class Proxy;

        public:
            /// @brief ctor deleted, singleton static-only object
            State() = delete;

            /// @brief dtor deleted, singleton static-only object
            ~State() = delete;

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
            static auto call(jl_function_t*, Args_t&&...);

            /// @brief call julia function with exception forwarding
            /// @param function
            /// @param arguments
            /// @returns function result as jl_value_t*
            template<typename... Args_t>
            static auto safe_call(jl_function_t*, Args_t&&...);

            template<Decayable... Args_t>
            static auto safe_call(jl_function_t*, Args_t&&...);

            /// @brief access a function in a specific module
            /// @param function_name: exact function name, e.g. "push!"
            /// @param module_name: name of module including submodule, e.g. "Base.InteractiveUtils"
            static jl_function_t* get_function(const std::string& function_name, const std::string& module_name);

            /// @brief access a function just by name, searches for it in any module currently loaded
            /// @param function_name: exact function name, e.g. "push!"
            static jl_function_t* find_function(const std::string& function_name);

        //protected:
            /// @brief add a value to be safeguarded from the garbage collector
            /// @param pointer to value
            /// @note point is used as indexing, therefore it should never be reassigned or a dangling "reference" will be produced
            static jl_value_t* create_reference(jl_value_t*);

            /// @brief remove a value from the safeguard, after the call the garbage collector is free to collect it at any point
            /// @param pointer to value
            static void free_reference(jl_value_t*);

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