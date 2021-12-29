#
# Copyright 2021 Clemens Cords
# Created on 28.12.2021 by clem (mail@clemens-cords.com)
#

begin # included into module jlwrap

    """
    offers julia-side memory management for C++ jlwrap
    """
    module memory_handler

        """
        typealias, 64-bit C-pointer
        """
        const Cpointer = UInt64
        export Cpointer

        """
        convert pointer to C++ to_string notation

        @param pointer
        @returns string
        """
        function string(ptr::Cpointer) ::String
            return "0x" * Base.string(ptr; base=16)
        end

        """
        holds reference to allocated objects to protect them from the garbage collector
        """
        _refs = Ref(IdDict{Cpointer, Base.RefValue{Any}}())

        """
        counts number of C++ objects currently holding a reference, only if all free_reference() will the memory be freed
        """
        _ref_counter = Ref(IdDict{Cpointer, UInt64}())


        """
        reassign variable in global scope

        @param [m]: module eval is called from (optional)
        @param symbol: exact variable name
        @param v: value of any type but string
        @returns value of expression evaluation

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
        reassign variable in global scope

        @param [m]: module eval is invoked in (optional)
        @param symbol: exact variable name
        @param v: new value of type string
        @returns value of expression evaluation
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
        reassign C++ manage value

        @param pointer: index in _refs
        @param v, value of any type
        """
        function assign(pointer::Cpointer, v::T) ::Nothing where T

            @assert haskey(_refs[], pointer)
            _refs[][pointer].x = v
            return nothing
        end

        """
        add reference to _refs

        @param ptr: c-pointer set C++ side
        @param to_wrap: value to push
        @returns value of new reference for further use
        """
        function create_reference(ptr::Cpointer, to_wrap::T) ::T where T

            #println("[JULIA] allocated" * string(ptr) * " (" * Base.string(typeof(to_wrap)) * ")")

            if (haskey(_refs[], ptr))
                @assert _refs[][ptr].x == to_wrap && typeof(to_wrap) == typeof(_refs[][ptr].x)
                _ref_counter[][ptr] += 1
            else
                _refs[][ptr] = Base.RefValue{Any}(to_wrap)
                _ref_counter[][ptr] = 0
            end

            return _refs[][ptr].x
        end

        """
        free reference from _refs

        @param ptr: c-pointer set C++ side
        """
        function free_reference(ptr::Cpointer) ::Nothing

            @assert haskey(_refs[], ptr)
            #println("[JULIA] freed " * string(ptr) * " (" * Base.string(typeof(_refs[][ptr].x)) * ")")

            count = _ref_counter[][ptr]

            if (count <= 1)
                delete!(_ref_counter[], ptr)
            else
                _ref_counter[][ptr] -= 1
            end

            return nothing;
        end

        """
        immediately deallocate all C++-managed values
        """
        function force_free() ::Nothing

            for k in keys(_refs)
                delete!(_refs, k)
                delete!(_ref_counter, k)
            end

            @assert isempty(_refs) && isempty(_ref_counter)

            return nothing;
        end

        """
        access value of allocated memory

        @param c-pointer set C++-side
        @returns value
        """
        function get_value(ptr::Cpointer) ::Any

            if (!haskey(_refs[], ptr))
                return nothing
            else
                return _refs[][ptr].x
            end
        end

        """
        access reference in _refs

        @param c-pointer set C++-side
        @returns allocated reference
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
