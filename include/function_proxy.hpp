// 
// Copyright 2021 Clemens Cords
// Created on 19.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <proxy.hpp>

namespace jlwrap
{
    union State;

    class Function : protected Proxy<State>
    {
        public:
            Function() = delete;

            template<typename... Arg_ts>
            decltype(auto) operator()(Arg_ts...);

        protected:
            Function(jl_function_t*);

        private:
            using Proxy::_value;
    };
}