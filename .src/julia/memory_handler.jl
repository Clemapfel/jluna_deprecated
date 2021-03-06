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

        _current_id = UInt64(0);
        const _refs = Ref(Dict{UInt64, Base.RefValue{Any}}())
        const _ref_counter = Ref(IdDict{UInt64, UInt64}())

        const _ref_id_marker = '#'

        """
        print_refs() -> Nothing

        pretty print _ref state, for debugging
        """
        function print_refs() ::Nothing

            println("jluna.memory_handler._refs: ");
            for e in _refs[]
                println("\t", Int64(e.first), " => ", e.second[], " (", typeof(e.second[]), ")")
            end
        end

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

            #println("[JULIA] allocated " * string(key) * " (" * Base.string(to_wrap) * ")")

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
        set_reference(::UInt64, ::T) -> Nothing

        update the value of a reference in _refs without adding a new entry or changing it's key, ref pointers C++ side stay valid
        """
        function set_reference(key::UInt64, new_value::T) ::Base.RefValue{Any} where T

            _refs[][key] = Base.RefValue{Any}(new_value)
            return _refs[][key]
        end

        """
        """
        function assign(new_value::T, names::Symbol...) ::T where T

            unnamed_to_index(s::Symbol) = tryparse(UInt64, chop(string(s), head = 1, tail = 0))

            name = "";

            in_main = false;

            for n in names

                as_string = string(n);
                if as_string[1] == _ref_id_marker
                    if as_string[2] == '1' && length(as_string) == 2 # main
                        in_main = true
                        continue
                    else
                        name *= "jluna.memory_handler._refs[][" * chop(string(n), head = 1, tail = 0) * "][]"
                    end
                elseif as_string[1] == '['
                    name *= string(n)
                else
                    name *= "." * string(n)
                end
            end

            if in_main
                name = chop(name, head = 1, tail = 0)   # remove first .
            end

            Main.eval(:($(Meta.parse(name)) = $new_value));
            return new_value;
        end

        """
        """
        function evaluate(names::Symbol...) ::Any

            name = "";

            for n in names

                as_string = string(n);
                if as_string[1] == _ref_id_marker
                    name *= "jluna.memory_handler._refs[][" * chop(string(n), head = 1, tail = 0) * "][]"
                elseif as_string[1] == '['
                    name *= string(n)
                else
                    name *= "." * string(n)
                end
            end

            return Main.eval(:($(Meta.parse(name))))
        end

        """
        get_reference(::Int64) -> Any
        """
        function get_reference(key::UInt64) ::Any

            if (key == 0)
                return nothing
            end

           return _refs[][key]
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

        """
        """
        function wrap_lambda() end
    end
end