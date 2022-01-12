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
        template<bool>
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

            template<std::enable_if_t<Rank == 1, bool> = true>
            void insert(size_t pos, Value_t value);

            template<std::enable_if_t<Rank == 1, bool> = true>
            void erase(size_t pos);

            template<std::enable_if_t<Rank == 1, bool> = true>
            void push_front(Value_t value);

            template<std::enable_if_t<Rank == 1, bool> = true>
            void push_back(Value_t value);

        private:
            void throw_if_index_out_of_range(int index, size_t dimension);
            using Proxy<State>::_content;

            template<bool IsConst>
            class Iterator
            {
                public:
                    /// @brief ctor
                    Iterator(size_t i, Array<Value_t, Rank>*);

                    template<Boxable T = Value_t, std::enable_if_t<not IsConst, bool> = true>
                    auto& operator=(T value);

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

                    /// @brief decays into value_type
                    template<Unboxable T = Value_t>
                    T operator*() const;

                    /// @brief decay into proxy
                    Proxy<State> operator*();

                private:
                    Array<Value_t, Rank>* _owner;
                    size_t _index;
            };
    };

    /// @brief vector typedef
    template<Boxable Value_t>
    using Vector = Array<Value_t, 1>;

    template<Boxable Value_t, size_t Rank>
    std::array<size_t, Rank> size(const Array<Value_t, Rank>&);

    template<Boxable Value_t, size_t Rank>
    std::array<size_t, Rank> size(const Array<Value_t, Rank>&);
}

#include ".src/array_proxy.inl"