// 
// Copyright 2021 Clemens Cords
// Created on 15.12.21 by clem (mail@clemens-cords.com)
//

#include <julia.h>
#include <state.hpp>

namespace jlwrap
{
    void State::initialize(std::string path)
    {
        jl_init_with_image(path.c_str(), NULL);
        _reference_dict = jl_eval_string("global __jlwrap_refs = IdDict()");
        _reference_dict_insert = jl_get_function(jl_base_module, "setindex!");
        _reference_dict_erase = jl_get_function(jl_base_module, "delete!");
        _reference_wrapper = reinterpret_cast<jl_datatype_t*>(jl_eval_string("Base.RefValue{Any}"));
    }

    auto State::script(std::string str)
    {
        return jl_eval_string(str.c_str());
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
