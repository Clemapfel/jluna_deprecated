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
        assert(value != nullptr);

        if (_value != nullptr)
            State_t::create_reference(value);
    }

    template<typename State_t>
    jl_value_t * Proxy<State_t>::data()
    {
        return _value;
    }

    template<typename State_t>
    Proxy<State_t>::~Proxy()
    {
        if (_value != nullptr)
            State_t::free_reference(_value);}

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