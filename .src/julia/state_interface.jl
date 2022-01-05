#
# Copyright 2022 Clemens Cords
# Created on 01.01.2022 by clem (mail@clemens-cords.com)
#

begin # state wrappers for exception-safe calling of c-functions

    """
    exception-safe function access wrapper
    """
    function get_function(function_name::Symbol, m::Module) ::Function

        return m.eval(function_name)
    end
    export get_function

    mutable struct Assignment
        _module::Module
        _symbol::Symbol
        _value::Any
    end

    """
    """
    function forward_assignment(expr::Expr) ::Assignment

        function aux(expr::Expr)

        end

        out = Assignment(Main, Symbol(""), nothing)

        eval(Meta.parse(expr))
    end

end
