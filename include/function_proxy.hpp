// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <state.hpp>
#include <proxy.hpp>

namespace jluna
{
    namespace detail
    {
        class FunctionProxy : public Proxy<State>
        {
            public:
                operator jl_function_t*() noexcept;

            protected:
                FunctionProxy(jl_function_t*);

                using Proxy<State>::_value;
        };
    }

    class SafeFunction : protected detail::FunctionProxy
    {
        public:
            SafeFunction(jl_function_t*);
            using detail::FunctionProxy::operator jl_function_t*;

            template<typename... Args_t>
            auto operator()(Args_t&&...);

        private:
            using detail::FunctionProxy::_value;
    };

    class Function : protected detail::FunctionProxy
    {
        public:
            Function(jl_function_t*);
            using detail::FunctionProxy::operator jl_function_t*;

            template<typename... Args_t>
            auto operator()(Args_t&&...);

        private:
            using detail::FunctionProxy::_value;
    };
}

#include ".src/function_proxy.inl"