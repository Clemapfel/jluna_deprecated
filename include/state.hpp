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
    class Primitive;

    union State
    {
        public:
            State() = delete;

            /// @brief init environment
            /// @param path: path to image (/usr/bin/ by default on unix)
            static void initialize(std::string path = "/usr/bin/");

            /// @brief execute line of code
            static auto script(std::string);

            /// @brief add a value to be safeguarded from the garbage collector
            /// @param pointer to value
            /// @note point is used as indexing, therefore it should never be reassigned or a dangling "reference" will be produced
            static void create_reference(jl_value_t*);

            /// @brief remove a value from the safeguard, after the call the garbage collector is free to collect it at any point
            /// @param pointer to value
            static void free_reference(jl_value_t*);

            [[nodiscard]] static Primitive get_primitive(std::string&, std::string& module_name);

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