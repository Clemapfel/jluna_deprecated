// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <vector>

namespace jlwrap
{
    /// @brief wrapper for julia arrays of arbitrary value type
    template<typename T, size_t R>
    class Array : public Proxy<State>
    {
        class Iterator;
        class ConstIterator;

        public:
            Array(jl_value_t*);
            Array(jl_array_t*);

            [[implicit]] operator jl_array_t*();

            auto at(size_t);
            const auto at(size_t) const;

            auto operator[](size_t);
            const auto operator[](size_t) const;

            auto begin();
            auto begin() const;
            auto end();
            auto end() const;

        protected:
            jl_array_t* _value;
    };

    /// @brief 1-dimension array specialization
    template<typename T>
    class Vector : public Array<T, 1>
    {
        public:
            bool empty() const;
            size_t size() const;

            void insert(size_t pos, T value);
            void erase(size_t pos);
            void replace(size_t pos, T value);

            void push_back(T);
            void push_front(T);

        protected:
            using Array<T, 1>::_value;

        private:
            static inline jl_function_t* _push_front = jl_get_function(jl_main_module, "pushfirst!");
            static inline jl_function_t* _push_back = jl_get_function(jl_main_module, "append!");
            static inline jl_function_t* _insert = jl_get_function(jl_main_module, "insert!");
            static inline jl_function_t* _erase = jl_get_function(jl_main_module, "deleteat!");
            static inline jl_function_t* _replace = jl_get_function(jl_main_module, "setindex!");
    };

    /// @brief non-const iterator
    template<typename T, size_t R>
    class Array<T, R>::Iterator
    {
        friend class Array<T, R>;

        public:
            auto& operator=(T);

            void operator++();
            void operator++(int);
            void operator--();
            void operator--(int);

            bool operator==(const Iterator&) const;
            bool operator!=(const Iterator&) const;

        protected:
            Iterator(jl_array_t*, size_t);
            operator jl_value_t*();

        private:
            jl_array_t* _data;
            size_t _index;

            static inline jl_function_t* _replace = jl_get_function(jl_main_module, "setindex!");

    };

    /// @brief const iterator
    template<typename T, size_t R>
    class Array<T, R>::ConstIterator
    {
        friend class Array<T, R>;

        public:
            void operator++();
            void operator++(int);
            void operator--();
            void operator--(int);

            bool operator==(const ConstIterator&) const;
            bool operator!=(const ConstIterator&) const;

        protected:
            ConstIterator(jl_array_t*, size_t);
            operator const jl_value_t*() const;

        private:
            const jl_array_t* _data;
            size_t _index;
    };

}

#include ".src/array_proxy.inl"
#include ".src/array_proxy_iterator.inl"
#include ".src/array_vector.inl"
