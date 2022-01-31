# THIS PROJECT IS DEPRECATED, USE: https://github.com/Clemapfel/jluna.git INSTEAD

Julia is a beautiful language, it is well-designed and well-documented. julia C-API is also well-designed, less beautiful and much less... documented.

Heavily inspired in design and syntax by (but in no way affiliated with) the excellent Lua⭤C++ wrapper [**sol2**](https://github.com/ThePhD/sol2), `jluna` aims to fully wrap the official Julia C-API and replace it in usage in C++ projects by making accessing julia unique strengths through C++ safe, hassle-free and just as beautiful.

---

### Table of Contents

0. [Introduction](README.md)
1. [Showcase](#showcase)<br>
2. [Features](#features)<br>
3. [Planned Features](#planned-but-not-yet-implemented)<br>
4. [Documentation](#documentation)<br>
    4.1 [Manual](./docs/docs.md)<br>
    4.2 [Quick & Dirty Overview](#documentation)<br>
5. [Dependencies](#dependencies)<br>
   5.1 [Julia 1.7.0+](#dependencies)<br>
   5.2 [g++10](#dependencies)<br>
   5.3 [cmake 3.19+](#dependencies)<br>
   5.4 [Linux / Mac OS](#dependencies)
6. [License](#license)
7. [Installation](#installation)<br>
  7.1 [Step-by-Step Guide](#installation)<br>
  7.2 [Troubleshooting](#troubleshooting)<br>
   
---

## Showcase

```cpp
using namespace jluna;

// one-line initialization and setup
State::initialize();

// run arbitrary code with exception forwarding
State::safe_script(R"(
    mutable struct Holder
        _array_field::Array{Int64, 3}
        _vector_field::Vector{String}
    
        Holder() = new(reshape(collect(1:(3*3*3)), 3, 3, 3), Vector{String}())
    end
    
    instance = Holder();
)");

// access and mutate variables
Array<Int64, 3> array = Main["instance"]["_array_field"];
array.at(0, 1, 2) = 9999;

// std:: objects are supported out-of-the-box
Main["instance"]["_vector_field"] = std::vector<std::string>{"string", "string", "string"};

// call julia-side functions with C++-side arguments
auto println = State::script("return Base.println");
println(Main["instance"]);

// call c++-side functions julia-side arguments
State::register_function("cpp_print", [](jl_value_t* in) -> jl_value_t* {
   
    std::cout << "cpp called" << std::endl;
    
    auto as_vector = unbox<jluna::Vector<size_t>>(in);
    for (auto e : as_vector)
        e = ((size_t)) e + 1
                
    return as_vector;
});
State::safe_script("println(cppcall(:cpp_print, [1, 2, 3, 4]))");
```
```
Holder([1 4 7; 2 5 8; 3 6 9;;; 10 13 16; 11 14 17; 12 15 18;;; 19 9999 25; 20 23 26; 21 24 27], ["string", "string", "string"])

cpp called
[2, 3, 4, 5]
```
---

### Features
Some of the many advantages `jluna` has over the C-API include:

+ expressive generic syntax
+ call C++ functions from julia using any julia-type
+ assigning C++-side proxies also mutates the corresponding variable with the same name Julia-side
+ Julia-side values, including temporaries, are kept safe from the garbage collector while they are in use C++-side
+ verbose exception forwarding from Julia, compile-time assertions
+ wraps [most](./docs/quick_and_dirty.md#list-of-unboxables) of the relevant C++ `std` objects and types
+ multidimensional, iterable array interface with Julia-style indexing
+ fully documented, including inline documentation for IDEs for both C++ and Julia code
+ mixing the C-API and `jluna` works no problem
+ And more!

### Planned (but not yet implemented):
In order of priority, highest first:

+ `v0.6`: expression proxy, access to meta features via C++
+ `v0.7`: creating new modules and datatypes with member-access completely C++-Side
+ `v0.8`: thread-safe `cppcall` and proxy-data read/write
+ `v0.9`: No-Overhead performance version of proxies and `cppcall`
+ `v1.0`: multiple julia states, save-states: restoring a previous julia state
---

## Documentation

A step-by-step introduction and reference guide intended for users is available [here](./docs/docs.md). Furthermore, all user-facing code has in-line documentation available through most IDEs (or the julia `help?` command). 

Advanced users are encouraged to check the headers (available in `jluna/include`) for implementation details. They are formatted specifically to be easily understood by 3rd parties. 

---

## Dependencies

`jluna` aims to be as modern as is practical. It uses C++20 features extensively and aims to support the newest Julia version, rather than focusing on backwards compatibility. If you are looking for a C++ library that supports Julia 1.5 or lower, consider checking out [CxxWrap](https://github.com/JuliaInterop/CxxWrap.jl) instead.

For `jluna` you'll need:
+ [**Julia 1.7.0**](https://julialang.org/downloads/#current_stable_release) (or higher)
+ [**g++10**](https://askubuntu.com/questions/1192955/how-to-install-g-10-on-ubuntu-18-04) (or higher)
  - including `-fconcepts`
+ [**cmake 3.16**](https://cmake.org/download/) (or higher)
+ unix-based operating system

Currently, only g++10 is supported, clang support is planned in the future.

---

## License

`jluna` is freely available for non-commercial and educational use. For use in for-profit commercial applications, please [contact the developer](https://www.clemens-cords.com/contact).

---

## Installation

The following is a step-by-step guide to creating your own application using `jluna`.

First, we create our workspace directory. For the remainder of this section, this will be assumed to be `~/my_project`. We now execute:

```bash
cd ~/my_project
git clone https://github.com/Clemapfel/jluna.git
```

This adds the folder `jluna/` to our directory. We now need to recompile `jluna`:

```bash
# still in ~/my_project
cd jluna
mkdir build
cd build
cmake -D CMAKE_CXX_COMPILER=g++-10 ..
make
```
If some dependencies are not met, this may throw errors. Make sure `g++-10`, `julia 1.7.0` (or higher) and `cmake 3.16` (or higher) are installed on a system level.

Some warnings will appear. This is due to julia official C header `julia.h` being slightly outdated and is nothing to worry about. `jluna` itself should show no warnings.

We verify everything works by running the test executable we just compiled:

```bash
# in ~/my_project/jluna/build
./JLUNA_TEST
```

A lot of output will appear, at the very end it should show:

```
Number of tests unsuccessful: 0
```

If errors appear here, head to [troubleshooting](#troubleshooting).


Moving on to creating our own application and linking it, we first create our own `main.cpp`:

```bash
cd ~/my_project
gedit main.cpp
```

This opens a GUI text editor. Any other editor (`vim`, `nano`, `emacs`, etc.) can be substituted for `gedit`.

We paste the following into our empty `my_project/main.cpp`:

```cpp
#include <jluna.hpp>

using namespace jluna;

int main()
{
    State::initialize();
    Base["println"]("hello julia");
    State::shutdown();
}
```

and save.

Of course we need a good way to compile it. To do this, we create our very own `CMakeLists.txt`:

```bash
# in ~/my_project
gedit CMakeLists.txt
```

As a starting point, we paste the following into our `my_project/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)

# name of our project
project(MyProject)

# cmake and cpp settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs -fconcepts -pthread -lpthread -lGL -Wl,--export-dynamic")
set(CMAKE_CXX_STANDARD 20)

# build type
set(CMAKE_BUILD_TYPE Debug)

# include directories needed by jluna
include_directories("./jluna/")
include_directories("./jluna/include")

# find julia
set(JULIA_EXECUTABLE julia) # may need to be modified, c.f. Troubleshooting
include(${CMAKE_SOURCE_DIR}/jluna/find_julia.cmake)

# find jluna and jluna_c_adapter
find_library(jluna REQUIRED NAMES libjluna.so PATHS ${CMAKE_SOURCE_DIR}/jluna/)
find_library(jluna_c_adapter REQUIRED NAMES libjluna_c_adapter.so PATHS ${CMAKE_SOURCE_DIR}/jluna/)

# add our executable
add_executable(MY_EXECUTABLE ./main.cpp)

# link executable with jluna, jluna_c_adapter and julia
target_link_libraries(MY_EXECUTABLE ${jluna} ${jluna_c_adapter} ${JULIA_DIR}/lib/libjulia.so)
```

Having created `CMakeLists.txt`, we now create our own build folder:

```bash
# in ~/my_project
mkdir build 
```

We can now compile our project:

```bash
# in ~/my_project
cd build
cmake -D CMAKE_CXX_COMPILER=g++-10 ..
make
```

Warnings will again appear (due to the official julia header). 

Our directory should now look like this:

```
my_project/
    main.cpp
    CMakeLists.txt
    jluna/
        jluna.hpp
        libjluna.so
        libjluna_c_adapter.so
        build/
            JLUNA_TEST
            (...)
        (...)
    build/
        MY_EXECUTABLE
        (...)
```
Where any name with the postfix `/` is a folder.

We execute our freshly compiled executable using:

```bash
./MY_EXECUTABLE
```
```
[JULIA][LOG] initialization successfull.
hello julia
```
---

## Troubleshooting

### CMake cannot find Julia

`jluna` detects the julia version and build parameters using the `julia` command in bash. If this command is not available on a system level, we will need to manually supply the path for the julia executable to `jluna` and our own program. To do this:

We open `jluna/CMakeLists.txt` in an editor and modify the following statement in line 10

```cmake
set(JULIA_EXECUTABLE julia)
```
to
```cmake
set(JULIA_EXECUTABLE /path/to/our/.../julia/bin/julia) # replace with the path to julia/bin/julia here
```

Furthermore, in our own `my_project/CMakeLists.txt` we modify:

```cmake
# find julia
set(JULIA_EXECUTABLE julia)
```
to
```cmake
set(JULIA_EXECUTABLE /path/to/our/.../julia/bin/julia)
```

We then redo all steps except folder creation outlined in [installation](#installation).
During make, `jluna` should now be able to determine all the information to build and link `jluna` itself and our own executable properly.

### `jl_init()` fails

`jluna` assumes that julia is installed on a system level. If this is not the case, we will need to manually specify the path to the julia image during the initialization step in C++. <br><br>
When calling `jluna::State::initialize()` at the start of our C++ main `my_project/main.cpp` we replace

```cpp
State::initialize();
```
with
```cpp
State::initialize("/path/to/our/.../Julia/bin");
```

Make sure that the image is uncompressed, as `.zip` or `.tar` files cannot be used for initialization.

### Other Issues

Please make sure that:
+ you are on a linux-based, 64-bit operating system
+ julia 1.7.0 (or higher) is installed
+ cmake 3.16 (or higher) is installed
+ g++-10 (exactly) and gcc-9 (or higher) are installed
+ `my_project/CMakeLists.txt` and `my_project/main.cpp` are identical to the code in [installation](#installation)
+ `State::initialize` and `set(JULIA_EXECUTABLE (...))` are modified as outlined above
+ `jluna` was freshly pulled from the git repo
+ `my_project/jluna/` contains `libjluna.so` and `libjluna_c_adapter.so`
+ `my_project/jluna/build/JLUNA_TEST` was ran

If all of the above apply, please create an issue stating your operating system, the output of `JLUNA_TEST`, and your problem in the [issues tab](https://github.com/Clemapfel/jluna/issues).

---
