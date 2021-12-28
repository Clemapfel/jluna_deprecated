#
# Copyright 2021 Clemens Cords
# Created on 28.12.2021 by clem (mail@clemens-cords.com)
#

begin # included into module jlwrap

    """
    offers julia-side memory management for C++ jlwrap
    """
   # module memory_handler

        const Cpointer = UInt64
        export Cpointer

        function assign(symbol::Symbol, new_value::T) where T

        end

        # holds reference to allocated objects to protect them from the garbage collector
        _refs = Ref(IdDict{Cpointer, Base.RefValue{Any}}())
        _ref_counter = Ref(IdDict{Cpointer, UInt64}())

        """
        """
        function create_reference(ptr::Cpointer, to_wrap::T) ::T where T

            if (haskey(_refs[], ptr))

                @assert _refs[][ptr].x == to_wrap && typeof(to_wrap) == typeof(_refs[][ptr].x)

                if (! haskey(_ref_counter[], ptr))
                    _ref_counter[][ptr] = 0
                end

                _ref_counter[][ptr] += 1
            end

            _refs[][ptr] = Base.RefValue{Any}(to_wrap)
            return _refs[][ptr].x
        end
        
        """
        """
        function free_reference(ptr::Cpointer) ::nothing

            @assert haskey(_refs[], ptr)

            count = _ref_counter[][ptr]

            if (count <= 1)
                delete!(_ref_counter[], ptr)
            else
                _ref_counter[][ptr] -= 1
            end
        end

        """
        """
        function get_value(ptr::Cpointer) #::Auto

            if (!haskey(_refs[], ptr))
                return nothing
            else
                return _refs[][ptr].x
            end
        end

        """
        """
        function get_reference(ptr::Cpointer) #::Auto

            if (!haskey(_refs[], ptr))
                return nothing
            else
                return _refs[][ptr]
            end
        end





    #end

end
