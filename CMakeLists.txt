cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -lstdc++fs -pthread -lpthread -lGL -Wl,--export-dynamic")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

project(jlwrap)

### DEPENDENCIES ################################################################################

# Julia
include_directories(/usr/include/julia/) # modify this if julia is located somewhere else

### jlwrap #######################################################################################

add_library(jlwrap STATIC
    include/state.hpp
    .src/state.inl

    include/primitive_proxy.hpp
    .src/primitive_proxy.inl

    include/proxy.hpp
    .src/proxy.inl
    include/julia_extension.h
    include/exceptions.hpp

    include/function_proxy.hpp
    .src/function_proxy.inl

    include/array_proxy.hpp
    .src/array_proxy.inl
    .src/array_proxy_iterator.inl
    .src/array_vector.inl)

set_target_properties(jlwrap PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(jlwrap julia)

include_directories("./include")
include_directories("./")

add_executable(JULIA .test/main.cpp)
target_link_libraries(JULIA jlwrap)


