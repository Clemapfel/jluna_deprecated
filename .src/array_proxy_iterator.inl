// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#include <array_proxy.hpp>
#include <box_any.hpp>

// NON CONST
namespace jlwrap
{
    template<typename T, size_t R>
    Array<T, R>::Iterator::Iterator(jl_array_t* array, size_t i)
        : _data(array), _index(i)
    {
        if (_replace == nullptr)
            _replace = jl_get_function(jl_base_module, "setindex!");

        assert(i <= array->length);
    }

    template<typename T, size_t R>
    Array<T, R>::Iterator::operator T()
    {
        return unbox<T>(jl_arrayref(const_cast<jl_array_t*>(_data), _index));
    }

    template<typename T, size_t R>
    Array<T, R>::Iterator::operator const jl_value_t*() const
    {
        return jl_arrayref(const_cast<jl_array_t*>(_data), _index);
    }

    template<typename T, size_t R>
    T Array<T, R>::Iterator::operator*() const
    {
        return unbox<T>(jl_arrayref(const_cast<jl_array_t*>(_data), _index));
    }

    template<typename T, size_t R>
    auto& Array<T, R>::Iterator::operator*()
    {
        return *this; //unbox<T>(jl_arrayref(const_cast<jl_array_t*>(_data), _index));
    }

    template<typename T, size_t R>
    void Array<T, R>::Iterator::operator++()
    {
        if (_index + 1 <= _data->length)
            ++_index;
    }

    template<typename T, size_t R>
    void Array<T, R>::Iterator::operator++(int i)
    {
        if (_index + i <= _data->length)
            _index++;
    }

    template<typename T, size_t R>
    void Array<T, R>::Iterator::operator--()
    {
        if (_index - 1 >= 0)
            --_index;
    }

    template<typename T, size_t R>
    void Array<T, R>::Iterator::operator--(int i)
    {
        if (_index - i >= 0)
            _index--;
    }

    template<typename T, size_t R>
    bool Array<T, R>::Iterator::operator==(const Iterator& other) const
    {
        return other._index == _index;
    }

    template<typename T, size_t R>
    bool Array<T, R>::Iterator::operator!=(const Iterator& other) const
    {
        return other._index != _index;
    }

    template<typename T, size_t R>
    auto& Array<T, R>::NonConstIterator::operator=(T value)
    {
        auto* v = box<T>(value);
        auto* i = jl_box_uint64(_index + 1);
        jl_call3(_replace, reinterpret_cast<jl_value_t*>(_data), v, i);
        return *this;
    }

    template<typename T, size_t R>
    Array<T, R>::NonConstIterator::NonConstIterator(jl_array_t* array, size_t i)
        : Array<T, R>::Iterator(array, i)
    {}

    template<typename T, size_t R>
    Array<T, R>::ConstIterator::ConstIterator(jl_array_t* array, size_t i)
        : Array<T, R>::Iterator(array, i)
    {}
}

/*

// CONST
namespace jlwrap
{
    template<typename T, size_t R>
    Array<T, R>::ConstIterator::ConstIterator(jl_array_t* array, size_t i)
        : _data(array), _index(i)
    {
        assert(i <= array->length);
    }

    template<typename T, size_t R>
    Array<T, R>::ConstIterator::operator const jl_value_t*() const
    {
        return jl_arrayref(const_cast<jl_array_t*>(_data), _index);
    }

    template<typename T, size_t R>
    Array<T, R>::ConstIterator::operator T() const
    {
        return unbox<T>(jl_arrayref(const_cast<jl_array_t*>(_data), _index));
    }

    template<typename T, size_t R>
    void Array<T, R>::ConstIterator::operator++()
    {
        if (_index + 1 <= _data->length)
            ++_index;
    }

    template<typename T, size_t R>
    void Array<T, R>::ConstIterator::operator++(int i)
    {
        if (_index + i <= _data->length)
            _index++;
    }

    template<typename T, size_t R>
    void Array<T, R>::ConstIterator::operator--()
    {
        if (_index - 1 >= 0)
            --_index;
    }

    template<typename T, size_t R>
    void Array<T, R>::ConstIterator::operator--(int i)
    {
        if (_index - i >= 0)
            _index--;
    }

    template<typename T, size_t R>
    bool Array<T, R>::ConstIterator::operator==(const ConstIterator& other) const
    {
        return other._index == _index;
    }

    template<typename T, size_t R>
    bool Array<T, R>::ConstIterator::operator!=(const ConstIterator& other) const
    {
        return other._index != _index;
    }
}
 */