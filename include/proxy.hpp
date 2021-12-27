// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <unordered_map>

namespace jlwrap
{
    /// @brief concept of a variable, has a type, name (symbol) and value
    /// @note do not inherit from this class unless you know what you're doing, improper handling of references to julia-side values can result in memory leaks or the garbage collector freeing in-use memory
    template<typename State_t>
    class Proxy
    {
        public:
            /// @brief ctor deleted
            Proxy() = delete;

            operator jl_value_t*();

            auto operator[](std::string field_name);
            const auto operator[](std::string field_name) const;

            template<typename T>
            operator T();

            /// @brief construct, adds a reference that holds ownership of the value to protect it from the garbage collection
            /// @param symbol: name of the variable as julia symbol
            /// @param value: pointer to the value of the variable
            /// @param type: pointer to the type of the variable
            Proxy(jl_value_t* value);

            /// @brief dtor, frees the reference so it can be garbage collected if appropriate
            ~Proxy();

            /// @brief copy ctor, both resulting objects have ownership and the value will only be deallocated, if the number of owners is exactly 0
            /// @param other
            Proxy(const Proxy&);

            /// @brief copy ctor, both resulting objects have ownership and the value will only be deallocated, if the number of owners is exactly 0
            /// @param other
            Proxy& operator=(const Proxy&);

            /// @brief move ctor, only *this* will hold ownership
            /// @param other
            Proxy(Proxy&&) noexcept;

            /// @brief move assignment, only *this* will hold ownership
            /// @param other
            Proxy& operator=(Proxy&&) noexcept;

        private:
            jl_value_t* _value;

            std::unordered_map<std::string, size_t> _field_to_index;
    };
}

#include ".src/proxy.inl"