// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

namespace jlwrap
{
    /// @brief concept of a variable, has a type, name (symbol) and value
    /// @note This is an interface-like class that should only be used by internal jlwrap code, not the user.
    ///       Do not inherit from this class unless you know what you're doing.
    template<typename State_t>
    class Proxy
    {
        public:
            /// @brief ctor deleted
            Proxy() = delete;

        protected:
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

            jl_value_t* _value;
    };
}

#include ".src/proxy.inl"