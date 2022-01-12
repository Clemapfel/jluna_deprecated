// 
// Copyright 2022 Clemens Cords
// Created on 11.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <proxy.hpp>

namespace jluna
{
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
            Array(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>&, jl_sym_t*);

            /// @brief linear indexing, no bounds checking
            auto operator[](size_t);

            /// @brief linear indexing, no bounds checking
            template<Unboxable T = Value_t>
            T operator[](size_t) const;

            /// @brief multi-dimensional indexing
            template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            auto at(Args... in);

            /// @brief multi-dimensional indexing
            template<Unboxable T = Value_t, typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            T at(Args... in) const;

            template<Boxable T = Value_t>
            void set(size_t i, T);

            size_t get_n_elements() const;

            auto begin();
            auto begin() const;

            auto end();
            auto end() const;

            auto front();

            template<Unboxable T = Value_t>
            T front() const;

            auto back();

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

                    template<Unboxable T = Value_t>
                    operator T() const;

                    operator Proxy<State>();

                protected:
                    Array<Value_t, Rank>* _owner;
                    size_t _index;
            };

            struct Iterator : public ConstIterator
            {
                ///
                Iterator(size_t i, Array<Value_t, Rank>*);

                using ConstIterator::operator*;

                ///
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
        Vector(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>&, jl_sym_t*);

        void insert(size_t pos, Value_t value);

        void erase(size_t pos);

        void push_front(Value_t value);

        void push_back(Value_t value);
    };

    template<Boxable Value_t, size_t Rank>
    std::array<size_t, Rank> size(const Array<Value_t, Rank>&);

    template<Boxable Value_t, size_t Rank>
    std::array<size_t, Rank> size(const Array<Value_t, Rank>&);
}

#include ".src/array_proxy.inl"
#include ".src/array_proxy_iterator.inl"