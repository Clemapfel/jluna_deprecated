// 
// Copyright 2021 Clemens Cords
// Created on 15.12.21 by clem (mail@clemens-cords.com)
//

#include <julia.h>
#include <state.hpp>
#include <sstream>
#include <exceptions.hpp>
#include <box_any.hpp>
#include <symbol_proxy.hpp>

namespace jlwrap
{
    namespace detail
    {
        static void on_exit()
        {
            jl_eval_string("jlwrap.memory_handler.force_free()");
            jl_atexit_hook(0);
        }
    }

    void State::initialize()
    {
        jl_init_with_image("/home/clem/Applications/julia/bin", NULL);
        jl_eval_string("include(\"/home/clem/Workspace/jlwrap/include/include.jl\")");

        jl_eval_string(R"(
            if isdefined(Main, :jlwrap)
                print("[JULIA][LOG] ")
                Base.printstyled("initialization successfull.\n"; color = :green)
            else
                print("[JULIA]")
                Base.printstyled("[ERROR] initialization failed.\n"; color = :red)
                throw(AssertionError(("[JULIA][ERROR] initialization failed.")))
            end
        )");

        std::atexit(&detail::on_exit);

        _jlwrap_module = (jl_module_t*) jl_eval_string("return jlwrap");

        jl_module_t* module = (jl_module_t*) jl_eval_string("return jlwrap.memory_handler");
        _create_reference = jl_get_function(module, "create_reference");
        _free_reference =  jl_get_function(module, "free_reference");
        _force_free =  jl_get_function(module, "force_free");
        _get_value =  jl_get_function(module, "get_reference");
        _get_reference =  jl_get_function(module, "get_value");
    }

    void State::forward_last_exception()
    {
        if (jl_unbox_bool(jl_eval_string("return jlwrap.exception_handler.has_exception_occurred()")))
        {
            throw JuliaException(
                    jl_eval_string("return jlwrap.exception_handler.get_last_exception()"),
                    std::string(jl_string_data(jl_eval_string("return jlwrap.exception_handler.get_last_message()")))
            );
        }
    }

    auto State::script(const std::string& str) noexcept
    {
        return jl_eval_string(str.c_str());
    }

    auto State::safe_script(const std::string& command)
    {
        std::stringstream str;
        str << "jlwrap.exception_handler.safe_call(\"";

        // promote \" to \\\"
        for (char c : command)
        {
            if (c == '\\')
                str << "\\\\";
            else if (c == '\"')
                str << "\\\"";
            else
                str << c;
        }

        str << "\")" << std::endl;

        auto* result = jl_eval_string(str.str().c_str());
        forward_last_exception();

        return result;
    }

    template<typename... Args_t>
    auto State::call(jl_function_t* function, Args_t&&... args)
    {
        std::array<jl_value_t*, sizeof...(Args_t)> params;
        auto insert = [&](size_t i, jl_value_t* to_insert) {params.at(i) = to_insert;};

        {
            size_t i = 0;
            (insert(i++, box(std::forward<Args_t>(args))), ...);
        }

        return jl_call(function, params.data(), params.size());
    }

    template<typename... Args_t>
    auto State::safe_call(jl_function_t* function, Args_t&&... args)
    {
        std::array<jl_value_t*, sizeof...(Args_t) + 1> params;
        auto insert = [&](size_t i, jl_value_t* to_insert) {params.at(i) = to_insert;};
        {
            params.at(0) = (jl_value_t*) function;
            size_t i = 1;
            (insert(i++, box(std::forward<Args_t>(args))), ...);
        }

        jl_module_t* module = (jl_module_t*) jl_eval_string("return jlwrap.exception_handler");
        jl_function_t* safe_call = jl_get_function(module, "safe_call");
        auto* result = jl_call(safe_call, params.data(), params.size());
        forward_last_exception();

        return result;
    }

    jl_value_t* State::create_reference(jl_value_t* in)
    {
        JL_GC_PUSH1(in);
        //std::cout << "added " << in << std::endl;
        jl_value_t* value = safe_call(_create_reference, jl_box_uint64(reinterpret_cast<size_t>(in)), in);
        assert(jl_exception_occurred() == nullptr);
        JL_GC_POP();
        return value;
    }

    void State::free_reference(jl_value_t* in)
    {
        JL_GC_PUSH1(in);
        //std::cout << "freed " << in << std::endl;
        safe_call(_free_reference, jl_box_uint64(reinterpret_cast<size_t>(in)));
        assert(jl_exception_occurred() == nullptr);
        JL_GC_POP();
    }

    jl_function_t* State::get_function(const std::string& function_name, const std::string& module_name)
    {
        jl_value_t* module_v = safe_script("return " + module_name);
        assert(jl_isa(module_v, (jl_value_t*) jl_module_type));

        static jl_function_t* get_function = jl_get_function(_jlwrap_module, "get_function");
        return safe_call(get_function, (jl_value_t*) jl_symbol(&function_name[0]), module_v);
    }

    jl_function_t* State::find_function(const std::string& function_name)
    {
        static jl_function_t* get_function = jl_get_function(_jlwrap_module, "find_function");
        jl_array_t* res = (jl_array_t*) safe_call(get_function, (jl_value_t*) jl_symbol(&function_name[0]));

        if (res->length == 0)
        {
            safe_script("throw(UndefVarError(:" + function_name + "))");
            return nullptr;
        }
        else if (not res->length == 1)
        {
            std::vector<std::string> candidate_modules;
            static jl_function_t* get_all_modules_defining = jl_get_function(_jlwrap_module, "get_all_modules_defining");
            auto* candidate_array = (jl_array_t*) safe_call(get_all_modules_defining, script("return Symbol(\"" + function_name + "\")"));

            static jl_function_t* to_string = jl_get_function(jl_base_module, "string");

            for (size_t i = 0; i < candidate_array->length; ++i)
            {
                jl_module_t* m = (jl_module_t*) jl_arrayref(candidate_array, i);
                candidate_modules.push_back(std::string(jl_string_data(safe_call(to_string, m))));
            }

            throw AmbiguousCandidateException(function_name, candidate_modules);
            return nullptr;
        }
        else
          return (jl_function_t*) jl_arrayref(res, 0);
    }
}
