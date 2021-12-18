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
    jl_datatype_t * PrimitiveProxy::get_type(jl_value_t* v)
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

    PrimitiveProxy::PrimitiveProxy(jl_sym_t* symbol, jl_value_t* value)
        : Proxy(symbol, value, get_type(value))
    {}


    template<typename T>
    T PrimitiveProxy::cast_to() const
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

    PrimitiveProxy::operator bool()
    {
        return cast_to<bool>();
    }

    PrimitiveProxy::operator char()
    {
        return cast_to<char>();
    }

    PrimitiveProxy::operator float()
    {
        return cast_to<float>();
    }

    PrimitiveProxy::operator double()
    {
        return cast_to<double>();
    }

    PrimitiveProxy::operator uint8_t()
    {
        return cast_to<uint8_t>();
    }

    PrimitiveProxy::operator uint16_t()
    {
        return cast_to<uint16_t>();
    }

    PrimitiveProxy::operator uint32_t()
    {
        return cast_to<uint32_t>();
    }

    PrimitiveProxy::operator uint64_t()
    {
        return cast_to<uint64_t>();
    }

    PrimitiveProxy::operator int8_t()
    {
        return cast_to<int8_t>();
    }

    PrimitiveProxy::operator int16_t()
    {
        return cast_to<int16_t>();
    }

    PrimitiveProxy::operator int32_t()
    {
        return cast_to<int32_t>();
    }

    PrimitiveProxy::operator int64_t()
    {
        return cast_to<int64_t>();
    }

    template<typename T>
    PrimitiveProxy & PrimitiveProxy::assign(T value)
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

        auto* c_symbol = jl_symbol_name_(_symbol);
        code << c_symbol << " = " << cast << "(" << value << ")" << std::endl;

        _value = State::script(code.str());
        _type = get_type(_value);

        State::create_reference(_value);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(bool v)
    {
        assign<bool>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(char v)
    {
        assign<char>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(float v)
    {
        assign<float>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(double v)
    {
        assign<double>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(uint8_t v)
    {
        assign<uint8_t>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(uint16_t v)
    {
        assign<uint16_t>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(uint32_t v)
    {
        assign<uint32_t>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(uint64_t v)
    {
        assign<uint64_t>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(int8_t v)
    {
        assign<int8_t>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(int16_t v)
    {
        assign<int16_t>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(int32_t v)
    {
        assign<int32_t>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(int64_t v)
    {
        assign<int64_t>(v);
    }

    PrimitiveProxy& PrimitiveProxy::operator=(nullptr_t v)
    {
        assign<nullptr_t>(v);
    }
}