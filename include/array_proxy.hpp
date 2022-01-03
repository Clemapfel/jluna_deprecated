// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <vector>
#include <proxy.hpp>
#include <state.hpp>

namespace jlwrap
{
    /// @brief wrapper for julia arrays of arbitrary value type
    template<typename T, size_t Rank>
    class Array : public Proxy<State>
    {
        /// @brief iterator super class, detail only
        class Iterator;

        /// @brief iterator with no julia-side assignment operator
        struct ConstIterator;

        /// @brief iterator with julia-side assignment operator
        struct NonConstIterator;

        public:
            /// @brief value type
            using value_type = T;

            /// @brief dimensionality, equivalent to julia-side Array{T, Rank}
            static constexpr size_t rank = Rank;

            /// @brief ctor
            /// @param julia-side value, asserted to inherit from AbstractArray
            Array(jl_value_t*);

            /// @brief ctor
            /// @param julia-side array value
            Array(jl_array_t*);

            /// @brief decay to julia C-API pointer
            operator jl_array_t*();

            /// @brief const-access linear indexing, no bounds checking
            /// @param i: index, 0-based
            /// @returns non-assignable iterator to element
            const auto operator[](size_t) const;

            /// @brief non-const access linear indexing, no bounds checking
            /// @param i: index, 0-based
            /// @returns assignable iterator to element
            auto operator[](size_t);

            /// @brief multi-dimensional indexing
            /// @tparam exactly Rank-many indices of integral type, each index is 0-based and asserted to be smaller than the corresponding length along that dimensions
            /// @returns assignable iterator to element
            template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            auto at(Args... in);

            /// @brief multi-dimensional indexing
            /// @tparam: excatly Rank-many indices of integral type, each index is 0-based and asserted to be smaller than the corresponding length along that dimensions
            /// @returns assignable iterator to element
            template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            const auto at(Args... in) const;

            /// @brief get total number of elements
            /// @returns size_t
            size_t size();

            /// @brief get length along dimensions
            /// @param dimension_index: index, range [0, Rank-1]
            /// @returns size_t
            size_t size(size_t dimension_index);

            /// @brief iterable begin
            /// @returns assignable iterator to element at index 0
            auto begin();

            /// @brief iterable begin
            /// @returns non-assignable iterator to element at index 0
            auto begin() const;

            /// @brief iterable begin
            /// @returns assignable iterator to past-the-end element
            auto end();

            /// @brief iterable begin
            /// @returns non-assignable iterator to past-the-end element
            auto end() const;

        protected:
            using Proxy<State>::_value;

        private:
            void throw_if_index_out_of_range(int index, size_t dimension);
            size_t get_dimension(size_t);
    };

    /// @brief 1-dimensional array specialization
    template<typename T>
    class Vector : public Array<T, 1>
    {
        public:
            /// @brief default ctor
            Vector();

            /// @brief is empty
            /// @returns bool
            bool empty() const;

            /// @brief insert element at index, increasing length by 1
            /// @param pos: index
            /// @param value: element
            void insert(size_t pos, T value);

            /// @brief erase element at index, decreasing length by 1
            /// @param pos: index
            void erase(size_t pos);

            /// @brief set element at index, not altering length
            /// @param pos: index
            /// @param value: new value
            void replace(size_t pos, T value);

            /// @brief append to front, increasing length by 1
            /// @param T: value
            void push_back(T);

            /// @brief append to back, increasing length by 1
            /// @param T: value
            void push_front(T);

        protected:
            using Array<T, 1>::_value;

        private:
            static inline jl_function_t* _push_front = nullptr;
            static inline jl_function_t* _push_back = nullptr;
            static inline jl_function_t* _insert = nullptr;
            static inline jl_function_t* _erase = nullptr;
            static inline jl_function_t* _replace = nullptr;
    };
}

#include ".src/array_proxy.inl"
#include ".src/array_proxy_iterator.inl"
#include ".src/array_vector.inl"
