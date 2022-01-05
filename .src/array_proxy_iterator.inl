// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#include <array_proxy.hpp>
#include <box_any.hpp>

// NON CONST
namespace jluna
{
    /// @brief iterator superclass, handles assignment, only children ConstIterator and NonConstIterator are actually returned by Array
    template<typename T, size_t R>
    class Array<T, R>::Iterator
    {
        friend class Array<T, R>;

        public:
            /// @brief increment
            void operator++();

            /// @brief post-fix increment
            void operator++(int);

            /// @brief post-fix decrement
            void operator--();

            /// @brief post-fix decrement
            void operator--(int);

            /// @brief equality operator
            /// @param other
            /// @returns bool
            bool operator==(const Iterator&) const;

            /// @brief inequality operator
            /// @param other
            /// @returns bool
            bool operator!=(const Iterator&) const;

            /// @brief decays into value_type if called in const-context
            /// @returns non-reference value_type
            T operator*() const;

            /// @brief stays as assignable proxy iterator in non-const-context
            /// @returns reference to self
            auto& operator*();

            /// @brief cast to value_type
            operator T();

            /// @brief cast to julia C-API pointer
            explicit operator const jl_value_t*() const;

        protected:
            Iterator(jl_array_t*, size_t);

            jl_array_t* _data;
            size_t _index;

            static inline jl_function_t* _replace = nullptr;
    };

    template<typename T, size_t R>
    class Array<T, R>::NonConstIterator : public Array<T, R>::Iterator
    {
        public:
            /// @brief assign julia-side
            /// @param new value
            /// @returns reference to self
            auto& operator=(T);

            /// @brief ctor
            /// @param array
            /// @param index, 0-base
            NonConstIterator(jl_array_t*, size_t);

        private:
            using Array<T, R>::Iterator::_data;
            using Array<T, R>::Iterator::_index;
            using Array<T, R>::Iterator::_replace;
    };

    template<typename T, size_t R>
    struct Array<T, R>::ConstIterator : public Array<T, R>::Iterator
    {
        /// @brief ctor
        /// @param array
        /// @param index, 0-base
        ConstIterator(jl_array_t*, size_t);
    };

    template<typename T, size_t R>
    inline jl_value_t* box(typename Array<T, R>::Iterator value)
    {
        return value.operator const _jl_value_t *();
    }

    template<typename T, size_t R>
    inline jl_value_t* box(typename Array<T, R>::Iterator& value)
    {
        return value.operator const _jl_value_t *();
    }

    template<typename T, size_t R>
    inline jl_value_t* box(typename Array<T, R>::NonConstIterator value)
    {
        return value.operator const _jl_value_t *();
    }

    template<typename T, size_t R>
    inline jl_value_t* box(typename Array<T, R>::NonConstIterator& value)
    {
        return value.operator const _jl_value_t *();
    }

    template<typename T, size_t R>
    inline jl_value_t* box(typename Array<T, R>::ConstIterator value)
    {
        return value.operator const _jl_value_t *();
    }

    template<typename T, size_t R>
    inline jl_value_t* box(typename Array<T, R>::ConstIterator& value)
    {
        return value.operator const _jl_value_t *();
    }

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