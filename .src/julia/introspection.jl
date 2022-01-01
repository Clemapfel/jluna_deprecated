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
                if (typeof(current) == Module) && (current != m) && !exists(out, current) && !exists(exclude_roots, current)
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
    searches for function in any module currently loaded, returns vector of all functions with that name
    """
    function get_function(function_name::Symbol) ::Vector{Function}

        candidates = Vector{Function}()

        for m in list_all_modules(; exclude_roots = Vector{Module}())
            if exists(names(m), function_name)
                push!(candidates, m.eval(function_name))
            end
        end

        return candidates
    end
    export get_function

    """
    get all modules for whom Base.isdefined(m, x) returns true
    """
    function get_all_modules_defining(x::Symbol) ::Vector{Module}

        out = Vector{Module}()
        for m in list_all_modules(; exclude_roots = Vector{Module}())
            if exists(names(m), x)
                push!(out, m)
            end
        end

        return out
    end
    export get_all_modules_defining
end
