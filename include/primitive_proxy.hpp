// 
// Copyright 2021 Clemens Cords
// Created on 15.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <proxy.hpp>

namespace jlwrap
{
    union State;

    /// @brief proxy holding ownership of a primitive typed variable julia-side. Modifying the proxy modifies the julia-side variable at the same time
    class Primitive : public Proxy<State>
    {
        friend union State;

        public:
            /// @brief ctor, can only be constructed by State
            Primitive() = delete;

            /// @brief cast to bool
            operator bool();

            /// @brief cast to char
            operator char();

            /// @brief cast to float
            operator float();

            /// @brief cast to double
            operator double();

            /// @brief cast to uint8_t
            operator uint8_t();

            /// @brief cast to uint16_t
            operator uint16_t();

            /// @brief cast to uint32_t
            operator uint32_t();

            /// @brief cast to uint64_t
            operator uint64_t();

            /// @brief cast to int8_t
            operator int8_t();

            /// @brief cast to int16_t
            operator int16_t();

            /// @brief cast to int32_t
            operator int32_t();

            /// @brief cast to int64_t
            operator int64_t();

            /// @brief cast to nullptr
            operator nullptr_t();

            /// @brief cast
            /// @tparam type to cast to, must be primitive
            template<typename T>
            T cast_to() const;

        protected:
            /// @brief ctor
            /// @param value: pointer to julia-side value
            /// @param symbol: pointer to julia-side symbol or nullptr
            Primitive(jl_value_t* value);

            /// @brief if value is primitive, get type
            static jl_datatype_t* get_type(jl_value_t*);

        private:
            using Proxy::_value;
            jl_datatype_t* _type;
    };
}

#include ".src/primitive_proxy.inl"