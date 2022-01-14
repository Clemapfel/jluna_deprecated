// 
// Copyright 2022 Clemens Cords
// Created on 11.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>

#include <memory>
#include <deque>

#include <box_any.hpp>
#include <unbox_any.hpp>

namespace jluna
{
    /// @brief concept that requires a value to be unboxed from a julia-side value
    template<typename T>
    concept Unboxable = requires(T t, jl_value_t* v)
    {
        {unbox<T>(v)};
    };

    /// @brief concept that requires a value to be boxable into a julia-side value
    template<typename T>
    concept Boxable = requires(T t)
    {
        {box(t)};
    };

    /// @brief holds ownership of julia-side value, if attached to a symbol, can be made mutable so it also modifies a julia-side variable
    template<typename State_t>
    class Proxy
    {
        friend union State;
        protected: class ProxyValue;

        public:

            /// @brief construct with no owner, reserved for global temporaries and main
            /// @param value
            /// @param symbol
            Proxy(jl_value_t* value, jl_sym_t* symbol);

            /// @brief construct with no owner, reserved for global temporaries and main
            /// @param value
            /// @param owner: shared pointer to owner, get's incremented
            /// @param symbol
            Proxy(jl_value_t* value, std::shared_ptr<ProxyValue>& owner, jl_sym_t* symbol);

            /// @brief dtor
            ~Proxy() = default;

            /// @brief access field
            /// @param field_name
            /// @returns field as proxy
            virtual Proxy<State_t> operator[](const std::string& field);

            /// @brief access field
            /// @param field_name
            /// @returns unboxed value
            template<Unboxable T>
            T operator[](const std::string& field);

            /// @brief access via linear index, if array type returns getindex! result
            /// @param index
            /// @returns field as proxy
            auto operator[](size_t);

            /// @brief access via linear index, if array type returns getindex! result
            /// @param index
            /// @returns field as proxy
            template<Unboxable T>
            T operator[](size_t);

            /// @brief cast to jl_value_t
            operator jl_value_t*();

            /// @brief cast to string using julias Base.string
            virtual operator std::string() const;

            /// @brief implicitly convert to T via unboxing
            /// @returns value as T
            template<Unboxable T>
            operator T() const;

            /// @brief implicitly downcast to base
            /// @returns value as T
            template<typename T, std::enable_if_t<std::is_base_of_v<Proxy<State_t>, T>, bool> = true>
            operator T();

            /// @brief equivalent alternative to explicit operator T
            /// @returns value as T
            template<typename T, std::enable_if_t<std::is_base_of_v<Proxy<State_t>, T>, bool> = true>
            T as();

            /// @brief get variable name, if any
            std::string get_name() const;

            /// @brief get field names
            /// @returns vector of strings
            std::vector<std::string> get_field_names() const;

            /// @brief call with any arguments
            /// @tparams Args_t: types of arguments, need to be boxable
            template<Boxable... Args_t>
            auto call(Args_t&&...);

            /// @brief call with any arguments
            /// @tparams Args_t: types of arguments, need to be boxable
            template<Boxable... Args_t>
            auto safe_call(Args_t&&...);

            /// call with arguments and exception forwarding, if proxy is a callable function
            /// @tparams Args_t: types of arguments, need to be boxable
            template<Boxable... Args_t>
            auto operator()(Args_t&&...);

            /// @brief check whether assigning to this proxy will modify values julia-side
            /// @returns true if set as mutating and neither an immutable type, singleton or const variable
            bool is_mutating() const;

            /// @brief set mutating behavior, if true, proxy will mutation julia-side variables if possible
            /// @param bool
            void set_mutating(bool);

            /// @brief check whether a call to set_mutating(true) would result in an exception
            /// @returns bool
            bool can_mutate() const;

            /// @brief assign value to proxy, this modifies the value julia-side
            /// @param jl_value_t*
            /// @returns reference to self
            auto& operator=(jl_value_t*);

            /// @brief assign value to proxy, this modifies the value julia-side
            /// @param T: value
            /// @returns reference to self
            template<Boxable T>
            auto& operator=(T);

        protected:
            struct ProxyValue
            {
                ProxyValue(jl_value_t*, jl_sym_t*);
                ProxyValue(jl_value_t*, std::shared_ptr<ProxyValue>& owner, jl_sym_t*);
                ~ProxyValue();

                jl_value_t* get_field(jl_sym_t*);

                std::shared_ptr<ProxyValue> _owner;
                jl_sym_t* _symbol;
                jl_value_t* _value;
            };

            std::shared_ptr<ProxyValue> _content;

            bool _is_mutating = false;
            std::deque<jl_sym_t*> assemble_name() const;

            // for nicer syntax internally:
            jl_value_t* value();
            jl_sym_t* symbol();
            ProxyValue* owner();
    };

    /// @brief forward proxy after setting to mutating, useful for inline-forwarding
    /// @param proxy
    /// @returns proxy after mutation
    /// @exceptions throws ImmutableVariableException if the proxies underlying julia type cannot be modified
    template<typename Proxy_t>
    inline Proxy_t& make_mutating(Proxy_t& proxy)
    {
        proxy.set_mutating(true);
        return std::forward<Proxy_t&>(proxy);
    }

    /// @brief forward proxy after setting to mutating, useful for inline-forwarding
    /// @param proxy
    /// @returns proxy after mutation
    /// @exceptions throws ImmutableVariableException if the proxies underlying julia type cannot be modified
    template<typename Proxy_t>
    inline Proxy_t& make_mutating(Proxy_t& proxy, const std::string& name)
    {
        proxy.assign_name(name);
        proxy.set_mutating(true);
        return std::forward<Proxy_t&>(proxy);
    }

    /// @brief forward proxy after setting to mutating, useful for inline-forwarding
    /// @param proxy
    /// @returns proxy after mutation
    /// @exceptions throws ImmutableVariableException if the proxies underlying julia type cannot be modified
    template<typename Proxy_t>
    inline Proxy_t make_mutating(Proxy_t&& proxy)
    {
        proxy.set_mutating(true);
        return std::forward<Proxy_t>(proxy);
    }

    /// @brief forward proxy after setting to mutating, useful for inline-forwarding
    /// @param proxy
    /// @returns proxy after mutation
    /// @exceptions throws ImmutableVariableException if the proxies underlying julia type cannot be modified
    template<typename Proxy_t>
    inline Proxy_t make_mutating(Proxy_t&& proxy, const std::string& name)
    {
        proxy.assign_name(name);
        proxy.set_mutating(true);
        return std::forward<Proxy_t>(proxy);
    }

    /// @brief forward proxy after setting to mutating, if this is not possible, simply forward the proxy with no operation
    /// @param proxy
    /// @returns proxy
    /// @exceptions no exceptions are thrown
    template<typename Proxy_t>
    inline decltype(auto) try_make_mutating(Proxy_t& proxy) noexcept
    {
        if (proxy.can_mutate())
            proxy.set_mutating(true);

        return std::forward<Proxy_t>(proxy);
    }
}

#include ".src/proxy.inl"