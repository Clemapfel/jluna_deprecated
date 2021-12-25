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
    void State::initialize()
    {
        jl_init();
        jl_eval_string(R"(include("/home/clem/Workspace/jlwrap/.src/common.jl")");
        _reference_dict = jl_eval_string("global __jlwrap_refs = IdDict()");
        _reference_dict_insert = jl_get_function(jl_base_module, "setindex!");
        _reference_dict_erase = jl_get_function(jl_base_module, "delete!");
        _reference_wrapper = reinterpret_cast<jl_datatype_t*>(jl_eval_string("Base.RefValue{Any}"));
    }

    auto State::script(std::string str)
    {
        return jl_eval_string(str.c_str());
    }

    auto State::safe_script(std::string command)
    {
        std::stringstream str;

        str << "try\n" << command << "\n";
        str << R"(
            jlwrap.ExceptionHandler.update()
        catch exception
            jlwrap.ExceptionHandler.update(exception)
        end)";

        auto* result = jl_eval_string(str.str().c_str());
        return result;
    }

    template<typename... T>
    jl_value_t* State::execute(T... args)
    {
        std::stringstream str;
        (str << ... << args);
        return jl_eval_string(str.str().c_str());
    }

    Primitive State::get_primitive(std::string var_name, std::string module_name)
    {
        throw_if_undefined(var_name, module_name);

        auto* res = execute("return ", var_name);
        return Primitive(res);
    }

    template<typename T>
    Primitive State::wrap_primitive(T v)
    {
        static_assert(
            std::is_same_v<T, bool> or
            std::is_same_v<T, char> or
            std::is_same_v<T, float> or
            std::is_same_v<T, double> or
            std::is_same_v<T, uint8_t> or
            std::is_same_v<T, uint16_t> or
            std::is_same_v<T, uint32_t> or
            std::is_same_v<T, uint64_t> or
            std::is_same_v<T, int8_t> or
            std::is_same_v<T, int16_t> or
            std::is_same_v<T, int32_t> or
            std::is_same_v<T, int64_t> or
            std::is_same_v<T, nullptr_t>,
            "Only fundamental types are available to be wrapped in jlwrap::Primitive"
        );

        if (std::is_same_v<T, bool>)
            return Primitive(jl_box_bool(v));
        else if (std::is_same_v<T, char>)
            return Primitive(jl_box_char(v));
        else if (std::is_same_v<T, float>)
            return Primitive(jl_box_float32(v));
        else if (std::is_same_v<T, double>)
            return Primitive(jl_box_float64(v));
        else if (std::is_same_v<T, uint8_t>)
            return Primitive(jl_box_uint8(v));
        else if (std::is_same_v<T, uint16_t>)
            return Primitive(jl_box_uint16(v));
        else if (std::is_same_v<T, uint32_t>)
            return Primitive(jl_box_uint32(v));
        else if (std::is_same_v<T, uint64_t>)
            return Primitive(jl_box_uint64(v));
        else if (std::is_same_v<T, int8_t>)
            return Primitive(jl_box_int8(v));
        else if (std::is_same_v<T, int16_t>)
            return Primitive(jl_box_int16(v));
        else if (std::is_same_v<T, int32_t>)
            return Primitive(jl_box_int32(v));
        else if (std::is_same_v<T, int64_t>)
            return Primitive(jl_box_int64(v));
        else if (std::is_same_v<T, nullptr_t>)
            return Primitive(jl_box_voidpointer((void*) NULL));
    }

    template<typename T>
    T* State::get(std::string& var_name) noexcept
    {
        return reinterpret_cast<T*>(execute("return ", var_name));
    }

    bool State::is_defined(std::string var_name, std::string module_name)
    {
        auto* res = execute("isdefined(", module_name, ",", var_name, ")");
        return jl_unbox_bool(res);
    }

    void State::throw_if_undefined(std::string& var_name, std::string& module_name)
    {
        if (not is_defined(module_name, "Main"))
            throw UndefVarException("module_name", "Main");

        if (not is_defined(var_name, module_name))
            throw UndefVarException(var_name, module_name);
    }

    void State::create_reference(jl_value_t* in)
    {
        if (_reference_counter.find(in) == _reference_counter.end())
            _reference_counter.insert({in, 1});
        else
            _reference_counter.at(in) += 1;

        jl_value_t* wrapped = jl_new_struct(_reference_wrapper, in);

        JL_GC_PUSH1(wrapped)
        jl_call3(_reference_dict_insert, _reference_dict, wrapped, in);
        JL_GC_POP();
    }

    void State::free_reference(jl_value_t* in)
    {
        assert(_reference_counter.find(in) != _reference_counter.end());
        _reference_counter.at(in) -= 1;

        if (_reference_counter.at(in) == 0)
        {
            _reference_counter.erase(in);
            jl_call2(_reference_dict_erase, _reference_dict, in);
        }
    }
}
