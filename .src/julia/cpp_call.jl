#
# Copyright 2022 Clemens Cords
# Created on 20.01.2022 by clem (mail@clemens-cords.com)
#

#begin # included in module jluna

    module cpp_call

        mutable struct State
            _id::UInt64
            _arguments::Tuple
            _result::Any

            State() = new(0, (nothing,), nothing)
        end

        _state = cpp_call.State();

        function set_result(x::Any) ::Nothing

            global cpp_call._state._result = x;
        end
    end

    """
    `cppcall(::Symbol, ::Any...) -> Any`

    Call a lambda registered via `jluna::State::register_function`
    and forward the `xs...` to it.
    After the C++-side function returns, return the resulting object
    (or `nothing` if the C++ function returns `void`)
    """
    function cppcall(function_name::Symbol, xs...) ::Any

        global cpp_call._state._arguments = tuple(xs...);
        global cpp_call._state._id = hash(function_name);
        global cpp_call._state._result = nothing

        ccall((:call_cpp, "/home/clem/Workspace/jluna/libjluna_c_adapter.so"), Cvoid, (Csize_t,), hash(function_name))
        #ccall((:call_cpp, "/home/clem/Workspace/jluna/libjluna_c.so"), Cvoid, (Csize_t,), UInt64(32));
        return cpp_call._state._result;
    end
    export cppcall
#end
