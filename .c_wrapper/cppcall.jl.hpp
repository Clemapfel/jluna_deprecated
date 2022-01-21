#pragma once

namespace c_adapter
{
    const char* cppcall_module = R"(
"""
helper module used by jluna c_adapter and cppcall
"""
module _cppcall

    mutable struct State
        _arguments::Tuple
_result::Any

        State() = new((), nothing)
    end

    _state = Base.Ref{_cppcall.State}(State())

    """
    `set_result(::Any) -> Nothing`
    
    modify _cppcall state result
    """
    function set_result(x::Any) ::Nothing

        global _cppcall._state[]._result = x
        return nothing
    end
    
    """
    `get_result() -> Any`
    
    access _cppcall result
    """
    function get_result() ::Any

        return _cppcall._state[]._result
    end
    
    """
    `set_arguments(xs...) -> Nothing`
    
    modify _cppcall state argument tuple
    """
    function set_arguments(xs...) ::Nothing

        global _cppcall._state[]._arguments = xs
        return nothing
    end
    
    """
    `get_result() -> Tuple`
    
    access _cppcall state argument tuple
    """
    function get_arguments() ::Tuple

        return _cppcall._state[]._arguments
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

    _cppcall.set_arguments(xs...)
    _cppcall.set_result(nothing)

    ccall((:call_function, "./libjluna_c_adapter.so"), Cvoid, (Csize_t,), hash(function_name))

    return _cppcall.get_result()
end
export cppcall
)";
}
