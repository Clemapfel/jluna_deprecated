// 
// Copyright 2021 Clemens Cords
// Created on 15.12.21 by clem (mail@clemens-cords.com)
//

#include <julia.h>
#include <julia_extension.h>
#include <primitive_proxy.hpp>
#include <proxy.hpp>
#include <sstream>

namespace jlwrap
{
    jl_datatype_t * Primitive::get_type(jl_value_t* v)
    {
        if (jl_typeis(v, jl_bool_type))
            return jl_bool_type;
        else if (jl_typeis(v, jl_char_type))
            return jl_char_type;
        else if (jl_typeis(v, jl_int8_type))
            return jl_int8_type;
        else if (jl_typeis(v, jl_int16_type))
            return jl_int16_type;
        else if (jl_typeis(v, jl_int32_type))
            return jl_int32_type;
        else if (jl_typeis(v, jl_int64_type))
            return jl_int64_type;
        else if (jl_typeis(v, jl_uint8_type))
            return jl_uint8_type;
        else if (jl_typeis(v, jl_uint16_type))
            return jl_uint16_type;
        else if (jl_typeis(v, jl_uint32_type))
            return jl_uint32_type;
        else if (jl_typeis(v, jl_uint64_type))
            return jl_uint64_type;
        else if (jl_typeis(v, jl_float16_type))
            return jl_float16_type;
        else if (jl_typeis(v, jl_float32_type))
            return jl_float32_type;
        else if (jl_typeis(v, jl_float64_type))
            return jl_float64_type;
        else if (jl_typeis(v, jl_void_type))
            return jl_void_type;
        else if (jl_typeis(v, jl_voidpointer_type))
            return jl_voidpointer_type;
        else if (jl_typeis(v, jl_nothing_type))
            return jl_nothing_type;
        else
            return nullptr;
    }

    Primitive::Primitive(jl_value_t* value)
        : Proxy(value), _type(get_type(value))
    {
        _value = Proxy<State>::operator _jl_value_t *();
    }

    template<typename T>
    T Primitive::cast_to() const
    {
        static_assert(std::is_fundamental_v<T> or std::is_same_v<T, nullptr_t>);

        if (_type == jl_bool_type)
        {
            auto temp = jl_unbox_bool(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_char_type)
        {
            auto temp = jl_unbox_int8(_value);
            return static_cast<T>(static_cast<char>(temp));
        }
        else if (_type == jl_int8_type)
        {
            auto temp = jl_unbox_int8(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_int16_type)
        {
            auto temp = jl_unbox_int16(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_int32_type)
        {
            auto temp = jl_unbox_int32(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_int64_type)
        {
            auto temp = jl_unbox_int64(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_uint8_type)
        {
            auto temp = jl_unbox_uint8(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_uint16_type)
        {
            auto temp = jl_unbox_uint16(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_uint32_type)
        {
            auto temp = jl_unbox_uint32(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_uint64_type)
        {
            auto temp = jl_unbox_int64(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_float16_type)
        {
            auto temp = jl_unbox_float16(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_float32_type)
        {
            auto temp = jl_unbox_float32(_value);
            return static_cast<T>(temp);
        }
        else if (_type == jl_float64_type)
        {
            auto temp = jl_unbox_float64(_value);
            return static_cast<T>(temp);
        }
        else
        {
            return static_cast<T>(NULL);
        }
    }

    Primitive::operator bool() const
    {
        return cast_to<bool>();
    }

    Primitive::operator char() const
    {
        return cast_to<char>();
    }

    Primitive::operator float() const
    {
        return cast_to<float>();
    }

    Primitive::operator double() const
    {
        return cast_to<double>();
    }

    Primitive::operator uint8_t() const
    {
        return cast_to<uint8_t>();
    }

    Primitive::operator uint16_t() const
    {
        return cast_to<uint16_t>();
    }

    Primitive::operator uint32_t() const
    {
        return cast_to<uint32_t>();
    }

    Primitive::operator uint64_t() const
    {
        return cast_to<uint64_t>();
    }

    Primitive::operator int8_t() const
    {
        return cast_to<int8_t>();
    }

    Primitive::operator int16_t() const
    {
        return cast_to<int16_t>();
    }

    Primitive::operator int32_t() const
    {
        return cast_to<int32_t>();
    }

    Primitive::operator int64_t() const
    {
        return cast_to<int64_t>();
    }

    /*
            /// @brief cast bool argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(bool);

            /// @brief cast char argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(char);

            /// @brief cast float argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(float);

            /// @brief cast double argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(double);

            /// @brief cast uint8_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(uint8_t);

            /// @brief cast uint16_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(uint16_t);

            /// @brief cast uint32_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(uint32_t);

            /// @brief cast uint64_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(uint64_t);

            /// @brief cast int8_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(int8_t);

            /// @brief cast int16_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(int16_t);

            /// @brief cast int32_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(int32_t);

            /// @brief cast int64_t argument to the underlying type, then assign. Will change value julia-side
            /// @param arg
            Primitive& operator=(int64_t);

            /// @brief assign null (or the underlying types version of null
            /// @param arg
            Primitive& operator=(nullptr_t);

            /// @brief cast argument to the underlying type, then assign. Will change value julia-side
            /// @tparam type of argument to be assigned
            /// @param arg
            template<typename T>
            Primitive& assign(T);
             */

    /*

    template<typename T>
    Primitive & Primitive::assign(T value)
    {
        static_assert(std::is_fundamental_v<T> or std::is_same_v<T, nullptr_t>);

        std::string cast;
        if (std::is_same_v<T, bool>)
            cast = "Bool";
        else if (std::is_same_v<T, char>)
            cast = "Char";
        else if (std::is_same_v<T, int8_t>)
            cast = "Int8";
        else if (std::is_same_v<T, int16_t>)
            cast = "Int16";
        else if (std::is_same_v<T, int32_t>)
            cast = "Int32";
        else if (std::is_same_v<T, int64_t>)
            cast = "Int64";
        else if (std::is_same_v<T, uint8_t>)
            cast = "UInt8";
        else if (std::is_same_v<T, uint16_t>)
            cast = "UInt16";
        else if (std::is_same_v<T, uint32_t>)
            cast = "UInt32";
        else if (std::is_same_v<T, uint64_t>)
            cast = "UInt64";
        else if (std::is_same_v<T, float>)
            cast = "Float32";
        else if (std::is_same_v<T, double>)
            cast = "Float64";
        else if (std::is_same_v<T, nullptr_t>)
        {
            cast = "Ref{Int64}";
        }

        std::stringstream code;
        State::free_reference(_value);
        State::free_reference(reinterpret_cast<jl_value_t*>(_type));

        //auto* c_symbol = jl_symbol_name_(_symbol);
        //code << c_symbol << " = " << cast << "(" << value << ")" << std::endl;

        _value = State::script(code.str());
        _type = get_type(_value);

        State::create_reference(_value);
        State::create_reference(reinterpret_cast<jl_value_t*>(_type));

        return *this;
    }

    Primitive& Primitive::operator=(bool v)
    {
        assign<bool>(v);
        return *this;
    }

    Primitive& Primitive::operator=(char v)
    {
        assign<char>(v);
        return *this;
    }

    Primitive& Primitive::operator=(float v)
    {
        assign<float>(v);
        return *this;
    }

    Primitive& Primitive::operator=(double v)
    {
        assign<double>(v);
        return *this;
    }

    Primitive& Primitive::operator=(uint8_t v)
    {
        assign<uint8_t>(v);
        return *this;
    }

    Primitive& Primitive::operator=(uint16_t v)
    {
        assign<uint16_t>(v);
        return *this;
    }

    Primitive& Primitive::operator=(uint32_t v)
    {
        assign<uint32_t>(v);
        return *this;
    }

    Primitive& Primitive::operator=(uint64_t v)
    {
        assign<uint64_t>(v);
        return *this;
    }

    Primitive& Primitive::operator=(int8_t v)
    {
        assign<int8_t>(v);
        return *this;
    }

    Primitive& Primitive::operator=(int16_t v)
    {
        assign<int16_t>(v);
        return *this;
    }

    Primitive& Primitive::operator=(int32_t v)
    {
        assign<int32_t>(v);
        return *this;
    }

    Primitive& Primitive::operator=(int64_t v)
    {
        assign<int64_t>(v);
        return *this;
    }

    Primitive& Primitive::operator=(nullptr_t v)
    {
        assign<nullptr_t>(v);
        return *this;
    }
     */
}