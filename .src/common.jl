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
    offers verbose exception interface

    Example:
        try
            # function call here
            jlwrap.ExceptionHandler.update()
        catch x
            jlwrap.ExceptionHandler.update(x)
        end
    """
    module ExceptionHandler

        global _last_message = String("")
        global _occurred = Bool(false)
        global _type

        """
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
        """
        function update(exception::Exception) ::Nothing
            global _last_message = sprint(Base.showerror, exception, catch_backtrace());
            global _occurred = true
            global _type = typeof(exception)
            return nothing
        end

        """
        """
        function update() ::Nothing
            global _last_message = ""
            global _occurred = false
            global _type = Nothing
            return nothing
        end
    end
end