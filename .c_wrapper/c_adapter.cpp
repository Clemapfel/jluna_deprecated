#include "c_adapter.hpp"

#ifdef __cplusplus

#include <iostream>
#include "./cppcall.jl.hpp"

extern "C"
{
    namespace c_adapter
    {
        void initialize()
        {
            if (not jl_is_initialized())
                jl_init();

            _functions = {};

            jl_eval_string(cppcall_module);
            std::cout << "[C][LOG] c_adapter initialized" << std::endl;

            register_function("test_function", [](jl_value_t* tuple_in) -> jl_value_t* {

                static jl_function_t* println = jl_get_function(jl_base_module, "println");
                jl_call1(println, tuple_in);

                static jl_function_t* string = jl_get_function(jl_base_module, "string");
                return jl_call1(string, (jl_value_t*) jl_symbol("success"));
            });
        }

        void call_function(size_t id)
        {
            static jl_function_t* get_args = jl_get_function((jl_module_t*) jl_eval_string("return Main._cppcall"), "get_arguments");
            static jl_function_t* set_result = jl_get_function((jl_module_t*) jl_eval_string("return Main._cppcall"), "set_result");
            jl_value_t* tuple = jl_call0(get_args);

            jl_value_t* res = _functions.at(id)(tuple);
            jl_call1(set_result, res);
        }

        size_t hash(const std::string& str)
        {
            static jl_function_t* hash = jl_get_function(jl_base_module, "hash");
            return jl_unbox_uint64(jl_call1(hash, (jl_value_t*) jl_symbol(str.data())));
        }

        void register_function(const std::string& name, std::function<jl_value_t*(jl_value_t*)>&& lambda)
        {
            _functions.insert({hash(name), lambda});
            std::cout << "[C][LOG] " << name << " registered" << std::endl;
        }
    }
}

#endif

// g++ -fpic -shared cpplibrary.cpp -o -libcpplibrary
