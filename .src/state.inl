// 
// Copyright 2021 Clemens Cords
// Created on 15.12.21 by clem (mail@clemens-cords.com)
//

#include <julia.h>
#include <state.hpp>
#include <sstream>
#include <exceptions.hpp>

namespace jlwrap
{
    static jl_value_t* _refs;
    static void on_exit()
    {
        jl_eval_string("jlwrap.memory_handler.force_free()");
        jl_atexit_hook(0);
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

        std::atexit(&on_exit);

        jl_module_t* module = (jl_module_t*) jl_eval_string("return jlwrap.memory_handler");
        _create_reference = jl_get_function(module, "create_reference");
        _free_reference =  jl_get_function(module, "free_reference");
        _force_free =  jl_get_function(module, "force_free");
        _get_value =  jl_get_function(module, "get_reference");
        _get_reference =  jl_get_function(module, "get_value");
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

        if (jl_unbox_bool(jl_eval_string("return jlwrap.exception_handler.has_exception_occurred()")))
        {
            throw JuliaException(
                    jl_eval_string("return jlwrap.exception_handler.get_last_exception()"),
                    std::string(jl_string_data(jl_eval_string("return jlwrap.exception_handler.get_last_message()")))
            );
        }

        return result;
    }

    jl_value_t* State::create_reference(jl_value_t* in)
    {
        JL_GC_PUSH1(in);
        jl_value_t* value = jl_call2(_create_reference, jl_box_uint64(reinterpret_cast<size_t>(in)), in);
        JL_GC_POP();
        return value;
    }

    void State::free_reference(jl_value_t* in)
    {
        JL_GC_PUSH1(in);
        jl_call1(_free_reference, jl_box_uint64(reinterpret_cast<size_t>(in)));
        JL_GC_POP();
    }
}
