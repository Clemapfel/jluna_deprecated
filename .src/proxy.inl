// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#include <proxy.hpp>

namespace jlwrap
{
    template<typename State_t>
    Proxy<State_t>::Proxy(jl_value_t* value)
        : _value(value)
    {
        if (_value != nullptr)
            State_t::create_reference(value);

        auto* symbol_vec = jl_field_names((jl_datatype_t*) jl_typeof(_value));

    }

    template<typename State_t>
    Proxy<State_t>::operator jl_value_t*()
    {
        return _value;
    }

    template<typename State_t>
    Proxy<State_t>::~Proxy()
    {
        if (_value != nullptr)
            State_t::free_reference(_value);
    }

    template<typename State_t>
    auto Proxy<State_t>::operator[](std::string field_name)
    {
    }

    template<typename State_t>
    Proxy<State_t>::Proxy(Proxy&& other) noexcept
        : _value(other._value)
    {
        other._value = nullptr;
    }

    template<typename State_t>
    Proxy<State_t>::Proxy(const Proxy& other)
        : _value(other._value)
    {
        State_t::create_reference(_value);
    }

    template<typename State_t>
    Proxy<State_t>& Proxy<State_t>::operator=(Proxy&& other) noexcept
    {
        if (&other == this)
            return *this;

        _value = other._value;
        other._value = nullptr;

        return *this;
    }

    template<typename State_t>
    Proxy<State_t>& Proxy<State_t>::operator=(const Proxy& other)
    {
        if (&other == this)
            return *this;

        _value = other._value;
        State_t::create_reference(_value);

        return *this;
    }
}