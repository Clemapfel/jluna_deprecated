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

            res = _functions.at(id).first(tuple);

            if (res == nullptr) // catch returning nullptr
                res = jl_nothing;

            jl_call1(set_result, res);
        }

        size_t hash(const std::string& str)
        {
            static jl_function_t* hash = jl_get_function(jl_base_module, "hash");
            return jl_unbox_uint64(jl_call1(hash, (jl_value_t*) jl_symbol(str.data())));
        }

        void register_function(const std::string& name, size_t n_args, std::function<jl_value_t*(jl_value_t*)>&& lambda)
        {
            [[unlikely]]
            if (name.find('.') != std::string::npos or name.at(0) == '#')
            {
                std::string str = "In register_function(\"" + name + "\"): function names cannot begin with \'#\' or contain \'.\' in any place";
                throw std::invalid_argument(str.c_str());
            }

            _functions.insert({hash(name), std::make_pair(lambda, n_args)});
        }

        void unregister_function(const std::string& name)
        {
            _functions.erase(hash(name));
        }

        bool is_registered(size_t id)
        {
            auto it = _functions.find(id);
            return it != _functions.end();
        }

        size_t get_n_args(size_t id)
        {
            return _functions.at(id).second;
        }
    }
}

#endif
