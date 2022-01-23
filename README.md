# jluna: A modern Julia ⭤ C++ Wrapper API (v0.5)

Julia is a beautiful language, it is well-designed and well-documented. Julias C-API is also well-designed, less beautiful and much less... documented.

Heavily inspired in design and syntax by (but in no way affiliated with) the excellent Lua⭤C++ wrapper [**sol2**](https://github.com/ThePhD/sol2), `jluna` aims to fully wrap the official Julia C-API and replace it in usage in C++ projects by making accessing Julias unique strengths through C++ safe, hassle-free and just as beautiful.

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
    
    // increment all elements in vector by one
    auto as_vector = unbox<jluna::Vector<size_t>>(in);
    for (auto e : as_vector)
        e = ((size_t)) e + 1
                
    return as_vector;
});

// called by julia:
State::safe_script("println(cppcall(:cpp_print, [1, 2, 3, 4]))");
```
```
Holder([1 4 7; 2 5 8; 3 6 9;;; 10 13 16; 11 14 17; 12 15 18;;; 19 9999 25; 20 23 26; 21 24 27], ["string", "string", "string"])

cpp called
[2, 3, 4, 5]
```
---

### Table of Contents

0. [Introduction](./README.md)
1. [Showcase](#showcase)<br>
2. [Features](#features)<br>
3. [Planned Features](#planned-but-not-yet-implemented)<br>
4. [Documentation](#documentation)<br>
    4.1 [Manual](./docs/proxies.md)<br>
    4.2 [Quick & Dirty Overview](#documentation)<br>
5. [Dependencies](#dependencies)<br>
   5.1 [Julia 1.7.0+](#dependencies)<br>
   5.2 [g++10](#dependencies)<br>
   5.3 [cmake 3.19+](#dependencies)<br>
   5.4 [Linux / Mac OS](#dependencies)
6. [Installation](#installation)<br>
  6.1 [Single Application](#jluna-only-application)<br>
  6.2 [As a Library](#adding-jluna-to-your-existing-library)<br>
  6.3 [Troubleshooting](#troubleshooting)<br>
   
---

### Features
Some of the many advantages `jluna` has over the C-API:

+ automatically detects and links Julia during make
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
+ `v1.0`: save-states, restoring a previous Julia state
---

## Documentation

A fly-by overview of all of the relevant features is available [here](./docs/quick_and_dirty.md). Innline-documentation inside the headers is already available through any IDE. A more in-depth manual intended for people developing `jluna` itself is in the works.

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

If you are curious, modernity is also a necessity: `jluna` makes extensive use of C++ concepts to allow for easy-to-understand compile-time errors when boxing and unboxing julia-side values into various C++ types. This requires specifically G++10 or higher due to [this bug in G++9](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79917). <br>
For julia, `jluna` needs `get(::Tuple, ::Integer, default)` to forward generic function arguments to C++ during `cppcall` directly as a tuple which is [only available in v.1.7+](https://docs.julialang.org/en/v1/base/collections/#Base.get), though this is not the only 1.7+-only function used in `jluna`.

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
If errors appear, make sure [all the dependencies](#dependencies) are met and check the [troubleshooting sections](#troubleshooting) for FAQs. 

You can verify everything works by calling:

```bash
# still in jluna/build
./JLUNA_TEST
```

At the very end of the programs console output it should show:
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

Then add the following lines to the end of `jluna/CmakeLists.txt`:

```cmake
add_executable(MY_EXECUTABLE path/to/.../my_main.cpp)   # modify this as needed
target_link_libraries(MY_EXECUTABLE jluna)
```

Then, navigate again to `jluna/build` and execute:
```bash
cd path/to/.../jluna/build
cmake -D CMAKE_CXX_COMPILER=g++-10 ..
make
```

Your executable can now be run via `./MY_EXECUTABLE`:

```
[JULIA] hello world
```

Alternatively you can run `jluna/CmakeLists.txt` from within your own `CMakeLists.txt` using [include](https://cmake.org/cmake/help/latest/command/include.html).

If you are using a cmake-based IDE like [CLion](https://www.jetbrains.com/clion/) or [Atom](https://atom.io/), it may be enough to simply create a new project and open `jluna/CMakeLists.txt`. Make sure to set your compiler to G++10, though.

### Adding jluna to your existing Library

First, add `jluna` as a submodule to your git repository:

```
git submodule add https://github.com/Clemapfel/jluna.git
```

Add the header include path to your `CMakeLists.txt` and link your library like so:

```cmake
include_directories("/path/to/.../jluna/")
target_link_libraries(YOUR_LIBRARY jluna)
```

Now simply add `#import <jluna.hpp>` to your headers and everything should work.

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

### jl_init() fails

jluna assumes that `Julia` is installed on a system level. If this is not the case, you will need to manually specify the path to your image during the initialization step in C++. When calling `jluna::State::initialize()` at the start of your C++ main, instead of the no-argument call, use this overload and provide the full path to your Julia image like so:

```cpp
jluna::State::initialize("/path/to/your/.../Julia/bin");
```

Make sure that the image is uncompressed, as `.zip` or `.tar` files cannot be used for initialization.

### cppcall fails

While jluna is header-only, julia needs a shared c library to interface with jluna in the julia -> C++ direction. This library is `libjluna_c_adapter.so` and comes precompiled with the github repo. If `cppcall` fails on your system (usually an assertion is triggered on `State::initialize`), we will need to recompile it.

First navigate to `jluna/`, then:

```bash
rm libjluna_c_adapter.so

mkdir temp
cd temp
cmake -D CMAKE_CXX_COMPILER=g++-10 ..
make
```

We just recompiled the library, we can now make sure everything works and nothing is corrupted:

```bash
# still in jluna/temp
./JLUNA_TEST
```

If this executable reports no failed tests, we can remove the superfluous files:

```bash
cd ..
rm -r temp
```

This will leave a shiny new `jluna/libjluna_c_adapter.so` in your folder that should now allow julia to interface with C++ through the jluna C library.

---

## License

`jluna` is freely available for non-commercial and educational use. For use in for-profit commercial applications, please [contact the developer](https://www.clemens-cords.com/contact).
