# file meant to be included from jluna/CMakeLists.txt

# detect julia directory
execute_process(
        COMMAND ${JULIA_EXECUTABLE} -e "print(joinpath(Sys.BINDIR, \"..\"))"
        OUTPUT_VARIABLE JULIA_DIR
)

if ("${JULIA_DIR}" STREQUAL "")
    message(FATAL_ERROR "cannot determine location of julia executable. Please specify it manually by replacing the line\n\tset(JULIA_EXECUTABLE julia)\nwith\n\tset(JULIA_EXECUTABLE /path/to/your/executable)\nin ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
endif()

# assert julia version
execute_process(
        COMMAND ${JULIA_EXECUTABLE} -e "print(Base.VERSION)"
        OUTPUT_VARIABLE JULIA_VERSION
)

if ("${JULIA_VERSION}" LESS "1.7.0")
    message(WARNING "julia version is ${JULIA_VERSION} but jluna uses version 1.7.0 or higher. Some features may not work correctly.\nYou can download the latest version at https://julialang.org/downloads/#current_stable_release")
endif()

# include dir of julia.h
include_directories(${JULIA_DIR}/include/julia/)