// 
// Copyright 2022 Clemens Cords
// Created on 12.01.22 by clem (mail@clemens-cords.com)
//

namespace jluna
{
    template<Boxable V, size_t R>
    Array<V, R>::Array(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>& owner, jl_sym_t* symbol)
        : Proxy<State>(value, owner, symbol)
    {}
    
    template<Boxable V, size_t R>
    template<Unboxable T>
    T Array<V, R>::operator[](size_t i) const
    {
        return unbox<T>(jl_arrayref((jl_array_t*) _content->_value, i));
    }

    template<Boxable V, size_t R>
    auto Array<V, R>::operator[](size_t i)
    {
        return Proxy<State>(jl_arrayref((jl_array_t*) _content->_value, i), _content, jl_symbol(("[" + std::to_string(i) + "]").c_str()));
    }

    template<Boxable V, size_t R>
    template<Unboxable T, typename... Args, std::enable_if_t<sizeof...(Args) == R and (std::is_integral_v<Args> and ...), bool>>
    T Array<V, R>::at(Args... in) const
    {
        static jl_function_t* getindex = jl_get_function(jl_base_module, "getindex");
        std::vector<jl_value_t*> indices = {jl_box_uint64(in)...};
        auto* res = jl_call(getindex, indices.data(), indices.size());
        forward_last_exception();

        return unbox<T>(res);
    }

    template<Boxable V, size_t R>
    template<typename... Args, std::enable_if_t<sizeof...(Args) == R and (std::is_integral_v<Args> and ...), bool>>
    auto Array<V, R>::at(Args... in)
    {
        static jl_function_t* getindex = jl_get_function(jl_base_module, "getindex");
        std::vector<jl_value_t*> boxed = {jl_box_uint64(in)...};
        auto* res = jl_call(getindex, boxed.data(), boxed.size());
        forward_last_exception();

        std::vector<size_t> indices = {in...};
        std::stringstream symbol;
        symbol << "[";
        for (size_t i = 0; i < indices.size(); ++i)
            symbol << indices.at(i) << (i != indices.size() - 1 ? ", " : "");

        symbol << "]";

        return Proxy<State>(res, _content, jl_symbol(symbol.str().c_str()));
    }

    template<Boxable V, size_t R>
    auto Array<V, R>::front()
    {
        return operator[](0);
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
        return operator[](jl_unbox_uint64(jl_call1(length, _content->_value)));
    }

    template<Boxable V, size_t R>
    template<Unboxable T>
    T Array<V, R>::back() const
    {
        static jl_function_t* length = jl_get_function(jl_base_module, "length");
        return operator[]<T>(jl_unbox_uint64(jl_call1(length, _content->_value)));
    }

    template<Boxable V, size_t R>
    size_t Array<V, R>::get_n_elements() const
    {
        static jl_function_t* length = jl_get_function(jl_base_module, "length");
        return jl_unbox_uint64(jl_call1(length, _content->_value));
    }

    template<Boxable V, size_t R>
    template<std::enable_if_t<R == 1, bool>>
    void Array<V, R>::insert(size_t pos, V value)
    {
        static jl_value_t* insert = jl_get_function(jl_base_module, "insert!");
        jl_call3(insert, value(), jl_box_uint64(pos), box(value));
        forward_last_exception();
    }

    template<Boxable V, size_t R>
    template<std::enable_if_t<R == 1, bool>>
    void Array<V, R>::erase(size_t pos)
    {
        static jl_value_t* deleteat = jl_get_function(jl_base_module, "deleteat!");
        jl_call2(deleteat, value(), jl_box_uint64(pos));
        forward_last_exception();
    }

    template<Boxable V, size_t R>
    template<std::enable_if_t<R == 1, bool>>
    void Array<V, R>::push_front(V value)
    {
        static jl_value_t* pushfirst = jl_get_function(jl_base_module, "pushfirst!");
        jl_call2(pushfirst, value(), box(value));
        forward_last_exception();
    }

    template<Boxable V, size_t R>
    template<std::enable_if_t<R == 1, bool>>
    void Array<V, R>::push_back(V value)
    {
        static jl_value_t* push = jl_get_function(jl_base_module, "push!");
        jl_call2(push, value(), box(value));
        forward_last_exception();
    }

}