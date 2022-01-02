// 
// Copyright 2022 Clemens Cords
// Created on 02.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <proxy.hpp>
#include <state.hpp>

namespace jlwrap
{
    /// @brief proxy with non-const field access
    struct MutableStruct : public Proxy<State>
    {
        public:
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
    };

    /// @brief proxy with only const field access
    struct Struct : public Proxy<State>
    {
        public:
            /// @brief access field but immediately decay into type
            /// @tparam T: type the result will be unbox<T>'d to
            /// @param field_name: exact name of field, as defined julia-side
            /// @returns value as T
            template<typename T>
            T operator[](const std::string& field_name) const;
    };
}