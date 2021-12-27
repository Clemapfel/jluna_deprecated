// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#include <array_proxy.hpp>
#include <primitive_proxy.hpp>

namespace jlwrap
{
    template<typename T>
    Vector<T>::Vector()
    {
       if (_push_front == nullptr)
           _push_front = jl_get_function(jl_main_module, "pushfirst!");

       if (_push_back== nullptr)
           _push_back = jl_get_function(jl_main_module, "append!");

       if (_insert == nullptr)
           _insert = jl_get_function(jl_main_module, "insert!");

       if (_erase == nullptr)
           _erase = jl_get_function(jl_main_module, "deleteat!");

       if (_replace == nullptr)
           _replace = jl_get_function(jl_main_module, "setindex!");
    }

    template<typename T>
    bool Vector<T>::empty() const
    {
        return Array<T, 1>::_value->length == 0;
    }

    template<typename T>
    void Vector<T>::push_back(T to_push)
    {
        auto* v = (jl_value_t*) to_push;
        jl_call2(_push_back, reinterpret_cast<jl_value_t*>(_value), v);
    }

    template<typename T>
    void Vector<T>::push_front(T to_push)
    {
        auto* v = (jl_value_t*) to_push;
        jl_call2(_push_front, reinterpret_cast<jl_value_t*>(_value), v);
    }

    template<typename T>
    void Vector<T>::insert(size_t pos, T value)
    {
        auto* v = (jl_value_t*) value;
        auto* i = jl_box_uint64(pos);
        jl_call3(_insert, reinterpret_cast<jl_value_t*>(_value), i, v);
    }

    template<typename T>
    void Vector<T>::erase(size_t pos)
    {
        auto* i = jl_box_uint64(pos);
        jl_call2(_erase, reinterpret_cast<jl_value_t*>(_value), i);
    }

    template<typename T>
    void Vector<T>::replace(size_t pos, T value)
    {
        auto* v = (jl_value_t*) value;
        auto* i = jl_box_uint64(pos);
        jl_call3(_replace, reinterpret_cast<jl_value_t*>(_value), v, i);
    }
}