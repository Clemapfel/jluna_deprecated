//
// Copyright 2021 Clemens Cords
// Created on 15.12.21 by clem (mail@clemens-cords.com)
//

#pragma once

namespace jluna::detail
{
    ///@brief allow julia to load jluna by using C++ #import statement
    const char* include = R"(
        include("../.src/julia/visibility_macros.jl")

        module jluna

            function println(xs...)
               Base.println("[JULIA] ", xs...)
            end

            include("../.src/julia/common.jl")
            include("../.src/julia/exception_handler.jl")
            include("../.src/julia/memory_handler.jl")
            include("../.src/julia/introspection.jl")
            include("../.src/julia/cpp_call.jl")
     end
    )";
}