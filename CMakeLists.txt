cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -pthread -lpthread -lGL -Wl,--export-dynamic")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

project(jlwrap)

### DEPENDENCIES #######################################################################################

set(JULIA_EXECUTABLE /home/clem/Applications/julia/bin/julia) # modify this if necessary
include(${CMAKE_CURRENT_SOURCE_DIR}/.src/detect_julia.cmake)

### JLWRAP #############################################################################################

add_library(jlwrap STATIC

    include/state.hpp
    .src/state.inl

    include/primitive_proxy.hpp
    .src/primitive_proxy.inl

    include/proxy.hpp
    .src/proxy.inl
    include/julia_extension.h
    include/exceptions.hpp

    #include/function_proxy.hpp
    #.src/function_proxy.inl

    include/array_proxy.hpp
    .src/array_proxy.inl
    .src/array_proxy_iterator.inl
    .src/array_vector.inl

    include/unbox_any.hpp
    include/box_any.hpp
        .src/exceptions.inl include/symbol_proxy.hpp include/function_proxy.hpp .src/function_proxy.inl)

set_target_properties(jlwrap PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(jlwrap ${JULIA_DIR}/lib/libjulia.so)

include_directories("./include")
include_directories("./")

### EXECUTABLES ########################################################################################

add_executable(TEST .test/main.cpp)
target_link_libraries(TEST jlwrap)


