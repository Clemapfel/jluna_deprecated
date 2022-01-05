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
        /// @brief pure virtual function interface
        class FunctionProxy : public Proxy<State>
        {
            public:
                operator jl_function_t*() noexcept;

            protected:
                FunctionProxy(jl_function_t*);
                using Proxy<State>::_value;
        };
    }

    /// @brief callable function with exception handling
    class SafeFunction : protected detail::FunctionProxy
    {
        public:
            /// @brief attach already existing value
            /// @param julia-function
            SafeFunction(jl_function_t*);

            /// @brief cast to jl_function_t
            /// @returns jl_function_t*
            using detail::FunctionProxy::operator jl_function_t*;

            /// @brief call with any arguments
            /// @tparams Args_t: types of arguments, need to be boxable
            template<Boxable... Args_t>
            auto operator()(Args_t&&...);

        private:
            using detail::FunctionProxy::_value;
    };

    /// @brief callable function without exception handling
    class Function : protected detail::FunctionProxy
    {
        public:
            /// @brief attach already existing value
            /// @param julia-function
            Function(jl_function_t*);

            /// @brief cast to jl_function_t
            /// @returns jl_function_t*
            using detail::FunctionProxy::operator jl_function_t*;

            /// @brief call with any arguments
            /// @tparams Args_t: types of arguments, need to be boxable
            template<Boxable... Args_t>
            auto operator()(Args_t&&...);

        private:
            using detail::FunctionProxy::_value;
    };
}

#include ".src/function_proxy.inl"