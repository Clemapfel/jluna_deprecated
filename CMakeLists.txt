cmake_minimum_required(VERSION 3.18)

project(jluna)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -fconcepts -pthread -lpthread -lGL -Wl,--export-dynamic")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

# cmake -D CMAKE_C_COMPILER=gcc-10 -D CMAKE_CXX_COMPILER=g++-10 .ls

### DEPENDENCIES ###

set(JULIA_EXECUTABLE /home/clem/Applications/julia/bin/julia) # modify this if necessary
include(${CMAKE_CURRENT_SOURCE_DIR}/.src/detect_julia.cmake)

### JLUNA ###

add_library(jluna STATIC

    jluna.hpp

    include/state.hpp
    .src/state.inl

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

    include/cpp_call.hpp
)

set_target_properties(jluna PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(jluna ${JULIA_DIR}/lib/libjulia.so)

include_directories("./include")
include_directories("./")

# c adapter
add_library(jluna_c_adapter SHARED
    .c_wrapper/cpplibrary.hpp
    .c_wrapper/cpplibrary.hpp
)

set_target_properties(jluna_c_adapter PROPERTIES
    LINKER_LANGUAGE C
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARY_OUTPUT_NAME jluna_c
)

target_link_libraries(jluna_c_adapter ${JULIA_DIR}/lib/libjulia.so)

### EXECUTABLES ###

add_executable(JLUNA_TEST .test/main.cpp .test/test.hpp)
target_link_libraries(JLUNA_TEST jluna)


