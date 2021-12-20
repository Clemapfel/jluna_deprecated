// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#include <sstream>

#include <array_proxy.hpp>
#include <exceptions.hpp>
#include <proxy.hpp>

namespace jlwrap
{
    template<typename T, size_t R>
    Array<T, R>::Array(jl_value_t* value)
        : Proxy<State>(reinterpret_cast<jl_value_t*>(value))
    {
        if (not jl_typeis(value, jl_array_t))
        {
            std::stringstream str;
            str << jl_typeof(value) << std::endl;
            throw TypeException("Array<T, R>(jl_value_t*)", "jl_array_t", "str.str()");
        }

        _value = (jl_array_t*) Proxy<State>::operator _jl_value_t *();
    }

    template<typename T, size_t R>
    Array<T, R>::Array(jl_array_t* value)
    {}

    template<typename T, size_t R>
    Array<T, R>::operator jl_array_t*()
    {
        return _value;
    }

    template<typename T, size_t R>
    auto Array<T, R>::at(size_t i)
    {
        if (reinterpret_cast<jl_array_t*>(_value)->length >= i)
        {
            std::stringstream str;
            str << "In Array<T, R>::at(size_t i): index out of range for an array of size " << reinterpret_cast<jl_array_t*>(_value)->length << std::endl;
            throw std::out_of_range(str.str());
        }

        return Iterator(_value, i);
    }

    template<typename T, size_t R>
    const auto Array<T, R>::at(size_t i) const
    {
        if (reinterpret_cast<jl_array_t*>(_value)->length >= i)
        {
            std::stringstream str;
            str << "In Array<T, R>::at(size_t i): index out of range for an array of size " << reinterpret_cast<jl_array_t*>(_value)->length << std::endl;
            throw std::out_of_range(str.str());
        }

        return ConstIterator(_value, i);
    }

    template<typename T, size_t R>
    auto Array<T, R>::operator[](size_t i)
    {
        return Iterator(_value, i);
    }

    template<typename T, size_t R>
    const auto Array<T, R>::operator[](size_t i) const
    {
        return ConstIterator(_value, i);
    }

    template<typename T, size_t R>
    auto Array<T, R>::begin()
    {
        return Iterator(_value, 0);
    }

    template<typename T, size_t R>
    auto Array<T, R>::begin() const
    {
        return ConstIterator(_value, 0);
    }

    template<typename T, size_t R>
    auto Array<T, R>::end()
    {
        return Iterator(_value, reinterpret_cast<jl_array_t*>(_value)->length);
    }

    template<typename T, size_t R>
    auto Array<T, R>::end() const
    {
        return ConstIterator(_value, reinterpret_cast<jl_array_t*>(_value)->length);
    }

}