#
# Copyright 2021 Clemens Cords
# Created on 17.12.21 by clem (mail@clemens-cords.com)
#
begin # not part of jluna

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
    check if element exists in array
    """
    function exists(array::T, v::Any) ::Bool where T <: AbstractArray

        return !isempty(findall(x -> x == v, array))
    end

    """
    wrap dot operator for both modules, structs and arrays
    """
    function dot(x::Array, field_name::Symbol) ::Any

        index_maybe = parse(Int, string(symbol));
        @assert index_maybe isa Integer
        return x[index_maybe];
    end
    export dot;

    dot(x::Module, field_name::Symbol) = return x.eval(field_name);
    dot(x::Any, field_name::Symbol) = return eval(:($x.$field_name))

    """
    used by jluna::Proxy to reassemble the full variable name and then assign it
    """
    function assemble_assign(new_value::Any, names::Symbol...) ::Nothing

        assembled = ""

        for (i, s) in enumerate(names)

            assembled *= string(s)
            if i != length(names)
                assembled *= string(Symbol("."))
            end
        end

        Main.eval(Expr(:(=), Meta.parse(assembled), new_value));
        return nothing
    end

    """
    transform a quote block to an identical :() expression by removing the first quote node
    """
    macro unquote(expr::Expr)

        function aux!(args::Vector{Any}) ::Nothing

            to_delete = Vector{Integer}()
            for (i, x) in enumerate(args)
                if x isa LineNumberNode
                    push!(to_delete, i)
                elseif x isa Expr
                    aux!(x.args)
                end
            end

            n_deleted = 0;
            for i in to_delete
                deleteat!(args, i - n_deleted)
                n_deleted += 1
            end
        end

        aux!(expr.args)
        return Expr(expr.head, :($(expr.args...)))
    end


end

