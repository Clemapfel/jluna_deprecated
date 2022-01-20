# jluna: A modern Julia ⭤ C++ Wrapper API (v0.5)

Julia is a beautiful language, it is well-designed and well-documented. Julias C-API is also well-designed, less beautiful and much less... documented.

Heavily inspired in design and syntax by (but in no way affiliated with) the excellent Lua⭤C++ wrapper [**sol2**](https://github.com/ThePhD/sol2)*, `jluna` aims to fully replace the official Julia C-API in usage in C++ projects and makes accessing Julias unique strengths through C++ safe and hassle-free.

---

### Table of Contents

0. [Introduction](./README.md)
1. [Showcase](#showcase)<br>
2. [Features](#features)<br>
3. [Planned Features](#planned-but-not-yet-implemented)<br>
4. [Dependencies](#dependencies)<br>
   4.1 [Julia 1.7.0+](#dependencies)<br>
   4.2 [g++10](#dependencies)<br>
   4.3 [cmake 3.19+](#dependencies)<br>
   4.4 [Linux / Mac OS](#dependencies)
5. [Installation](#installation)<br>
  5.1 [Linking](#installation)<br>
  5.2 [Troubleshooting](#troubleshooting)<br>
6. [Documentation](#documentation)<br>
    6.1 [Manual](./docs/docs.md)<br>
    6.2 [Quick & Dirty Overview](#documentation)
   
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

// std:: object are supported out-of-the-box
Main["instance"]["_vector_field"] = std::vector<std::string>{"string", "string", "string"};

// call julia-side functions with C++-side arguments
auto println = State::script("return Base.println");
println(Main["instance"]);

// call c++-side functions julia-side arguments
State::register_function("cpp_print", [](auto first, auto second) -> jl_value_t* {
   
    std::cout << "cpp prints: " << jl_to_string(first) << " " << jl_to_string(second) << std::endl;
    return nullptr;
});

State::safe_script("cppcall(:cpp_print, [1, 2, 3, 4], Main");
```
```
Holder([1 4 7; 2 5 8; 3 6 9;;; 10 13 16; 11 14 17; 12 15 18;;; 19 9999 25; 20 23 26; 21 24 27], ["string", "string", "string"])
cpp prints: [1, 2, 3, 4] Main
```
---

### Features
Some advantages `jluna` has over the C-API include:

+ automatically detects and links Julia during make
+ expressive generic syntax
+ Julia-side values, including temporaries, are kept safe from the garbage collector while they are in use C++-side
+ assigning C++-side proxies also mutates the corresponding variable with the same name Julia-side
+ verbose exceptions, including exception forwarding from Julia
+ wraps [most](./docs/quick_and_dirty.md#list-of-unboxables) of the relevant C++ std objec1ts and types
+ multi-dimensional, iterable array interface with Julia-Style indexing
+ `jluna` is fully documented, including tutorials and inline documentation for IDEs for both C++ and Julia code
+ mixing the C-API and `jluna` works out-of-the-box
+ And more!

### Planned (but not yet implemented):
In order of priority, highest first:
+ expression proxy, access to meta features via C++
+ Julia-side wrapper for C++ Functions, similar to `@ccall`
+ creating new modules and datatypes completely C++-Side
+ save-states, restoring a previous Julia state
+ clang support
+ Julia-side macro expansion during C++ compile time 

---

## Dependencies

`jluna` aims to be as modern as is practical. It uses C++20 features extensively and aims to support the newest Julia version, rather than focusing on backwards compatibility. If you are looking for a C++ library that supports Julia 1.5 or lower, consider checking out [CxxWrap](https://github.com/JuliaInterop/CxxWrap.jl) instead.

For `jluna` you'll need:
+ [**Julia 1.7.0**](https://julialang.org/downloads/#current_stable_release) (or higher)
+ [**g++10**](https://askubuntu.com/questions/1192955/how-to-install-g-10-on-ubuntu-18-04) (or higher)
  - including `-fconcepts`
+ [**cmake 3.19**](https://cmake.org/download/) (or higher)
+ unix-based operating system

Currently, only g++10 is supported, clang support is planned in the future.

---

## Installation

### jluna-Only Application:

Go to your (empty) workspace folder and execute:

```bash
git clone https://github.com/Clemapfel/jluna.git
cd jluna
mkdir build
cd build
cmake -D CMAKE_CXX_COMPILER=g++-10 ..
make
```
If errors appear, make sure all the dependencies are met. You can verify everything works by calling:

```
./JLUNA_TEST
```

At the very end it should show:
```
Number of tests unsuccessful: 0
```

You can now create your application, first create a main file `my_main.cpp`:
```
#include <jluna.h>

using namespace jluna;
int main()
{
    State::initialize();
    State::script(R"(println("hello world"))");
}
```

Then add the the follow lines to the end of `jluna/CmakeLists.txt`

```cmake
add_executable(MY_EXECUTABLE path/to/.../my_main.cpp)   # modify this as needed
target_link_libraries(MY_EXECUTABLE jluna)
```

Once again navigate to `jluna/build`:
```bash
cd path/to/.../jluna/build
cmake -D CMAKE_CXX_COMPILER=g++-10 ..
make
```

Your executable can now be run via `./MY_EXECUTABLE`:

```
[JULIA] hello world
```

### Adding jluna to your existing Library

First, add jluna as a submodule to your git repository:

```
git submodule add https://github.com/Clemapfel/jluna.git
```

Add the header include path to your CMakeLists.txt and link your library like so:

```cmake
include_directories("/path/to/.../jluna/")
target_link_libraries(YOUR_LIBRARY jluna)
```

Now simply `#import <jluna.hpp>` to your headers and everything should work.

---

## Troubleshooting

### CMake cannot find Julia

`jluna` detects your Julia version and build parameters using the `julia` command in bash, if this command is not available on a system level, you will need to manually supply the path for the Julia executable to `jluna`. To do this:

Open `jluna/CMakeLists.txt` in an editor and modify the following statement in line 10:

```cmake
set(JULIA_EXECUTABLE julia)
```
to:
```cmake
set(JULIA_EXECUTABLE /path/to/your/.../julia/bin/julia) # replace with the path to julia/bin/julia here
```

During make, `jluna` should now be able to determine all the information to build `jluna` and link Julia properly

### State::initialize() fails

jluna assumes that `Julia` is installed on a system level. If this is not the case, you will need to manually specify the path to your image during the initialization step in C++. When calling `jluna::State::initialize()` at the start of your C++ main, instead of the no-argument call, use this overload and provide the full path to your Julia image like so:

```cpp
jluna::State::initialize("/path/to/your/.../Julia/bin");
```

Make sure that the image is uncompressed, as `.zip` or `.tar` files cannot be used for initialization.

### @cppcall fails

While jluna is header-only, julia needs a shared c library to interface with jluna in the julia -> C++ direction. This library is `jluna/libjluna_c.so` and comes precompiled with the github repo. If `@cppcall` fails on your system, we will need to recompile it.

First navigate to `jluna/`, then:

```bash
mkdir temp
cd temp
cmake -D CMAKE_CXX_COMPILER=g++-10 ..
make
```

We just recompiled the library, we can now make sure everything works and nothing is corrupted:

```
# still in jluna/temp
./JLUNA_TEST

```
If this executable reports no failed tests, we can remove the superfluous files:

```bash
cd ..
rm -r temp
```

This will leave a shiny new `jluna/libjluna_c.so` in your folder that should now allow julia to interface with the jluna C library.

---

## Documentation

The manual is not yet complete, consider visiting the already completed [overview cheat-sheet](./docs/quick_and_dirty.md) instead. Furthermore, inline-documentation inside the headers is already available through any IDE.

----

## License

`jluna` is freely available for non-commercial and educational use. For use in for-profit commercial applications, please [contact the developer](https://www.clemens-cords.com/contact).











