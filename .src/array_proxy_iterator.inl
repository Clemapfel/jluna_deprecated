// 
// Copyright 2022 Clemens Cords
// Created on 12.01.22 by clem (mail@clemens-cords.com)
//

#include <array_proxy.hpp>

namespace jluna
{
    template<Boxable V, size_t R>
    template<bool IsConst>
    Array<V, R>::Iterator<IsConst>::Iterator(size_t i, Array<V, R>* array)
        : _index(i), _owner(array)
    {}

    template<Boxable V, size_t R>
    template<bool IsConst>
    void Array<V, R>::Iterator<IsConst>::operator++()
    {
        if (_index < _owner->get_n_elements())
            _index.operator++();
    }

    template<Boxable V, size_t R>
    template<bool IsConst>
    void Array<V, R>::Iterator<IsConst>::operator++(int i)
    {
        if (_index < _owner->get_n_elements())
            _index.operator++(i);
    }

    template<Boxable V, size_t R>
    template<bool IsConst>
    void Array<V, R>::Iterator<IsConst>::operator--()
    {
        if (_index > 0)
            _index.operator--();
    }

    template<Boxable V, size_t R>
    template<bool IsConst>
    void Array<V, R>::Iterator<IsConst>::operator--(int i)
    {
        if (_index > 0)
            _index.operator--(i);
    }

    template<Boxable V, size_t R>
    template<bool IsConst>
    bool Array<V, R>::Iterator<IsConst>::operator==(const typename Array<V, R>::Iterator<IsConst>& other) const
    {
        return static_cast<jl_value_t*>(this->_owner) == static_cast<jl_value_t*>(other._owner) and this->_index == other->_index;
    }

    template<Boxable V, size_t R>
    template<bool IsConst>
    bool Array<V, R>::Iterator<IsConst>::operator!=(const typename Array<V, R>::Iterator<IsConst>& other) const
    {
        return not (*this == other);
    }

    template<Boxable V, size_t R>
    template<bool IsConst>
    template<Unboxable T>
    T Array<V, R>::Iterator<IsConst>::operator*() const
    {
        return _owner->operator[]<T>(_index);
    }

    template<Boxable V, size_t R>
    template<bool IsConst>
    auto Array<V, R>::Iterator<IsConst>::operator*()
    {
        return _owner->operator[](_index);
    }
    
    template<Boxable V, size_t R>
    template<bool IsConst>
    template<Boxable T>
    void Array<V, R>::Iterator<IsConst>::operator=(T value)
    {
        if (_index == _owner->get_n_elements())
            throw std::out_of_range("In: jluna::Array::Iterator::operator=(): trying to assign value to past-the-end iterator")

        auto* boxed = box(value);
        jl_arrayset(static_cast<jl_value_t*>(*_owner), value, _index);
    }
}