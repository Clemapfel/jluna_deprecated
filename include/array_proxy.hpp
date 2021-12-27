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
    template<typename T, size_t Rank>
    class Array : public Proxy<State>
    {
        class Iterator;
        struct ConstIterator;
        struct NonConstIterator;

        public:
            using value_type = T;
            static constexpr size_t rank = Rank;

            Array(jl_value_t*);
            Array(jl_array_t*);

            [[implicit]] operator jl_array_t*();

            // linear indexing
            const auto operator[](size_t) const;
            auto operator[](size_t);

            template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            auto at(Args... in);

            template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            const auto at(Args... in) const;

            size_t length() const;

            auto begin();
            auto begin() const;
            auto end();
            auto end() const;

        protected:
            ConstIterator get(size_t) const;
            NonConstIterator get(size_t);

            jl_array_t* _value;

        private:
            void throw_if_index_out_of_range(int index, size_t dimension);
            size_t get_dimension(size_t);
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

    /// @brief iterator superclass, handles assignment, only children ConstIterator and NonConstIterator are actually returned by Array
    template<typename T, size_t R>
    class Array<T, R>::Iterator
    {
        friend class Array<T, R>;

        public:
            void operator++();
            void operator++(int);
            void operator--();
            void operator--(int);

            bool operator==(const Iterator&) const;
            bool operator!=(const Iterator&) const;

            // const: decays into value type
            T operator*() const;

            // non-const: stays as assignable iterator
            auto& operator*();

            operator T();
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
            auto& operator=(T);

            NonConstIterator(jl_array_t*, size_t);

        private:
            using Array<T, R>::Iterator::_data;
            using Array<T, R>::Iterator::_index;
            using Array<T, R>::Iterator::_replace;
    };

    template<typename T, size_t R>
    struct Array<T, R>::ConstIterator : public Array<T, R>::Iterator
    {
        ConstIterator(jl_array_t*, size_t);
    };
}

#include ".src/array_proxy.inl"
#include ".src/array_proxy_iterator.inl"
#include ".src/array_vector.inl"
