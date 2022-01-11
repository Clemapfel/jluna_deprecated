// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <unordered_map>
#include <box_any.hpp>
#include <unbox_any.hpp>
#include <memory>

namespace jluna
{

    /// @brief concept of a variable, has a type, name (symbol) and value
    /// @note do not inherit from this class unless you know what you're doing, improper handling of references to julia-side values can result in memory leaks or the garbage collector freeing in-use memory
    template<typename State_t>
    class Proxy
    {
        public:
            /// @brief ctor deleted
            Proxy() = delete;

            /// @brief construct as proxy
            Proxy(jl_value_t* value, Proxy<State_t>* owner = nullptr, jl_sym_t* symbol = nullptr);

            /// @brief dtor, frees the reference so it can be garbage collected if appropriate
            ~Proxy();

            /// @brief copy ctor, both resulting objects have ownership and the value will only be deallocated, if the number of owners is exactly 0
            /// @param other
            Proxy(const Proxy&);

            /// @brief check whether assigning to this proxy will modify values julia-side
            /// @returns true if set as mutating and neither an immutable type, singleton or const variable
            bool is_mutating() const;

            /// @brief set mutating behavior, if true, proxy will mutation julia-side variables if possible
            /// @param bool
            void set_mutating(bool);

            /// @brief check whether a call to set_mutating(true) would result in an exception
            /// @returns bool
            bool can_mutate() const;

            /// @brief assign variable a new name, if no name exist, also creates julia-side variable. If name already exists, renames the variable
            void assign_name(const std::string&);

            /// @brief copy ctor, both resulting objects have ownership and the value will only be deallocated, if the number of owners is exactly 0
            /// @param other
            Proxy& operator=(const Proxy&);

            /// @brief move ctor, only *this* will hold ownership
            /// @param other
            Proxy(Proxy&&) noexcept;

            /// @brief move assignment, only *this* will hold ownership
            /// @param other
            Proxy& operator=(Proxy&&) noexcept;

            /// @brief implicitly decay into value so it can be unbox<T>'d
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

            /// @brief assign value to proxy, this modifies the value julia-side
            /// @param T: value
            /// @returns reference to self
            template<Boxable T>
            auto& operator=(T);

            /// @brief assign value to proxy, this modifies the value julia-side
            /// @param jl_value_t*
            /// @returns reference to self
            auto& operator=(jl_value_t*);

            /// @brief check if value is a struct or mutable struct
            /// @returns true if julias Base.isstructtype would return true, false otherwise
            bool is_struct() const;

            /// @brief get fieldnames
            /// @returns dictionary where each key is the fields name, the keys value is the fields index
            const std::set<std::string>& get_fieldnames() const;

             /// @brief access field
            /// @param field_name: exact name of field, as defined julia-side
            /// @returns proxy holding value of field
            auto operator[](const std::string& field_name);

            /// @brief access field but immediately decay into type
            /// @tparam T: type the result will be unbox<T>'d to
            /// @param field_name: exact name of field, as defined julia-side
            /// @returns value as T
            template<typename T>
            T operator[](const std::string& field_name) const;

            /// @brief check if both proxies point to the same instance
            /// @param other
            /// @returns true if julia-side (===) would return true
            bool operator==(const Proxy<State_t>& other) const;

            /// @brief check if both proxies point to the same instance
            /// @param other
            /// @returns false if julia-side (===) would return true
            bool operator!=(const Proxy<State_t>& other) const;

            /// @brief get the name of the variable that would be modified, if the proxy was mutating
            /// @returns string, empty string if temporary value
            std::string get_name() const;

        protected:
            /// @brief access field
            /// @param field_name: exact name of field, as defined julia-side
            /// @returns proxy holding value of field
            auto get_field(const std::string& field_name);

            /// @brief access field but immediately decay into type
            /// @tparam T: type the result will be unbox<T>'d to
            /// @param field_name: exact name of field, as defined julia-side
            /// @returns value as T
            template<typename T>
            T get_field(const std::string& field_name) const;

            jl_value_t* _content;
            bool _is_mutating = false;

        private:
            std::deque<jl_sym_t*> assemble_name() const;

            std::unique_ptr<Proxy<State_t>> _owner = nullptr;
            jl_sym_t* _symbol = nullptr;

            void setup_fields();
            std::set<std::string> _fields;
    };
}

#include ".src/proxy.inl"