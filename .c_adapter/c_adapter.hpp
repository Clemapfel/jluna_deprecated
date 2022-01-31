// 
// Copyright 2022 Clemens Cords
// Created on 19.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <stddef.h>

#ifdef __cplusplus

#include <map>
#include <julia.h>
#include <functional>
#include <string>

extern "C"
{
    /// @brief c-compatible interface, only intended for internal use
    namespace jluna::c_adapter
    {
        /// @brief holds lambda registers via jluna
        static inline std::map<size_t, std::pair<std::function<jl_value_t*(jl_value_t*)>, size_t>> _functions = {};

        /// @brief hash lambda-side
        size_t hash(const std::string&);

        /// @brief add lambda to function register
        void register_function(const std::string& name, size_t n_args, std::function<jl_value_t*(jl_value_t*)>&&);

        /// @brief remove lambda from function register
        void unregister_function(const std::string& name);

        /// @brief call lambda by id
        void call_function(size_t);

        /// @brief get number of tuples allowed for function with id
        size_t get_n_args(size_t);

        /// @brief check if function is registered
        bool is_registered(size_t id);

        /// @brief free function
        void free_function(size_t);
    }
}

#else // exposed to juila as pure C header:

void initialize(const char*);
void call_function(size_t);
bool is_registered(size_t);
void throw_undefined_symbol(const char*);
size_t get_n_args(size_t);

#endif