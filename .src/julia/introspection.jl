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

                # shortcut operator order relevant for runtime
                if (typeof(current) == Module) && (current != m) && !exists(exclude_roots, current) && !exists(out, current)
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
    function find_function(function_name::Symbol) ::Vector{Function}

        candidates = Vector{Function}()

        # skip likely base/core for better runtime
        if (isdefined(Base, function_name))
            push!(candidates, Base.eval(function_name))
        end

        if (isdefined(Core, function_name))
            push!(candidates, Core.eval(function_name))
        end

        for m in list_all_modules(; exclude_roots = Vector{Module}([Base, Core]))
            if exists(names(m), function_name)
                push!(candidates, m.eval(function_name))
            end
        end

        return candidates
    end
    export find_function

    """
    get all modules for whom Base.isdefined(m, x) returns true
    """
    function get_all_modules_defining(x::Symbol) ::Vector{Module}

        out = Vector{Module}()

        if (isdefined(Base, x))
            push!(out, Base.eval(x))
        end

        if (isdefined(Core, function_name))
            push!(out, Core.eval(x))
        end

        for m in list_all_modules(; exclude_roots = Vector{Module}([Base, Core]))
            if exists(names(m), x)
                push!(out, m)
            end
        end

        return out
    end
    export get_all_modules_defining

end
