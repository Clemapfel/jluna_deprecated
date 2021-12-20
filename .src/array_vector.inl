// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#include <array_proxy.hpp>
#include <primitive_proxy.hpp>

namespace jlwrap
{
    template<typename T>
    bool Vector<T>::empty() const
    {
        return reinterpret_cast<jl_array_t*>(_value)->length == 0;
    }

    template<typename T>
    size_t Vector<T>::size() const
    {
        return reinterpret_cast<jl_array_t*>(_value)->length;
    }

    template<typename T>
    void Vector<T>::push_back(T value)
    {
        jl_value_t* v = value.data();
        jl_call(_push_back, _value, v);
    }

    template<typename T>
    void Vector<T>::push_front(T value)
    {
        jl_value_t* v = value.data();
        jl_call(_push_front, _value, v);
    }
}