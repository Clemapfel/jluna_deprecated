// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <unordered_map>
#include <type_proxy.hpp>

namespace jlwrap
{
    template<typename T>
    concept Unboxable = requires(T t, jl_value_t* v)
    {
        {unbox<T>(v)};
    };

    template<typename T>
    concept Boxable = requires(T t, jl_value_t* v)
    {
        {box(v)};
    };

    /// @brief concept of a variable, has a type, name (symbol) and value
    /// @note do not inherit from this class unless you know what you're doing, improper handling of references to julia-side values can result in memory leaks or the garbage collector freeing in-use memory
    template<typename State_t>
    class Proxy
    {
        public:
            /// @brief ctor deleted
            Proxy() = delete;

            /// @brief construct, adds a reference that holds ownership of the value to protect it from the garbage collection
            /// @param value: pointer to the value of the variable
            Proxy(jl_value_t* value);

            /// @brief construct as proxy of a field of an owner
            /// @param value: pointer to the value of the variable
            Proxy(jl_value_t* value, jl_value_t* owner, size_t field_i);

            /// @brief dtor, frees the reference so it can be garbage collected if appropriate
            ~Proxy();

            /// @brief copy ctor, both resulting objects have ownership and the value will only be deallocated, if the number of owners is exactly 0
            /// @param other
            Proxy(const Proxy&);

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
            virtual explicit operator std::string();

            /// @brief implicitly convert to T via unboxing
            /// @returns value as T
            template<Unboxable T>
            operator T();

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

            /// @brief type
            const Type type = Type((jl_value_t*) jl_nothing_type);

        //protected:
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

            jl_value_t* _value;

        private:
            jl_value_t* _owner = nullptr;
            long int _field_i = -1;

            void setup_field_to_index();
            std::unordered_map<std::string, size_t> _field_to_index;
    };
}

#include ".src/proxy.inl"