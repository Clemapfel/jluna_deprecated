// 
// Copyright 2022 Clemens Cords
// Created on 12.01.22 by clem (mail@clemens-cords.com)
//

namespace jluna
{
    template<Boxable V, size_t R>
    Array<V, R>::Array(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>& owner, jl_sym_t* symbol)
        : Proxy<State>(value, owner, symbol)
    {
        assert_type(value, "Array");
    }

    template<Boxable V, size_t R>
    Array<V, R>::Array(jl_value_t* value, jl_sym_t* symbol)
        : Proxy<State>(value, symbol)
    {
        assert_type(value, "Array");
    }

    /*
    template<Boxable V, size_t R>
    template<Boxable T>
    Array<V, R>::Array(const std::vector<T>& value, jl_sym_t* symbol)
        : Proxy<State>(box(value), symbol)
    {}
     */

    template<Boxable T, size_t Rank>
    size_t Array<T, Rank>::get_dimension(int index)
    {
        static jl_function_t* size = jl_get_function(jl_base_module, "size");
        return jl_unbox_uint64(jl_call2(size, _content->value(), jl_box_int32(index + 1)));
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
            str << "0-based index " << index << " out of range for array of length " << dim << " along " << dim_id << std::endl;
            throw std::out_of_range(str.str().c_str());
        }
    }
    
    template<Boxable V, size_t R>
    template<Unboxable T>
    T Array<V, R>::operator[](size_t i) const
    {
        if (i >= get_n_elements())
        {
            std::stringstream str;
            str << "0-based index " << i << " out of range for array of length " << get_n_elements() << std::endl;
            throw std::out_of_range(str.str().c_str());
        }

        return unbox<T>(jl_arrayref((jl_array_t*) _content->value(), i));
    }

    template<Boxable V, size_t R>
    auto Array<V, R>::operator[](size_t i)
    {
        if (i >= get_n_elements())
        {
            std::stringstream str;
            str << "0-based index " << i << " out of range for array of length " << get_n_elements() << std::endl;
            throw std::out_of_range(str.str().c_str());
        }

        return Iterator(i, this);
    }

    template<Boxable V, size_t R>
    template<Unboxable T, typename... Args, std::enable_if_t<sizeof...(Args) == R and (std::is_integral_v<Args> and ...), bool>>
    T Array<V, R>::at(Args... in) const
    {
        {
            size_t i = 0;
            (throw_if_index_out_of_range(in, i++), ...);
        }

        std::array<size_t, R> indices = {size_t(in)...};
        size_t index = 0;
        size_t mul = 1;

        for (size_t i = 0; i < R; ++i)
        {
            index += (indices.at(i)) * mul;
            size_t dim = get_dimension(i);
            mul *= dim;
        }

        return operator[](index);
    }

    template<Boxable V, size_t R>
    template<typename... Args, std::enable_if_t<sizeof...(Args) == R and (std::is_integral_v<Args> and ...), bool>>
    auto Array<V, R>::at(Args... in)
    {
        {
            size_t i = 0;
            (throw_if_index_out_of_range(in, i++), ...);
        }

        std::array<size_t, R> indices = {size_t(in)...};
        size_t index = 0;
        size_t mul = 1;

        for (size_t i = 0; i < R; ++i)
        {
            index += (indices.at(i)) * mul;
            size_t dim = get_dimension(i);
            mul *= dim;
        }

        return operator[](index);
    }

    template<Boxable V, size_t R>
    template<Boxable T>
    void Array<V, R>::set(size_t i, T value)
    {
        jl_value_t* boxed = box(value);
        jl_arrayset((jl_array_t*) _content->value(), box<V>(value), i);
    }

    template<Boxable V, size_t R>
    auto Array<V, R>::front()
    {
        return operator[](0);
    }

    template<Boxable V, size_t R>
    auto Array<V, R>::begin()
    {
        return Iterator(0, this);
    }

    template<Boxable V, size_t R>
    auto Array<V, R>::begin() const
    {
        return ConstIterator(0, this);
    }

    template<Boxable V, size_t R>
    auto Array<V, R>::end()
    {
        return Iterator(get_n_elements(), this);
    }

    template<Boxable V, size_t R>
    auto Array<V, R>::end() const
    {
        return ConstIterator(get_n_elements(), this);
    }

    template<Boxable V, size_t R>
    template<Unboxable T>
    T Array<V, R>::front() const
    {
        return operator[](0);
    }

    template<Boxable V, size_t R>
    auto Array<V, R>::back()
    {
        static jl_function_t* length = jl_get_function(jl_base_module, "length");
        return operator[](jl_unbox_uint64(jl_call1(length, _content->value())));
    }

    template<Boxable V, size_t R>
    template<Unboxable T>
    T Array<V, R>::back() const
    {
        static jl_function_t* length = jl_get_function(jl_base_module, "length");
        return operator[]<T>(jl_unbox_uint64(jl_call1(length, _content->value())));
    }

    template<Boxable V, size_t R>
    size_t Array<V, R>::get_n_elements() const
    {
        static jl_function_t* length = jl_get_function(jl_base_module, "length");
        return jl_unbox_uint64(jl_call1(length, _content->value()));
    }

    // ###

    template<Boxable V>
    Vector<V>::Vector(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>& owner, jl_sym_t* symbol)
        : Array<V, 1>(value, owner, symbol)
    {
        assert_type(value, "Vector");
    }

    template<Boxable V>
    Vector<V>::Vector(jl_value_t* value, jl_sym_t* symbol)
        : Array<V, 1>(value, symbol)
    {
        assert_type(value, "Vector");
    }

    template<Boxable V>
    void Vector<V>::insert(size_t pos, V value)
    {
        static jl_value_t* insert = jl_get_function(jl_base_module, "insert!");
        jl_call3(insert, Array<V, 1>::value(), jl_box_uint64(pos), box(value));
        forward_last_exception();
    }

    template<Boxable V>
    void Vector<V>::erase(size_t pos)
    {
        static jl_value_t* deleteat = jl_get_function(jl_base_module, "deleteat!");
        jl_call2(deleteat, Array<V, 1>::value(), jl_box_uint64(pos));
        forward_last_exception();
    }

    template<Boxable V>
    template<Boxable T>
    void Vector<V>::push_front(T value)
    {
        static jl_value_t* pushfirst = jl_get_function(jl_base_module, "pushfirst!");
        jl_call2(pushfirst, Array<V, 1>::value(), box(value));
        forward_last_exception();
    }

    template<Boxable V>
    template<Boxable T>
    void Vector<V>::push_back(T value)
    {
        static jl_value_t* push = jl_get_function(jl_base_module, "push!");
        jl_call2(push, Array<V, 1>::value(), box(value));
        forward_last_exception();
    }
}