cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -fconcepts -pthread -lpthread -lGL -Wl,--export-dynamic")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

project(jluna)

### DEPENDENCIES #######################################################################################

set(JULIA_EXECUTABLE /home/clem/Applications/julia/bin/julia) # modify this if necessary
include(${CMAKE_CURRENT_SOURCE_DIR}/.src/detect_julia.cmake)

### jluna #############################################################################################

add_library(jluna STATIC

    jluna.hpp

    include/state.hpp
    .src/state.inl

    include/proxy.hpp
    .src/proxy.inl

    .src/julia_extension.h

    #include/~array_proxy.hpp
    #.src/~array_proxy.inl
    #.src/~array_proxy_iterator.inl
    #.src/~array_vector.inl

    include/unbox_any.hpp
    include/box_any.hpp

    include/exceptions.hpp
    .src/exceptions.inl

    include/function_proxy.hpp
    .src/function_proxy.inl

    include/symbol_proxy.hpp
    .src/symbol_proxy.inl

    include/type_proxy.hpp
    .src/type_proxy.inl

    include/global_utilities.hpp
    .src/global_utilities.inl
        include/array_proxy.hpp)

set_target_properties(jluna PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(jluna ${JULIA_DIR}/lib/libjulia.so)

include_directories("./include")
include_directories("./")

### EXECUTABLES ########################################################################################

add_executable(TEST .test/main.cpp)
target_link_libraries(TEST jluna)


