// 
// Copyright 2021 Clemens Cords
// Created on 15.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <string>
#include <deque>
#include <map>

// https://docs.julialang.org/en/v1/manual/calling-c-and-fortran-code/
// https://docs.julialang.org/en/v1/devdocs/init/

namespace jlwrap
{
    union State
    {
        friend class Proxy;

        public:
            /// @brief init environment
            /// @param path: path to image (/usr/bin/ by default on unix)
            static void initialize(std::string path = "/usr/bin/");

            /// @brief execute line of code
            static auto script(std::string);

            static void create_reference(jl_value_t*);
            static void free_reference(jl_value_t*);

        private:
            static inline jl_value_t* _reference_dict;
            static inline jl_function_t* _reference_dict_insert;
            static inline jl_function_t* _reference_dict_erase;
            static inline jl_datatype_t* _reference_wrapper;
            static inline std::map<jl_value_t*, size_t> _reference_counter = {};
    };
}

#include ".src/state.inl"

            /*
            template<typename T>
            static PrimitiveProxy register_variable(std::string name, T value);
            static PrimitiveProxy get_variable(std::string name);

            template<typename Function_t>
            static PrimitiveProxy register_function(std::string name, Function_t function);

            static void collect_garbage();

        private:
            template<typename... T>
            static jl_value_t* execute(T... strings);

            // garbage collection and allocation management
            static void allocate(jl_value_t*);
            static void schedule_free(jl_value_t*);

            static inline std::map<jl_value_t*, int> _n_schedules = {};

            static inline jl_value_t* _no_gc_dict_name = nullptr;
            static inline jl_function_t* _no_gc_dict_insert = nullptr;
            static inline jl_function_t* _no_gc_dict_erase = nullptr;
            static inline jl_datatype_t* _no_gc_wrapper = nullptr;

            // cleanup on exit
            static inline bool _initialized = false;
            static void at_exit()
            {
                if (_initialized)
                {
                    jl_eval_string(R"(
                    for (key, val) in _dict
                               delete!(_dict, key)
                           end
                    )");

                    jl_atexit_hook(0);
                }
            }
    };
}

#include ".src/state.inl"
             */