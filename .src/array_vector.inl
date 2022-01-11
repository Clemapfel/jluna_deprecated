// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#include <array_proxy.hpp>

namespace jluna
{
    template<typename T>
    inline jl_value_t* box(Vector<T> value)
    {
        return (jl_value_t*) value.operator jl_array_t*();
    }

    template<typename T>
    inline jl_value_t* box(Vector<T>& value)
    {
        return (jl_value_t*) value.operator jl_array_t*();
    }

    /// @brief unbox to vector, flattens multidimensional arrays
    template<typename T,
        typename Value_t = typename T::value_type,
        std::enable_if_t<std::is_same_v<T, std::vector<Value_t>>, bool> = true>
    T unbox(jl_value_t* value)
    {
        THROW_IF_UNINITIALIZED;
        if (not jl_is_array(value))
            assert(false);

        auto* as_array = (jl_array_t*) value;

        std::vector<Value_t> out;
        out.reserve(as_array->length);

        for (size_t i = 0; i < as_array->length; ++i)
            out.push_back(unbox<Value_t>(jl_arrayref(as_array, i)));

        return out;
    }

    template<Boxable T>
    Vector<T>::Vector()
    {
        THROW_IF_UNINITIALIZED;

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

    template<Boxable T>
    bool Vector<T>::empty() const
    {
        return Array<T, 1>::_content->length == 0;
    }

    template<Boxable T>
    void Vector<T>::push_back(T to_push)
    {
        auto* v = (jl_value_t*) to_push;
        jl_call2(_push_back, box<T>(_content->_value), v);
    }

    template<Boxable T>
    void Vector<T>::push_front(T to_push)
    {
        auto* v = (jl_value_t*) to_push;
        jl_call2(_push_front, box<T>(_content->_value), v);
    }

    template<Boxable T>
    void Vector<T>::insert(size_t pos, T value)
    {
        auto* v = (jl_value_t*) value;
        auto* i = jl_box_uint64(pos);
        jl_call3(_insert, box<T>(_content->_value), i, v);
    }

    template<Boxable T>
    void Vector<T>::erase(size_t pos)
    {
        auto* i = jl_box_uint64(pos);
        jl_call2(_erase, box<T>(_content->_value), i);
    }

    template<Boxable T>
    void Vector<T>::replace(size_t pos, T value)
    {
        auto* v = (jl_value_t*) value;
        auto* i = jl_box_uint64(pos);
        jl_call3(_replace, box<T>(_content->_value), v, i);
    }
}