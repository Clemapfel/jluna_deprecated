// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#include <array_proxy.hpp>

// NON CONST
namespace jlwrap
{
    template<typename T, size_t R>
    Array<T, R>::Iterator::Iterator(jl_array_t* array, size_t i)
        : _data(array), _index(i)
    {
        assert(i <= array->length);
    }

    template<typename T, size_t R>
    auto& Array<T, R>::Iterator::operator=(T value)
    {
        auto* v = (jl_value_t*) value;
        auto* i = jl_box_uint64(_index);
        jl_call3(_replace, reinterpret_cast<jl_value_t*>(_data), v, i);
    }

    template<typename T, size_t R>
    Array<T, R>::Iterator::operator jl_value_t*()
    {
        return jl_arrayref(_data, _index);
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
}

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
        return const_cast<const jl_value_t*>(jl_arrayref(const_cast<jl_array_t*>(_data), _index));
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