
module _cppcall

    mutable struct State
        _arguments::Tuple
        _result::Any

        State() = new((), nothing)
    end

    const _library_name = "@RESOURCE_PATH@/libjluna_c_adapter.so"
    _state = Base.Ref{_cppcall.State}(State())

    """
    Wrapper object for unnamed functions, frees function once object is destroyed
    """
    mutable struct UnnamedFunctionProxy

        _id::Symbol
        _f::Function

        function UnnamedFunctionProxy(id::Symbol)

            _id = id
            x = new(id, function (xs...) Main.cppcall(_id, xs...) end)

            finalizer(function (t::UnnamedFunctionProxy)
                ccall((:free_function, _cppcall._library_name), Cvoid, (Csize_t,), hash(t._id))
            end, x)

            return x
        end
    end

    # make function proxy struct callable
    (x::UnnamedFunctionProxy)(xs...) = return x._f(xs...)

    # ctor wrapper for jluna
    new_unnamed_function(s::Symbol) = return UnnamedFunctionProxy(s)

    """
    an exception thrown when trying to invoke cppcall with a function name that
    has not yet been registered via jluna::register_function
    """
    mutable struct UnregisteredFunctionNameException <: Exception

        _function_name::Symbol
    end
    Base.showerror(io::IO, e::UnregisteredFunctionNameException) = print(io, "cppcall.UnregisteredFunctionNameException: no C++ function with name :" * string(e._function_name) * " registered")

    """
    an exception thrown when the number of arguments does not match the number of arguments
    expected by the registered lambda
    """
    mutable struct TupleSizeMismatchException <: Exception

        _function_name::Symbol
        _expected::Int64
        _got::Int64
    end
    Base.showerror(io::IO, e::TupleSizeMismatchException) = print(io, "cppcall.TupleSizeMismatchException: C++ function with name :" * string(e._function_name) * " expects " * string(e._expected) * " arguments but was called with " * string(e._got))

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

    """
    `verify_library() -> Bool`

    check if c_adapter library is available
    """
    function verify_library() ::Bool

        if isfile(_cppcall._library_name)
            return true
        end

        message = "when trying to initialize jluna.cppcall: "
        message *= "cannot find " * _cppcall._library_file

        println(sprint(Base.showerror, AssertionError(message), backtrace()))
        return false
    end
end

"""
`cppall(::Symbol, ::Any...) -> Any`

Call a lambda registered via `jluna::State::register_function` using `xs...` as arguments.
After the C++-side function returns, return the resulting object
(or `nothing` if the C++ function returns `void`)

This function is not thread-safe and should not be used in a parallel context
"""
function cppcall(function_name::Symbol, xs...) ::Any

    id = hash(function_name)

    if !ccall((:is_registered, _cppcall._library_name), Bool, (Csize_t,), id)
        throw(_cppcall.UnregisteredFunctionNameException(function_name))
    end

    n_args_expected = ccall((:get_n_args, _cppcall._library_name), Csize_t, (Csize_t,), id)
    if length(xs) != n_args_expected
        throw(_cppcall.TupleSizeMismatchException(function_name, n_args_expected, length(xs)))
    end

    _cppcall.set_arguments(xs...)
    _cppcall.set_result(nothing)

    ccall((:call_function, _cppcall._library_name), Cvoid, (Csize_t,), id)

    return _cppcall.get_result()
end
export cppcall
