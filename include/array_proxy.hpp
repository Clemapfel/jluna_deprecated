// 
// Copyright 2022 Clemens Cords
// Created on 11.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <proxy.hpp>

namespace jluna
{
    /// @brief wrapper for julia-side Array{Value_t, Rank}
    template<Boxable Value_t, size_t Rank>
    class Array : public Proxy<State>
    {
        friend class ConstIterator;
        class Iterator;

        public:
            /// @brief value type
            using value_type = Value_t;

            /// @brief dimensionality, equivalent to julia-side Array{Value_t, Rank}
            static constexpr size_t rank = Rank;

            /// @brief ctor
            /// @param value
            /// @param owner
            /// @param symbol
            Array(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>&, jl_sym_t*);

            /// @brief linear indexing, no bounds checking
            /// @param index, 0-based
            /// @returns assignable iterator to element
            auto operator[](size_t);

            /// @brief linear indexing, no bounds checking
            /// @tparam return type
            /// @param index, 0-based
            /// @returns unboxed value
            template<Unboxable T = Value_t>
            T operator[](size_t) const;

            /// @brief multi-dimensional indexing
            /// @tparam integral type
            /// @param n integrals, where n is the rank of the array
            /// @returns assignable iterator to value
            template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            auto at(Args... in);

            /// @brief multi-dimensional indexing
            /// @tparam integral type
            /// @param n integrals, where n is the rank of the array
            /// @returns unboxed value
            template<Unboxable T = Value_t, typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            T at(Args... in) const;

            /// @brief manually assign a value using a linear index
            /// @param index: 0-based
            /// @param value
            template<Boxable T = Value_t>
            void set(size_t i, T);

            /// @brief get number of elements, equal to Base.length
            /// @returns length
            size_t get_n_elements() const;

            /// @brief get iterator to 0-indexed element
            /// @returns assignable iterator
            auto begin();

            /// @brief get iterator to 0-indexed element
            /// @returns const iterator
            auto begin() const;

            /// @brief get iterator to past-the-end element
            /// @returns assignable iterator
            auto end();

            /// @brief get iterator to past-the-end element
            /// @returns const iterator
            auto end() const;

            /// @brief get first element, equivalent to operator[](0)
            /// @returns assignable iterator
            auto front();

            /// @brief get first element, equivalent to operator[](0)
            /// @returns unboxed value
            template<Unboxable T = Value_t>
            T front() const;

            /// @brief get last valid element
            /// @returns unboxed value
            auto back();

            /// @brief get last valid element
            /// @returns unboxed value
            template<Unboxable T = Value_t>
            T back() const;

        private:
            void throw_if_index_out_of_range(int index, size_t dimension);
            size_t get_dimension(int);
            using Proxy<State>::_content;

            class ConstIterator
            {
                public:
                    /// @brief ctor
                    /// @param index
                    /// @param pointer to array
                    ConstIterator(size_t i, Array<Value_t, Rank>*);

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
                    bool operator==(const ConstIterator&) const;

                    /// @brief inequality operator
                    /// @param other
                    /// @returns bool
                    bool operator!=(const ConstIterator&) const;

                    /// @brief decays into value_type
                    template<Unboxable T = Value_t>
                    T operator*() const;

                    /// @brief decay into proxy
                    auto operator*();

                    /// @brief decay into unboxed value
                    /// @tparam value-type, not necessarily the same as declared in the array type
                    template<Unboxable T = Value_t>
                    operator T() const;

                    /// @brief decay into proxy
                    operator Proxy<State>();

                protected:
                    Array<Value_t, Rank>* _owner;
                    size_t _index;
            };

            struct Iterator : public ConstIterator
            {
                /// @brief ctor
                /// @param index
                /// @param pointer to array
                Iterator(size_t i, Array<Value_t, Rank>*);

                using ConstIterator::operator*;

                /// @brief assign value, also assign value of proxy, regardless of wether it is mutating
                /// @param value
                /// @returns reference to self
                template<Boxable T = Value_t>
                auto& operator=(T value);

                protected:
                    using ConstIterator::_owner;
                    using ConstIterator::_index;
            };
    };

    /// @brief vector typedef
    template<Boxable Value_t>
    struct Vector : public Array<Value_t, 1>
    {
        /// @brief ctor
        /// @param value
        /// @param owner
        /// @param symbol
        Vector(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>&, jl_sym_t*);

        /// @brief insert
        /// @param linear index, 0-based
        /// @param value
        void insert(size_t pos, Value_t value);

        /// @brief erase
        /// @param linear index, 0-based
        void erase(size_t pos);

        /// @brief add to front
        /// @tparam type of value, not necessarily the same as the declared array type
        /// @param value
        template<Boxable T = Value_t>
        void push_front(T value);

        /// @brief add to back
        /// @tparam type of value, not necessarily the same as the declared array type
        /// @param value
        template<Boxable T = Value_t>
        void push_back(T value);
    };
}

#include ".src/array_proxy.inl"
#include ".src/array_proxy_iterator.inl"