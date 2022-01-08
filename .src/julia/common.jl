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
    wrap the dot operator to work on structs and modules
    """
    function dot(x::T, field_name::Symbol) ::Any where T

        if (isstructtype(T))
            return getfield(x, field_name)
        else
            return Base.eval(Main, :($x.$field_name))
        end
    end
    export dot;

    dot(x::Module, field_name::Symbol) = return x.eval(field_name);

    """
    assign value in module
    """
    function assign(m::Module, name::Symbol, new_value::Any) ::Nothing
        Base.eval(m, :($name = $new_value))
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

