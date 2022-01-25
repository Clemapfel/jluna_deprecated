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

        public: class ProxyValue;

        public:
            Proxy() = default;

            /// @brief construct with no owner, reserved for global temporaries and main
            /// @param value
            /// @param symbol
            Proxy(jl_value_t* value, jl_sym_t* symbol = nullptr);

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
            template<Unboxable T, std::enable_if_t<not std::is_same_v<T, std::string>, bool> = true>
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

            /// @brief if proxy is a value, get fieldnames of typeof(value), if proxy is a type, get fieldnames of itself
            /// @returns vector of strings
            std::vector<std::string> get_field_names() const;

            /// @brief get type
            /// @returns proxy to singleton type
            auto get_type() const;

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

            /// @brief assign value to proxy, this modifies the value julia-side
            /// @param jl_value_t*
            /// @returns reference to self
            auto& operator=(jl_value_t*);

            /// @brief assign value to proxy, this modifies the value julia-side
            /// @param T: value
            /// @returns reference to self
            template<Boxable T>
            auto& operator=(T);

            /// @brief create a new unnamed proxy that holds the same value
            /// @returns decltype(this)
            auto value() const;

            /// @brief update value if proxy symbol was reassigned outside of operator=
            void update();

        //protected:
            class ProxyValue
            {
                friend class Proxy<State_t>;

                public:
                    ProxyValue(jl_value_t*, jl_sym_t*);
                    ProxyValue(jl_value_t*, std::shared_ptr<ProxyValue>& owner, jl_sym_t*);
                    ~ProxyValue();

                    jl_value_t* get_field(jl_sym_t*);

                    std::shared_ptr<ProxyValue> _owner;

                    jl_value_t* value();
                    jl_value_t* symbol();

                    size_t value_key();
                    size_t symbol_key();

                    const jl_value_t* value() const;
                    const jl_value_t* symbol() const;

                    const bool _is_mutating = true;

                private:
                    size_t _symbol_key;
                    size_t _value_key;

                    jl_value_t* _symbol_ref;
                    jl_value_t* _value_ref;
            };

            std::shared_ptr<ProxyValue> _content;
            std::deque<jl_sym_t*> assemble_name() const;
    };

    template<typename Proxy_t>
    inline Proxy_t make_mutating(Proxy_t&& proxy, const std::string& name)
    {
        //TODO
    }

    template<typename Proxy_t>
    inline Proxy_t make_non_mutating(Proxy_t&& proxy)
    {
        //TODO
    }
}

#include ".src/proxy.inl"