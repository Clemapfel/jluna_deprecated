#
# Copyright 2021 Clemens Cords
# Created on 28.12.2021 by clem (mail@clemens-cords.com)
#

include("./visibility_macros.jl")

begin # included into module jluna

    """
    offers julia-side memory management for C++ jluna
    """
    module memory_handler

        _current_id = UInt64(1);
        const _refs = Ref(Dict{UInt64, Base.RefValue{Any}}())
        const _ref_counter = Ref(IdDict{UInt64, UInt64}())
        
        """
        assign(::Module, ::Symbol, ::Any) -> Any

        reassign variable in global scope

        Examples:
            module MyModule
                my_var = "abcd"
            end

            assign(MyModule, :my_var, 1234)
            assign(:my_var, 1234) # if MyModule is submodule of main
        """
        function assign(m::Module, symbol::Symbol, v::T) ::Any where T

            @assert isdefined(m, symbol)

            if typeof(m.eval(symbol)) <: Ref
                return m.eval(Meta.parse("global " * string(symbol) * "[] = " * string(v)))
            else
                return m.eval(Meta.parse("global " * string(symbol) * " = " * string(v)))
            end
        end

        assign(symbol::Symbol, v::T) where T = assign(Main, symbol, v)

        """
        assign(::Module, ::Symbol, ::String) -> Any

        reassign variable in global scope
        """
        function assign(m::Module, symbol::Symbol, v::String) ::Any

            @assert isdefined(m, symbol)
            v = replace(v, "\"" => "\\\"", "\\" => "\\\\")

            if typeof(m.eval(symbol)) <: Ref
                return m.eval(Meta.parse("global " * string(symbol) * "[] = " * "\"" * v * "\""))
            else
                return m.eval(Meta.parse("global " * string(symbol) * " = " * "\"" * v * "\""))
            end
        end

        assign(symbol::Symbol, v::String) = assign(Main, symbol, v)

        """
        create_reference(::UInt64, ::Any) -> UInt64

        add reference to _refs
        """
        function create_reference(to_wrap::Any) ::UInt64

            if (to_wrap == nothing)
                return 0;
            end

            global _current_id += 1;
            key = _current_id;

            #println("[JULIA] allocated " * string(key) * " (" * Base.string(typeof(to_wrap)) * ")")

            if (haskey(_refs[], key))
                @assert _refs[][key].x == to_wrap && typeof(to_wrap) == typeof(_refs[][key].x)
                _ref_counter[][key] += 1
            else
                _refs[][key] = Base.RefValue{Any}(to_wrap)
                _ref_counter[][key] = 1
            end

            return key;
        end

        """
        get_reference(::Int64) -> Any
        """
        function get_reference(key::UInt64) ::Any

            if (key == 0)
                return nothing;
            end

           return _refs[][key];
        end

        """
        free_reference(::UInt64) -> Nothing

        free reference from _refs
        """
        function free_reference(key::UInt64) ::Nothing

            if (key == 0)
                return nothing;
            end

            @assert haskey(_refs[], key)
            #println("[JULIA] freed " * string(key) * " (" * Base.string(typeof(_refs[][key].x)) * ")")

            global _ref_counter[][key] -= 1
            count = _ref_counter[][key]

            if (count == 0)
                delete!(_ref_counter[], key)
                delete!(_refs[], key)
            end

            return nothing;
        end

        """
        force_free() -> Nothing

        immediately deallocate all C++-managed values
        """
        function force_free() ::Nothing

            for k in keys(_refs)
                delete!(_refs[], k)
                delete!(_ref_counter[], k)
            end

            @assert isempty(_refs) && isempty(_ref_counter)

            return nothing;
        end
    end
end
