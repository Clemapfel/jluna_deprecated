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
#include <global_utilities.hpp>
#include <include.jl.hpp>

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

    void State::initialize(const std::string& path)
    {
        if (path.empty())
            jl_init();
        else
            jl_init_with_image(path.c_str(), NULL);

        c_adapter::initialize();

        jl_eval_string(detail::include);
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
        forward_last_exception();

        std::atexit(&detail::on_exit);

        _jluna_module = (jl_module_t*) jl_eval_string("return jluna");

        jl_module_t* module = (jl_module_t*) jl_eval_string("return jluna.memory_handler");
        _create_reference = jl_get_function(module, "create_reference");
        _free_reference =  jl_get_function(module, "free_reference");
        _force_free =  jl_get_function(module, "force_free");
        _get_reference =  jl_get_function(module, "get_reference");

        jluna::Main = Proxy<State>((jl_value_t*) jl_main_module, nullptr);
        jluna::Base = Main["Base"];//Proxy<State>((jl_value_t*) jl_base_module, Main._content, jl_symbol("Base"));
        jluna::Core = Main["Core"];//Proxy<State>((jl_value_t*) jl_core_module, Main._content, jl_symbol("Core"));
    }

    void State::shutdown()
    {
        if (not jl_is_initialized())
            return;

        _jluna_module = nullptr;
        _create_reference = nullptr;
        _free_reference = nullptr;
        _force_free = nullptr;
        _get_value = nullptr;
        _get_reference = nullptr;

        detail::on_exit();
    }

    auto State::script(const std::string& command) noexcept
    {
        THROW_IF_UNINITIALIZED;

        std::stringstream str;
        str << "jluna.exception_handler.unsafe_call(quote " << command << " end)" << std::endl;
        return Proxy<State>(jl_eval_string(str.str().c_str()), nullptr);
    }

    auto State::script(const std::string& command, const std::string& module) noexcept
    {
        THROW_IF_UNINITIALIZED;

        std::stringstream str;
        str << "jluna.exception_handler.unsafe_call(quote " << command << " end, " << module << ")" << std::endl;
        return Proxy<State>(jl_eval_string(str.str().c_str()), nullptr);
    }

    auto State::safe_script(const std::string& command)
    {
        THROW_IF_UNINITIALIZED;

        std::stringstream str;
        str << "jluna.exception_handler.safe_call(quote " << command << " end)" << std::endl;
        auto* result = jl_eval_string(str.str().c_str());
        if (jl_exception_occurred() or exception_occurred())
        {
            std::cerr << "exception in jluna::State::safe_script for expression:\n\"" << command << "\"\n" << std::endl;
            forward_last_exception();
        }
        return Proxy<State>(result, nullptr);
    }

    auto State::safe_script(const std::string& command, const std::string& module)
    {
        THROW_IF_UNINITIALIZED;

        std::stringstream str;
        str << "jluna.exception_handler.safe_call(quote " << command << " end, " << module << ")" << std::endl;
        auto* result = jl_eval_string(str.str().c_str());
        if (jl_exception_occurred() or exception_occurred())
        {
            std::cerr << "exception in jluna::State::safe_script for expression:\n\"" << command << "\"\n" << std::endl;
            forward_last_exception();
        }
        return Proxy<State>(result, nullptr);
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

    void State::collect_garbage()
    {
        THROW_IF_UNINITIALIZED;

        static jl_function_t* gc = jl_get_function((jl_module_t*) jl_eval_string("return Base.GC"), "gc");

        bool before = jl_gc_is_enabled();

        jl_gc_enable(true);
        jl_call0(gc);
        jl_gc_enable(before);
    }

    void State::set_garbage_collector_enabled(bool b)
    {
        THROW_IF_UNINITIALIZED;

        jl_gc_enable(b);
    }

    size_t State::create_reference(jl_value_t* in)
    {
        THROW_IF_UNINITIALIZED;

        static jl_value_t* _refs = jl_eval_string("return jluna.memory_handler._refs[]");

        if (in == nullptr)// or in == _refs)
            return 0;

        //std::cout << "added " << in << " (" << jl_typeof_str(in) << ")" << std::endl;

        size_t res = -1;
        auto before = jl_gc_is_enabled();
        jl_gc_enable(false);
        jl_value_t* value;
        res = jl_unbox_uint64(safe_call(_create_reference, in));
        jl_gc_enable(before);

        return res;
    }

    jl_value_t * State::get_reference(size_t key)
    {
        return safe_call(_get_reference, jl_box_uint64(key));
    }

    void State::free_reference(size_t key)
    {
        THROW_IF_UNINITIALIZED;

        if (key == 0)
            return;

        auto before = jl_gc_is_enabled();
        jl_gc_enable(false);
        safe_call(_free_reference, jl_box_uint64(reinterpret_cast<size_t>(key)));
        jl_gc_enable(before);
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
