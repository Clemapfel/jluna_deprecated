#
# Copyright 2021 Clemens Cords
# Created on 17.12.21 by clem (mail@clemens-cords.com)
#

"""
internal julia-side functions for jlwrap C++ binding
"""
module jlwrap

    """
    list all submodules in Main

    @returns array of modules
    """
    function list_submodules() ::Array{Module, 1}

        return map(x -> convert(Module, x), filter(x->isa(x, Module), map(x -> eval(x), names(Main))))
    end

    """
    add expression to already existing module

    @param m: module
    @param expr: expression
    """
    function add_to_module(m::Module, expr::Expr) ::Nothing

        m.eval(expr)
    end

    """
    get value type of array

    @param _: array of any rank
    @returns value type T
    """
    function get_value_type_of_array(_::Array{T}) where {T}

        return T
    end

    """
    check if method of given function is available for a specific variable

    @param f: function
    @param variable
    @returns true if method is available, false otherwise
    """
    function is_method_available(f::Function, variable) ::Bool

        return hasmethod(f, Tuple{typeof(variable)})
    end

    """
    offers verbose exception interface. Any call with safe_call will store
    the last exception and full stack trace as string in _last_exception and
    _last_message respectively
    """
    module ExceptionHandler

        global _last_message = String("")
        global _last_exception

        """
        call any line of code, update the handler then forward the result, if any

        @param command: julia code as string
        @returns result of expression or nothing if an exception was thrown
        """
        function safe_call(command::String) #::Auto

            as_expression = Meta.parse(command)
            result = undef;
            try
                result = eval(as_expression)
                update()
            catch exc
                result = nothing
                update(exc)
            end

            return result
        end

        """
        update the handler after an exception was thrown
        @param exception
        """
        function update(exception::Exception) ::Nothing

            global _last_message = sprint(Base.showerror, exception, catch_backtrace());
            global _last_exception = exception
            return nothing
        end

        """
        update the handler after *no* exception was thrown
        """
        function update() ::Nothing

            global _last_message = ""
            global _last_exception = nothing
            return nothing
        end

        struct State
            _last_exception
            _last_message::String
        end

        """
        safe the current state of the handler

        @returns jlwrap.ExceptionHandler.State
        """
        function state() ::State

            return State(_last_exception, _last_message)
        end

        function has_exception_occurred() ::Bool

            return _last_exception == Nothing
        end
    end
end