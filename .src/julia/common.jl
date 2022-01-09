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
    dot
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
    assign
    """
    function assign(owner::Array, field_name::Symbol, new_value::Any) ::Any

        index_maybe = parse(Int, string(symbol));
        @assert index_maybe isa Integer
        owner[index_maybe] = new_value;
        return owner[index_maybe];
    end

    function assign(owner::Module, field_name::Symbol, new_value::Any)

        Base.eval(owner, :($field_name = $new_value))
        return Base.eval(owner, field_name)
    end

    function assign(owner::Any, field_name::Symbol, new_value::Any)

        println("[JULIA] " * string(owner) * " " * string(field_name) * " " * string(new_value))

        eval(:($owner.$field_name = $new_value))
        return eval(:($owner.$field_name));
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

