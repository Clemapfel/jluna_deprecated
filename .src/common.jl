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
end