// 
// Copyright 2022 Clemens Cords
// Created on 12.01.22 by clem (mail@clemens-cords.com)
//

namespace jluna
{
    template<Boxable T, size_t R>
    Array<T, R>::Array(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>& owner, jl_sym_t* symbol)
        : Proxy<State>(value, owner, symbol)
    {}

}