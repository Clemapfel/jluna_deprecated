// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#include <proxy.hpp>
#include <state.hpp>

namespace jlwrap
{
    template<typename T>
    Proxy<T>::Proxy(jl_sym_t* symbol, T* value, jl_datatype_t* type)
        : _symbol(symbol), _value(value), _type(type)
    {
        assert(value != nullptr);

        if (_value != nullptr)
            State::create_reference(value);

        if (_symbol != nullptr)
            State::create_reference(reinterpret_cast<jl_value_t*>(symbol));

        if (_type != nullptr)
            State::create_reference(reinterpret_cast<jl_value_t*>(type));
    }

    template<typename T>
    Proxy<T>::~Proxy()
    {
        if (_value != nullptr)
            State::free_reference(_value);

        if (_symbol != nullptr)
            State::free_reference(reinterpret_cast<jl_value_t*>(_symbol));

        if (_type != nullptr)
            State::free_reference(reinterpret_cast<jl_value_t*>(_type));
    }

    template<typename T>
    Proxy<T>::Proxy(Proxy&& other) noexcept
        : _value(other._value),
          _symbol(other._symbol),
          _type(other._type)
    {
        other._value = nullptr;
        other._symbol = nullptr;
        other._type = nullptr;
    }

    template<typename T>
    Proxy<T>::Proxy(const Proxy& other)
        : _value(other._value),
          _symbol(other._symbol),
          _type(other._type)
    {
        State::create_reference(_value);
        State::create_reference(reinterpret_cast<jl_value_t*>(_symbol));
        State::create_reference(reinterpret_cast<jl_value_t*>(_type));
    }

    template<typename T>
    Proxy<T>& Proxy<T>::operator=(Proxy&& other) noexcept
    {
        if (&other == this)
            return *this;

        _value = other._value;
        _type = other._type;
        _symbol = other._symbol;

        other._value = nullptr;
        other._type = nullptr;
        other._symbol = nullptr;

        return *this;
    }

    template<typename T>
    Proxy<T>& Proxy<T>::operator=(const Proxy& other)
    {
        if (&other == this)
            return *this;

        _value = other._value;
        _type = other._type;
        _symbol = other._symbol;

        State::create_reference(_value);
        State::create_reference(reinterpret_cast<jl_value_t*>(_type));
        State::create_reference(reinterpret_cast<jl_value_t*>(_symbol));

        return *this;
    }
}