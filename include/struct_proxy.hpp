// 
// Copyright 2021 Clemens Cords
// Created on 24.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <unordered_map>

#include <julia.h>
#include <state.hpp>
#include <proxy.hpp>

namespace jlwrap
{
    class MutableStruct : public Proxy<State>
    {
        public:
            MutableStruct(jl_value_t* instance);

            auto operator[](std::string) const;

        private:
            std::unordered_map<std::string, size_t> _fieldname_to_index;
    };
}