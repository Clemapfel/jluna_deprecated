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

namespace jluna
{
    namespace detail
    {
        static void on_exit()
        {
            jl_eval_string(R"([JULIA][LOG] Shutting down...)");
            jl_eval_string("jluna.memory_handler.force_free()");
            jl_atexit_hook(0);
        }
    }

    void State::initialize()
    {
        jl_init(); //_with_image("/home/clem/Applications/julia/bin", NULL);
        jl_eval_string("include(\"/home/clem/Workspace/jluna/include/include.jl\")");
        forward_last_exception();

        jl_eval_string(R"(
            if isdefined(Main, :jluna)
                print("[JULIA][LOG] ")
                Base.printstyled("initialization successfull.\n"; color = :green)
            else
                print("[JULIA]")
                Base.printstyled("[ERROR] initialization failed.\n"; color = :red)
                throw(AssertionError(("[JULIA][ERROR] initialization failed.")))
            end
        )");

        std::atexit(&detail::on_exit);

        _jluna_module = (jl_module_t*) jl_eval_string("return jluna");

        jl_module_t* module = (jl_module_t*) jl_eval_string("return jluna.memory_handler");
        _create_reference = jl_get_function(module, "create_reference");
        _free_reference =  jl_get_function(module, "free_reference");
        _force_free =  jl_get_function(module, "force_free");
        _get_value =  jl_get_function(module, "get_reference");
        _get_reference =  jl_get_function(module, "get_value");
    }

    void State::shutdown()
    {
        THROW_IF_UNINITIALIZED;

        detail::on_exit();

        _jluna_module = nullptr;
        _create_reference = nullptr;
        _free_reference = nullptr;
        _force_free = nullptr;
        _get_value = nullptr;
        _get_reference = nullptr;
    }

    auto State::script(const std::string& str) noexcept
    {
        THROW_IF_UNINITIALIZED;

        return Proxy<State>(jl_eval_string(str.c_str()));
    }

    auto State::safe_script(const std::string& command)
    {
        THROW_IF_UNINITIALIZED;

        std::stringstream str;
        str << "jluna.exception_handler.safe_call(@unquote(quote " << command << " end))";
        auto* result = jl_eval_string(str.str().c_str());
        if (jl_exception_occurred() or exception_occurred()) // catch parse errors in command
        {
            std::cerr << "exception in jluna::State::safe_script for expression:\n\"" << command << "\"\n" << std::endl;
            forward_last_exception();
        }
        return Proxy<State>(result);
    }

    template<typename... Args_t>
    auto State::call(jl_function_t* function, Args_t&&... args)
    {
        THROW_IF_UNINITIALIZED;

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
        THROW_IF_UNINITIALIZED;

        static jl_function_t* tostring = jl_get_function(jl_base_module, "string");
        std::array<jl_value_t*, sizeof...(Args_t) + 1> params;
        auto insert = [&](size_t i, jl_value_t* to_insert) {params.at(i) = to_insert;};

        {
            params.at(0) = (jl_value_t*) function;
            size_t i = 1;
            (insert(i++, box(std::forward<Args_t>(args))), ...);
        }

        static jl_module_t* module = (jl_module_t*) jl_eval_string("return jluna.exception_handler");
        static jl_function_t* safe_call = jl_get_function(module, "safe_call");
        auto* result = jl_call(safe_call, params.data(), params.size());

        forward_last_exception();
        return result;
    }

    template<Decayable... Args_t>
    auto State::safe_call(jl_function_t* function, Args_t&&... args)
    {
        THROW_IF_UNINITIALIZED;

        std::array<jl_value_t*, sizeof...(Args_t) + 1> params;
        auto insert = [&](size_t i, jl_value_t* to_insert) {params.at(i) = to_insert;};

        {
            params.at(0) = (jl_value_t*) function;
            size_t i = 1;
            (insert(i++, (jl_value_t*) args), ...);
        }

        static jl_module_t* module = (jl_module_t*) jl_eval_string("return jluna.exception_handler");
        static jl_function_t* safe_call = jl_get_function(module, "safe_call");
        auto* result = jl_call(safe_call, params.data(), params.size());
        forward_last_exception();

        return result;
    }

    jl_value_t* State::create_reference(jl_value_t* in)
    {
        THROW_IF_UNINITIALIZED;

        if (in == nullptr)
            return nullptr;

        //std::cout << "added " << in << " (" << jl_typeof_str(in) << ")" << std::endl;

        auto before = jl_gc_is_enabled();
        jl_gc_enable(false);
        jl_value_t* value;
        try
        {
            value = safe_call(_create_reference, reinterpret_cast<size_t>(in), in);
        }
        catch (jluna::JuliaException& exc)
        {
            std::cerr << "[C++][ERROR][FATAL] illegal allocation of value with pointer " << in << " (" << jl_typeof_str(in) << ").\n" << std::endl;
            std::cerr << "If this exception was triggered in an unmodified release version of jluna, please notify the developer.\n" << std::endl;
            std::cerr << exc.what() << std::endl;
            throw exc;
            exit(1);
        }
        jl_gc_enable(before);

        return value;
    }

    void State::free_reference(jl_value_t* in)
    {
        THROW_IF_UNINITIALIZED;

        if (in == nullptr)
            return;

        //std::cout << "freed " << in << " (" << jl_typeof_str(in) << ")" << std::endl;

        auto before = jl_gc_is_enabled();
        jl_gc_enable(false);
        try
        {
            safe_call(_free_reference, jl_box_uint64(reinterpret_cast<size_t>(in)));
        }
        catch (jluna::JuliaException& exc)
        {
            std::cerr << "[C++][ERROR][FATAL] illegal freeing of value with pointer " << in << " (" << jl_typeof_str(in) << ").\n" << std::endl;
            std::cerr << "If this exception was triggered in an unmodified release version of jluna, please notify the developer.\n" << std::endl;
            std::cerr << exc.what() << std::endl;
            throw exc;
            exit(1);
        }
        jl_gc_enable(before);
    }

    jl_function_t* State::get_function(const std::string& function_name, const std::string& module_name)
    {
        THROW_IF_UNINITIALIZED;

        jl_value_t* module_v = safe_script("return " + module_name);
        assert(jl_isa(module_v, (jl_value_t*) jl_module_type));

        static jl_function_t* get_function = jl_get_function(_jluna_module, "get_function");
        return safe_call(get_function, (jl_value_t*) jl_symbol(&function_name[0]), module_v);
    }

    jl_function_t* State::find_function(const std::string& function_name)
    {
        THROW_IF_UNINITIALIZED;

        static jl_function_t* get_function = jl_get_function(_jluna_module, "find_function");
        jl_array_t* res = (jl_array_t*) (jl_value_t*) safe_call(get_function, (jl_value_t*) jl_symbol(&function_name[0]));

        if (res->length == 0)
        {
            safe_script("throw(UndefVarError(:" + function_name + "))");
            return nullptr;
        }
        else if (not res->length == 1)
        {
            std::vector<std::string> candidate_modules;
            static jl_function_t* get_all_modules_defining = jl_get_function(_jluna_module, "get_all_modules_defining");
            auto* candidate_array = (jl_array_t*) safe_call(get_all_modules_defining, (jl_value_t*) script("return Symbol(\"" + function_name + "\")"));

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
