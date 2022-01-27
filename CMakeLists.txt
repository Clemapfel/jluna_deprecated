cmake_minimum_required(VERSION 3.16)

project(jluna)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -fconcepts -pthread -lpthread -lGL -Wl,--export-dynamic")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

set(JLUNA_DIR ${CMAKE_SOURCE_DIR})

### JULIA ###

set(JULIA_EXECUTABLE /home/clem/Applications/julia/bin/julia) # modify this if necessary
include(${CMAKE_SOURCE_DIR}/find_julia.cmake)

### JLUNA ###

set(RESOURCE_PATH ${CMAKE_SOURCE_DIR})
configure_file(${CMAKE_SOURCE_DIR}/.src/include.jl.hpp.in ${CMAKE_SOURCE_DIR}/.src/include.jl.hpp @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/.c_adapter/cppcall.jl.hpp.in ${CMAKE_SOURCE_DIR}/.c_adapter/cppcall.jl.hpp @ONLY)

add_library(jluna SHARED

    jluna.hpp

    include/state.hpp
    .src/state.inl
    .src/include.jl.hpp

    include/proxy.hpp
    .src/proxy.inl

    .src/julia_extension.h
    .src/common.hpp

    include/unbox_any.hpp
    include/box_any.hpp

    include/exceptions.hpp
    .src/exceptions.inl

    include/symbol_proxy.hpp
    .src/symbol_proxy.inl

    include/type_proxy.hpp
    .src/type_proxy.inl

    include/global_utilities.hpp
    .src/global_utilities.inl

    include/array_proxy.hpp
    .src/array_proxy.inl
    .src/array_proxy_iterator.inl

    include/typedefs.hpp

    .src/box_any.inl
    .src/unbox_any.inl

    include/cppcall.hpp
    .src/cppcall.inl)

set_target_properties(jluna PROPERTIES
    LINKER_LANGUAGE C
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# c adapter
add_library(jluna_c_adapter SHARED
        .c_adapter/c_adapter.cpp
        .c_adapter/c_adapter.hpp
)

set_target_properties(jluna_c_adapter PROPERTIES
    LINKER_LANGUAGE C
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# link
include_directories("${JLUNA_DIR}/include")
include_directories("${JLUNA_DIR}/")

target_link_libraries(jluna_c_adapter ${JULIA_DIR}/lib/libjulia.so)
target_link_libraries(jluna PUBLIC jluna_c_adapter ${JULIA_DIR}/lib/libjulia.so)

### EXECUTABLES ###

add_executable(JLUNA_TEST .test/main.cpp .test/test.hpp)
target_link_libraries(JLUNA_TEST jluna)


