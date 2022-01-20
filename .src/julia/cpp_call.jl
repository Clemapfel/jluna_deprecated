#
# Copyright 2022 Clemens Cords
# Created on 20.01.2022 by clem (mail@clemens-cords.com)
#

#begin # included in module jluna

    module _CppCall

        mutable struct State
            _arguments::Tuple
            _result::Any

            State() = new((), nothing)
        end

        _state = Base.RefValue(State());
    end

    """
    `cppcall(::Symbol, ::Any...) -> Any`

    Call a lambda registered via `jluna::State::register_function`
    and forward the `xs...` to it.
    After the C++-side function returns, return the resulting object
    (or `nothing` if the C++ function returns `void`)
    """
    function cppcall(function_name::Symbol, xs...) ::Any

        _CppCall._state[]._arguments = xs;
        ccall((:call_cpp, "/home/clem/Workspace/jluna/libjluna_c.so"), Nothing, (UInt64,), hash(function_name));
        return _CppCall._state[]._result;
    end
    export cppcall
#end
