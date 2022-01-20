// 
// Copyright 2022 Clemens Cords
// Created on 18.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <julia.h>
#include <functional>
#include <unordered_map>
#include <memory>
#include <string>
#include <cstdarg>
#include <proxy.hpp>

namespace jluna
{
    namespace cppcall
    {
        using ID = size_t;
        using Function_t = std::function<jl_value_t*(jl_value_t*, int count)>;

        std::unordered_map<ID, std::unique_ptr<Function_t>> _functions;

        template<typename Lambda_t>
        void register_function(std::string id, Lambda_t&& lambda)
        {
            _functions.insert({std::hash<std::string>{}(id), std::make_unique<Function_t>(lambda)});
        }

        template<typename... Ts>
        auto call_function(const std::string& id, Ts... in)
        {
            std::vector<jl_value_t*> args;
            args.reserve(sizeof...(Ts));
            (args.push_back(in), ...);

            _functions.at(std::hash<std::string>{}(id))->operator()(args[0], args.size());
        }
    }
}

//#include ".src/cpp_call.hpp"