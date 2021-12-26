#
# Copyright 2021 Clemens Cords
# Created on 17.12.21 by clem (mail@clemens-cords.com)
#
begin # included into module jlwrap

    """
    list all submodules in Main

    @returns array of modules
    """
    function list_submodules() ::Array{Module, 1}

        return map(x -> convert(Module, x), filter(x->isa(x, Module), map(x -> eval(x), names(Main))))
    end

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
end

