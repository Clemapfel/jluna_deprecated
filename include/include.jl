#
# Copyright 2021 Clemens Cords
# Created on 26.12.2021 by clem (mail@clemens-cords.com)
#

include("../.src/julia/visibility_macros.jl")

module jlwrap

    include("../.src/julia/common.jl")
    include("../.src/julia/exception_handler.jl")
    include("../.src/julia/memory_handler.jl")
    include("../.src/julia/state_interface.jl")
    include("../.src/julia/introspection.jl")
end

@make_public_rec jlwrap # TODO