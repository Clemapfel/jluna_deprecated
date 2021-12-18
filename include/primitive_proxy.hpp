// 
// Copyright 2021 Clemens Cords
// Created on 15.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <proxy.hpp>

namespace jlwrap
{
    /// @brief proxy holding ownership of a primitive typed variable julia-side. Modifying the proxy modifies the julia-side variable at the same time
    class PrimitiveProxy : public Proxy
    {
        public:
            /// @brief ctor, can only be constructed by State
            PrimitiveProxy() = delete;

            /// @brief cast to bool
            operator bool();

            /// @brief cast bool argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(bool);

            /// @brief cast to char
            operator char();

            /// @brief cast char argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(char);

            /// @brief cast to float
            operator float();

            /// @brief cast float argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(float);

            /// @brief cast to double
            operator double();

            /// @brief cast double argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(double);

            /// @brief cast to uint8_t
            operator uint8_t();

            /// @brief cast uint8_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(uint8_t);

            /// @brief cast to uint16_t
            operator uint16_t();

            /// @brief cast uint16_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(uint16_t);

            /// @brief cast to uint32_t
            operator uint32_t();

            /// @brief cast uint32_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(uint32_t);

            /// @brief cast to uint64_t
            operator uint64_t();

            /// @brief cast uint64_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(uint64_t);

            /// @brief cast to int8_t
            operator int8_t();

            /// @brief cast int8_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(int8_t);

            /// @brief cast to int16_t
            operator int16_t();

            /// @brief cast int16_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(int16_t);

            /// @brief cast to int32_t
            operator int32_t();

            /// @brief cast int32_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(int32_t);

            /// @brief cast to int64_t
            operator int64_t();

            /// @brief cast int64_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            PrimitiveProxy& operator=(int64_t);

            /// @brief cast to nullptr
            operator nullptr_t();

            /// @brief assign null (or the underlying types version of null
            /// @param arg
            PrimitiveProxy& operator=(nullptr_t);

            /// @brief cast 
            /// @tparam type to cast to, must be primitive
            template<typename T>
            T cast_to() const;

            /// @brief cast argument to the underlying type, then assign. Will change value julia-side
            /// @tparam type of argument to be assigned
            /// @param arg
            template<typename T>
            PrimitiveProxy& assign(T);

        protected:
            /// @brief ctor
            /// @param value: pointer to julia-side value
            /// @param symbol: pointer to julia-side symbol or nullptr
            PrimitiveProxy(jl_sym_t* symbol, jl_value_t* value);

            /// @brief if value is primitive, get type
            static jl_datatype_t* get_type(jl_value_t*);

        private:
            using Proxy::_symbol;
            using Proxy::_value;
            using Proxy::_type;
    };
}

#include ".src/primitive_proxy.inl"