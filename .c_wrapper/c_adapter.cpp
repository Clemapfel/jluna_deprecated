#include "c_adapter.hpp"

#ifdef __cplusplus

#include <iostream>
#include "./cppcall.jl.hpp"

extern "C"
{
    namespace jluna::c_adapter
    {
        bool initialize()
        {
            jl_init();
            _functions = {};

            jl_eval_string(cppcall_module);
        }

        void call_function(size_t id)
        {
            static jl_function_t* get_args = jl_get_function((jl_module_t*) jl_eval_string("return Main._cppcall"), "get_arguments");
            static jl_function_t* set_result = jl_get_function((jl_module_t*) jl_eval_string("return Main._cppcall"), "set_result");
            jl_value_t* tuple = jl_call0(get_args);
            jl_value_t* res;

            res = _functions.at(id)(tuple);

            if (res == nullptr) // catch returning nullptr
                res = jl_nothing;

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
        }

        bool is_registered(size_t id)
        {
            auto it = _functions.find(id);
            return it != _functions.end();
        }

        void throw_undefined_symbol(const char* str)
        {
            std::string msg = str;
            msg = "[JULIA][EXCEPTION] in julia-side cppcall: no function with symbol :" + msg + " registered";
            throw std::invalid_argument(msg);
        }
    }
}

#endif
