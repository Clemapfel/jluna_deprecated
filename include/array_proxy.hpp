// 
// Copyright 2022 Clemens Cords
// Created on 11.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <proxy.hpp>
#include <state.hpp>

namespace jluna
{
    template<Boxable T, size_t Rank>
    class Array : public Proxy<State>
    {
        template<bool>
        class Iterator;

        public:
            /// @brief value type
            using value_type = T;

            /// @brief dimensionality, equivalent to julia-side Array{T, Rank}
            static constexpr size_t rank = Rank;

            /// @brief ctor
            Array(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>&, jl_sym_t*);

            /// @brief linear indexing, no bounds checking
            auto operator[](size_t);

            /// @brief linear indexing, no bounds checking
            template<Unboxable T>
            T operator[](size_t) const;

            /// @brief multi-dimensional indexing
            template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            auto at(Args... in);

            /// @brief multi-dimensional indexing
            template<Unboxable T, typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool> = true>
            T at(Args... in) const;

            auto begin();
            auto begin() const;

            auto end();
            auto end() const;

            auto front();
            T front() const;

            auto back();
            T back() const;

            /// VECTOR UTILS

            template<std::enable_if_t<Rank == 1, bool> = true>
            void insert(size_t pos, T value);

            template<std::enable_if_t<Rank == 1, bool> = true>
            void erase(size_t pos, T value);

            template<std::enable_if_t<Rank == 1, bool> = true>
            void insert(size_t pos, T value);

            template<std::enable_if_t<Rank == 1, bool> = true>
            void insert(size_t pos, T value);

            template<std::enable_if_t<Rank == 1, bool> = true>
            void push_front(T value);

            template<std::enable_if_t<Rank == 1, bool> = true>
            void push_back(T value);

        protected:
            void throw_if_index_out_of_range(int index, size_t dimension);
            using Proxy<State>::_content;

            template<bool IsConst>
            class Iterator : public Proxy<State>
            {
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

                    /// @brief decays into value_type
                    T operator*() const;

                    /// @brief decay into proxy
                    Proxy<State> operator*();

                private:
                    using Proxy<State>::_content;
                    size_t _index;
            };
    };

    /// @brief vector typedef
    template<Boxable T>
    using Vector = Array<T, 1>;

    // TODO
    template<Boxable T, size_t Rank>
    std::array<size_t, Rank> size(const Array<T, Rank>&);

    template<Boxable T, size_t Rank>
    std::array<size_t, Rank> size(const Array<T, Rank>&);
}