// 
// Copyright 2022 Clemens Cords
// Created on 14.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

namespace jluna
{
    /// concept: can From be static cast to To
    template<typename From, typename To>
    concept CastableTo = requires(From t)
    {
        {static_cast<To>(t)};
    };

    /// concept: iterable range
    template<typename T>
    concept Iterable = requires(T t)
    {
        {t.begin()};
        {t.end()};
        typename T::value_type;
    };

    // concept: has value type
    template<typename T>
    concept HasValueType = requires(T t)
    {
        typename T::value_type;
    };

    /// get sizeof array
    /// reference: https://brevzin.github.io/c++/2020/02/05/constexpr-array-size/
    template<HasValueType Array_t>
    struct array_size
    {
        static inline constexpr size_t value = sizeof(Array_t) / sizeof(typename Array_t::value_type);
    };
}

