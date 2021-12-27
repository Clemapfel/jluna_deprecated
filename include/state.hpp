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


// https://docs.julialang.org/en/v1/manual/calling-c-and-fortran-code/
// https://docs.julialang.org/en/v1/devdocs/init/

namespace jlwrap
{
    template<typename>
    class Proxy;

    union State
    {
        public:
            State() = delete;

            /// @brief init environment
            static void initialize();

            /// @brief execute line of code
            /// @exceptions if an error occurs julia-side it will be ignore and the result of the call will be undefined
            static auto script(const std::string&) noexcept;

            /// @brief execute line of code with exception handling
            /// @exceptions if an error occurs julia-side a JuliaException will be thrown
            static auto safe_script(const std::string&);

            /// @brief execute line of code, may throw exception

            /// @brief add a value to be safeguarded from the garbage collector
            /// @param pointer to value
            /// @note point is used as indexing, therefore it should never be reassigned or a dangling "reference" will be produced
            static void create_reference(jl_value_t*);

            /// @brief remove a value from the safeguard, after the call the garbage collector is free to collect it at any point
            /// @param pointer to value
            static void free_reference(jl_value_t*);

            /*
            /// @brief access the value of a variable bound to a primitive
            /// @param var_name
            /// @param module_name
            /// @returns Primitive proxy holding ownership of the value
            /// @exceptions if no variable or module of the given name exists, a UndefVarException will be thrown
            [[nodiscard]] static Primitive get_primitive(std::string var_name, std::string module_name);

            /// @brief send a C++-side value to Julia
            /// @tparam: type of value, has to be primitive or a static assertion will trigger
            /// @param T: value
            /// @returns Primitive proxy holding ownership of the allocated value
            template<typename T>
            [[nodiscard]] static Primitive wrap_primitive(T);
             */

        //protected:
            template<typename T>
            static T* get(std::string&) noexcept;

            static bool is_defined(std::string var_name, std::string module_name);
            static void throw_if_undefined(std::string& var_name, std::string& module_name);

        private:
            template<typename... T>
            static jl_value_t* execute(T...);

            static std::string get_module_name(jl_module_t*);

            static inline jl_value_t* _reference_dict;
            static inline jl_function_t* _reference_dict_insert;
            static inline jl_function_t* _reference_dict_erase;
            static inline jl_datatype_t* _reference_wrapper;
            static inline std::map<jl_value_t*, size_t> _reference_counter = {};
    };
}

#include ".src/state.inl"