// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#include <sstream>

#include <exceptions.hpp>
#include <proxy.hpp>
#include <state.hpp>

/*
namespace jluna
{
    template<typename T, size_t R>
    inline jl_value_t* box(Array<T, R> value)
    {
        return (jl_value_t*) value.operator jl_array_t*();
    }

    template<typename T, size_t R>
    inline jl_value_t* box(Array<T, R>& value)
    {
        return (jl_value_t*) value.operator jl_array_t*();
    }

    template<Boxable T, size_t R>
    Array<T, R>::Array(jl_value_t* value, jl_value_t* owner, jl_sym_t* symbol)
        : Proxy<State>(value, owner, symbol)
    {
        THROW_IF_UNINITIALIZED;
        assert(jl_isa(value, (jl_value_t*) jl_array_type) && "value being bound is not an array");
    }

    template<Boxable T, size_t R>
    Array<T, R>::Array(jl_array_t* value, jl_value_t* owner, jl_sym_t* symbol)
        : Array((jl_value_t*) value, owner, symbol)
    {}

    template<Boxable T, size_t R>
    Array<T, R>::operator jl_array_t*()
    {
        return (jl_array_t*) _content;
    }

    template<Boxable T, size_t R>
    auto Array<T, R>::operator[](size_t i)
    {
        return NonConstIterator((jl_array_t*) _content, i);
    }

    template<Boxable T, size_t R>
    const auto Array<T, R>::operator[](size_t i) const
    {
        return ConstIterator((jl_array_t*) _content, i);
    }

    template<Boxable T, size_t R>
    auto Array<T, R>::begin()
    {
        return NonConstIterator((jl_array_t*) _content, 0);
    }

    template<Boxable T, size_t R>
    auto Array<T, R>::begin() const
    {
        return ConstIterator((jl_array_t*) _content, 0);
    }

    template<Boxable T, size_t R>
    auto Array<T, R>::end()
    {
        return NonConstIterator((jl_array_t*) _content, reinterpret_cast<jl_array_t*>(_content)->length);
    }

    template<Boxable T, size_t R>
    auto Array<T, R>::end() const
    {
        return ConstIterator((jl_array_t*) _content, reinterpret_cast<jl_array_t*>(_content)->length);
    }

    template<Boxable T, size_t Rank>
    size_t Array<T, Rank>::get_dimension(size_t dimension)
    {
        THROW_IF_UNINITIALIZED;
        static auto* size_at = jl_get_function(jl_base_module, "size");
        return jl_unbox_int64(jl_call2(size_at, (jl_value_t*) _content, jl_box_int64(dimension + 1)));
    }

    template<Boxable T, size_t Rank>
    void Array<T, Rank>::throw_if_index_out_of_range(int index, size_t dimension)
    {
        if (index < 0)
        {
            std::stringstream str;
            str << "negative index " << index << ", only indices >= 0 are permitted" << std::endl;
            throw std::out_of_range(str.str().c_str());
        }

        size_t dim = get_dimension(dimension);

        std::string dim_id;

        if (dimension == 0)
            dim_id = "1st dimension";
        else if (dimension == 1)
            dim_id = "2nd dimension";
        else if (dimension == 3)
            dim_id = "3rd dimension";
        else if (dimension < 11)
            dim_id = std::to_string(dimension) + "th dimension";
        else
            dim_id = "dimension " + std::to_string(dimension);

        if (index >= dim)
        {
            std::stringstream str;
            str << "index " << index << " out of range for array of length " << dim << " along " << dim_id << ". Indices C++ side are 0-based" << std::endl;
            throw std::out_of_range(str.str().c_str());
        }
    }

    template<Boxable T, size_t Rank>
    template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool>>
    auto Array<T, Rank>::at(Args... in)
    {
        {
            size_t i = 0;
            (throw_if_index_out_of_range(in, i++), ...);
        }

        std::array<size_t, Rank> indices = {size_t(in)...};
        size_t index = 0;
        size_t mul = 1;

        for (size_t i = 0; i < Rank; ++i)
        {
            index += (indices.at(i)) * mul;
            size_t dim = get_dimension(i);
            mul *= dim;
        }

        return operator[](index);
    }

    template<Boxable T, size_t Rank>
    template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool>>
    const auto Array<T, Rank>::at(Args... in) const
    {
        {
            size_t i = 0;
            (throw_if_index_out_of_range(in, i++), ...);
        }

        std::array<size_t, Rank> indices = {size_t(in)...};
        size_t index = 0;
        size_t mul = 1;

        for (size_t i = 0; i < Rank; ++i)
        {
            index += (indices.at(i)) * mul;
            size_t dim = get_dimension(i);
            mul *= dim;
        }

        return operator[](index);
    }
}
 */