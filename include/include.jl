#
# Copyright 2021 Clemens Cords
# Created on 26.12.2021 by clem (mail@clemens-cords.com)
#

macro make_public(module_name::Symbol)

    as_module = eval(module_name)
    @assert typeof(as_module) == Module

    for s in names(as_module; all = true)
        if (string(s)[1] != '#')
            as_module.eval(Meta.parse("export " * string(s)))
        end
    end

    return nothing
end

module detail
    efunction make_public_aux(child_name::Symbol, super::Module)

        if (string(child_name)[1] != '#')
            println("export " * string(child_name))
            super.eval(Meta.parse("export " * string(child_name)))
        end

        child = super.eval(child_name)
        if (child isa Module && string(child) != string(super))
            for name in names(child; all = true)
                make_public_aux(name, child)
            end
        end
    end
end

macro make_public_rec(module_name::Symbol)

    origin = eval(module_name)
    @assert origin isa Module

    for name in names(origin; all = true)
        make_public_aux(name, origin)
    end
end

module jlwrap

    include("../.src/julia/visibility_macros.jl")
    include("../.src/julia/common.jl")
    include("../.src/julia/exception_handler.jl")
    include("../.src/julia/memory_handler.jl")
end

#@make_public_rec jlwrap