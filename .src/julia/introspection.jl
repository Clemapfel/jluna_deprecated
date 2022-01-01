#
# Copyright 2022 Clemens Cords
# Created on 01.01.2022 by clem (mail@clemens-cords.com)
#

begin

    """
    recursively list all modules and submodules, excluding those that are a submodule of a module in exclude_roots
    """
    function list_all_modules(top::Module = Main; exclude_roots::Vector{Module} = [Base, Core, Main.InteractiveUtils]) ::Vector{Module}

        out = Vector{Module}()

        function aux(m::Module)

            for symbol in names(m)

                current = Base.eval(m, symbol)
                counter = counter + 1
                if (typeof(current) == Module) && (current != m) && !exists(current, out) && !exists(current, exclude_roots)
                    push!(out, current)
                    aux(current)
                end
            end
        end

        aux(top)
        return out
    end
    export list_all_modules

    """
    access function in specific module
    """
    function get_function(m::Module, function_name::Symbol) ::Function

        if !isdefined(m, function_name)
            throw(UndefVarError("No function named " * string(function_name) * " in module " * string(m)))
        end

        return m.eval(function_name)
    end
    export get_function

    """
    searches for function in any module currently loaded, returns vector of all functions with that name
    """
    function get_function(function_name::Symbol, top::Module = Main) ::Vector{Function}

        candidates = Vector{Function}()

        for m in list_all_modules(jlwrap; exclude_roots = Vector{Module}())
            if (isdefined(m, function_name))
                push!(candidates, m.eval(function_name))
            end
        end

        return candidates
    end
    export get_function

end
