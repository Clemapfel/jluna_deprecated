// 
// Copyright 2021 Clemens Cords
// Created on 20.12.21 by clem (mail@clemens-cords.com)
//

#include <sstream>

#include <array_proxy.hpp>
#include <exceptions.hpp>
#include <proxy.hpp>
#include <unbox_any.hpp>
#include <box_any.hpp>

namespace jlwrap
{
    template<typename T, size_t R>
    Array<T, R>::Array(jl_value_t* value)
        : Proxy<State>(reinterpret_cast<jl_value_t*>(value))
    {
        if (not jl_is_array(value))
        {
            std::stringstream str;
            str << jl_typeof(value) << std::endl;
            assert(false);
        }

        _value = (jl_array_t*) Proxy<State>::operator _jl_value_t *();
    }

    template<typename T, size_t R>
    Array<T, R>::Array(jl_array_t* value)
        : Array((jl_value_t*) value)
    {}

    template<typename T, size_t R>
    Array<T, R>::operator jl_array_t*()
    {
        return _value;
    }

    template<typename T, size_t R>
    auto Array<T, R>::operator[](size_t i)
    {
        return NonConstIterator(_value, i);
    }

    template<typename T, size_t R>
    const auto Array<T, R>::operator[](size_t i) const
    {
        return ConstIterator(_value, i);
    }

    template<typename T, size_t R>
    size_t Array<T, R>::length() const
    {
        return ((jl_array_t*) _value)->length;
    }

    template<typename T, size_t R>
    auto Array<T, R>::begin()
    {
        return Iterator(_value, 0);
    }

    template<typename T, size_t R>
    auto Array<T, R>::begin() const
    {
        return Iterator(_value, 0);
    }

    template<typename T, size_t R>
    auto Array<T, R>::end()
    {
        return Iterator(_value, reinterpret_cast<jl_array_t*>(_value)->length);
    }

    template<typename T, size_t R>
    auto Array<T, R>::end() const
    {
        return Iterator(_value, reinterpret_cast<jl_array_t*>(_value)->length);
    }

    template<typename T, size_t Rank>
    template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool>>
    auto Array<T, Rank>::at(Args... in)
    {
        // check for bounds along each dimensions separately
        auto throw_if_out_of_range = [&](auto index, size_t dimension)
        {
            if (index < 0)
                throw std::out_of_range("negative index, only indices >= 0 permitted");

            static auto* size_at = jl_get_function(jl_base_module, "size");
            size_t dim = unbox<size_t>(jl_call2(size_at, (jl_value_t*) _value, box<size_t>(dimension + 1)));

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
                str << "index " << index << " out of range for array of length " << dim << " along " << dim_id << std::endl;
                throw std::out_of_range(str.str().c_str());
            }
        };

        {
            size_t i = 0;
            (throw_if_out_of_range(in, i++), ...);
        }

        return operator[](0);
    }

    template<typename T, size_t Rank>
    template<typename... Args, std::enable_if_t<sizeof...(Args) == Rank and (std::is_integral_v<Args> and ...), bool>>
    const auto Array<T, Rank>::at(Args... in) const
    {

    }

}