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

        """
        typealias, 64-bit C-pointer
        """
        const Cpointer = UInt64
        export Cpointer

        """
        string(::Cpointer) -> String

        convert pointer to C++ to_string notation
        """
        function string(ptr::Cpointer) ::String
            return "0x" * Base.string(ptr; base=16)
        end
        export string

        const _refs = Ref(IdDict{Cpointer, Base.RefValue{Any}}())
        const _ref_counter = Ref(IdDict{Cpointer, UInt64}())
        
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
        assign(::Cpointer, ::Any) -> Nothing

        reassign C++ manage value
        """
        function assign(pointer::Cpointer, v::Any) ::Nothing

            @assert haskey(_refs[], pointer)
            _refs[][pointer].x = v
            return nothing
        end

        """
        create_reference(::Cpointer, ::T) -> T

        add reference to _refs
        """
        function create_reference(ptr::Cpointer, to_wrap::T) ::T where T

            #println("[JULIA] allocated " * string(ptr) * " (" * Base.string(typeof(to_wrap)) * ")")
            if (haskey(_refs[], ptr))
                @assert _refs[][ptr].x == to_wrap && typeof(to_wrap) == typeof(_refs[][ptr].x)
                _ref_counter[][ptr] += 1
            else
                _refs[][ptr] = Base.RefValue{Any}(to_wrap)
                _ref_counter[][ptr] = 1
            end

            return _refs[][ptr].x;
        end

        """
        free_reference(::Cpointer) -> Nothing

        free reference from _refs
        """
        function free_reference(ptr::Cpointer) ::Nothing

            @assert haskey(_refs[], ptr)
            #println("[JULIA] freed " * string(ptr) * " (" * Base.string(typeof(_refs[][ptr].x)) * ")")

            count = _ref_counter[][ptr]
            _ref_counter[][ptr] -= 1

            if (count == 0)
                delete!(_ref_counter[], ptr)
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

        """
        get_value(::Cpointer) -> Any

        access value of allocated memory
        """
        function get_value(ptr::Cpointer) ::Any

            if (!haskey(_refs[], ptr))
                return nothing
            else
                return _refs[][ptr].x
            end
        end

        """
        get_reference(::Cpointer) -> RefValue{Any}

        access reference in _refs
        """
        function get_reference(ptr::Cpointer) ::Base.RefValue{Any}

            if (!haskey(_refs[], ptr))
                return nothing
            else
                return _refs[][ptr]
            end
        end
    end
end
