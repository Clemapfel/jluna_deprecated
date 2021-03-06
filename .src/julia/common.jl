#
# Copyright 2021 Clemens Cords
# Created on 17.12.21 by clem (mail@clemens-cords.com)
#
begin

    """
    get_value_type_of_array(::Array{T}) -> Type

    forward value type of array
    """
    function get_value_type_of_array(_::Array{T}) ::Type where T

        return T
    end

    """
    get_reference_value(::Base.RefValue{T}) -> T

    forward value of reference
    """
    function get_reference_value(ref::Base.RefValue{T}) ::T where T

        return ref[];
    end

    setindex!(str::String, c::Char, i::Int64) = setindex!(collect(str), c, i)

    """
    is_number_only(::String) -> Bool

    check whether a string can be transformed into a base 10 number
    """
    function is_number_only(x::String) ::Bool

        for s in x
            if s != '0' || s != '1' || s != '2' || s != '3' || s != '4' || s != '5' || s != '6' || s != '7' || s != '8' || s != '9'
                return false
            end
        end

        return true;
    end

    """
    is_method_available(::Function, ::Any) -> Bool

    check if method of given function is available for a specific variable
    """
    function is_method_available(f::Function, variable) ::Bool

        return hasmethod(f, Tuple{typeof(variable)})
    end

    """
    get_function(::Symbol, ::Module) -> Function

    exception-safe function access wrapper
    """
    function get_function(function_name::Symbol, m::Module) ::Function

        return m.eval(function_name)
    end
    export get_function

    """
    exists(<:AbstractArray, ::Any) -> Bool

    check if element exists in array
    """
    function exists(array::T, v::Any) ::Bool where T <: AbstractArray

        return !isempty(findall(x -> x == v, array))
    end

    """
    tuple_at(::Tuple, ::Integer) -> Any

    get nth element of tuple
    """
    function tuple_at(x::Tuple, i::Integer)
        return x[i]
    end

    """
    make_vector(::T...) -> Vector{T}

    wrap vector ctor in varargs argument, used by box/unbox
    """
    function make_vector(args::T...) ::Vector{T} where T

        return [args...]
    end

    """
    make_set(::T...) -> Set{T}

    wrap set ctor in varargs argument, used by box/unbox
    """
    function make_set(args::T...) ::Set{T} where T

        return Set([args...]);
    end

    """
    assert_isa(::T, ::Symbol) -> Nothing

    throw assertion if x is not of named type
    """
    function assert_isa(x::Any, type_name::Symbol) ::Nothing

        type = Main.eval(type_name);
        @assert type isa Type

        if !(x isa type)
            throw(AssertionError("expected " * string(type) * " but got an object of type " * string(typeof(x))));
        end

        return nothing
    end

    """
    convert(::T, symbol::Symbol) -> Any

    convert value type, declared via symbol
    """
    function convert(x::T, symbol::Symbol) ::Any where T

        type = Main.eval(symbol);
        @assert type isa Type

        if T isa type
            return T
        end

        return Base.convert(type, x)
    end

    """
    invoke(function::Any, arguments::Any...) -> Any

    wrap function call for non-function objects
    """
    function invoke(x::Any, args...) ::Any
        return x(args...)
    end

    """
    serialize(<:AbstractDict{T, U}) -> Vector{Pair{T, U}}

    transform dict into array
    """
    function serialize(x::T) ::Vector{Pair{Key_t, Value_t}} where {Key_t, Value_t, T <: AbstractDict{Key_t, Value_t}}

        out = Vector{Pair{Key_t, Value_t}}()
        for e in x
            push!(out, e)
        end
        return out;
    end

    """
    serialize(::Set{T}) -> Vector{T}

    transform dict into array
    """
    function serialize(x::T) ::Vector{U} where {U, T <: AbstractSet{U}}

        out = Vector{U}()

        for e in x
            push!(out, e)
        end
        return out;
    end

    """
    dot(::Array, field::Symbol) -> Any

    wrapped dot operator
    """
    function dot(x::Array, field_name::Symbol) ::Any

        index_maybe = parse(Int, string(field_name));
        @assert index_maybe isa Integer
        return x[index_maybe];
    end
    export dot;

    """
    dot(::Module, field::Symbol) -> Any

    wrapped dot operator
    """
    dot(x::Module, field_name::Symbol) = return x.eval(field_name);

    """
    dot(x::Any, field::Symbol) -> Any

    wrapped dot operator, x.field
    """
    dot(x::Any, field_name::Symbol) = return eval(:($x.$field_name))

    """
    unquote(::Expr) -> Expr

    remove all line number notes and the outer most :quote block from an expression
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

