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
#include <.test/test.hpp>
#include <unordered_map>
#include <functional>

namespace jluna
{
    template<typename>
    class Proxy;

    /// @brief concept that describes types which can be directly cast to jl_value_t*
    template<typename T>
    concept Decayable = requires(T t)
    {
        {t.operator jl_value_t*() || std::is_same_v<T, jl_value_t*>};
    };

    /// @brief static interface to the julia state
    union State
    {
        template<typename>
        friend class Proxy;

        friend class Test;

        public:
            /// @brief ctor deleted, singleton static-only object
            State() = delete;

            /// @brief dtor deleted, singleton static-only object
            ~State() = delete;

            /// @brief init environment
            static void initialize(const std::string& = "");

            /// @brief exit julia environment and deallocate all variables
            static void shutdown();

            /// @brief execute line of code, evaluated in Main
            /// @param command
            /// @returns proxy to result, if any
            /// @exceptions if an error occurs julia-side it will be ignore and the result of the call will be undefined
            static auto script(const std::string&) noexcept;

            /// @brief execute line of code, evaluated in Main
            /// @param command
            /// @param module: name of module the command will be evaluated in
            /// @returns proxy to result, if any
            /// @exceptions if an error occurs julia-side it will be ignore and the result of the call will be undefined
            static auto script(const std::string& command, const std::string& module) noexcept;

            /// @brief execute line of code with exception handling
            /// @param command
            /// @returns proxy to result, if any
            /// @exceptions if an error occurs julia-side a JuliaException will be thrown
            static auto safe_script(const std::string&);

            /// @brief execute line of code with exception handling
            /// @param command
            /// @param module: name of module the command will be evaluated in
            /// @returns proxy to result, if any
            /// @exceptions if an error occurs julia-side a JuliaException will be thrown
            static auto safe_script(const std::string& command, const std::string& module);

            /// @brief trigger the garbage collector
            static void collect_garbage();

            /// @brief activate/deactivate garbage collector
            static void set_garbage_collector_enabled(bool);

            template<typename Lambda_t>
            static void register_function(const std::string& name, Lambda_t&& lambda);

            /// @brief TODO
            static void call_function(size_t hash);

        protected:
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

            /// @overload safe_call for non-unboxable values that can still be cast to jl_value_t*
            template<Decayable... Args_t>
            static auto safe_call(jl_function_t*, Args_t&&...);

            /// @brief access a function just by name, searches for it in any module currently loaded
            /// @param function_name: exact function name, e.g. "push!"
            static jl_function_t* find_function(const std::string& function_name);

            /// @brief add a value to be safeguarded from the garbage collector
            /// @param pointer to value
            /// @note point is used as indexing, therefore it should never be reassigned or a dangling "reference" will be produced
            static size_t create_reference(jl_value_t*);

            /// @brief remove a value from the safeguard, after the call the garbage collector is free to collect it at any point
            /// @param pointer to value
            static void free_reference(size_t);

            /// @brief access reference for protected value
            static jl_value_t* get_reference(size_t);

        private:
            static inline jl_module_t* _jluna_module = nullptr;

            // memory handler interface
            static inline jl_function_t* _create_reference = nullptr;
            static inline jl_function_t* _free_reference = nullptr;
            static inline jl_function_t* _force_free = nullptr;
            static inline jl_function_t* _get_value = nullptr;
            static inline jl_function_t* _get_reference = nullptr;

            // cppcall interface
            static inline jl_function_t* _hash = nullptr;
            std::unordered_map<size_t, std::function<jl_value_t*()>> _functions;
    };
}

#include ".src/state.inl"