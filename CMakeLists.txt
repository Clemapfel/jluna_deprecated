cmake_minimum_required(VERSION 3.16)

project(jluna)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -fconcepts -pthread -lpthread -lGL -Wl,--export-dynamic")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

### DEPENDENCIES ###

set(JULIA_EXECUTABLE /home/clem/Applications/julia/bin/julia) # modify this if necessary
include(${CMAKE_CURRENT_SOURCE_DIR}/.src/detect_julia.cmake)

### JLUNA ###

add_library(jluna SHARED

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

    include/cppcall.hpp .src/cppcall.inl)

set_target_properties(jluna
    PROPERTIES LINKER_LANGUAGE C
)

# c adapter
add_library(jluna_c_adapter SHARED
    .c_wrapper/c_adapter.cpp
    .c_wrapper/c_adapter.hpp
)

set_target_properties(jluna_c_adapter PROPERTIES
    LINKER_LANGUAGE C
)

# link

include_directories("./include")
include_directories("./")

target_link_libraries(jluna_c_adapter ${JULIA_DIR}/lib/libjulia.so)
target_link_libraries(jluna jluna_c_adapter ${JULIA_DIR}/lib/libjulia.so)

### EXECUTABLES ###

add_executable(JLUNA_TEST .test/main.cpp .test/test.hpp)
target_link_libraries(JLUNA_TEST jluna)


