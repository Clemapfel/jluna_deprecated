# jluna: A modern Julia ⭤ C++ Wrapper API (v0.5)

Julia is a beautiful language, it is well-designed and well-documented. Julias C-API is also well-designed, less beautiful and much less... documented. Heavily inspired in design and syntax by the excellent Lua⭤C++ wrapper [**sol2**](https://github.com/ThePhD/sol2)*, `jluna` aims to fully replace the official Julia C-API in usage in C++ projects and makes accessing Julias unique strengths through C++ easy and hassle-free.

Some advantages `jluna` has over the C-API:
+ automatically detects and links Julia during make
+ expressive, generic syntax
+ Julia-side values are kept safe from the garbage collector while in use
+ mutating C++-side proxies also assigns the corresponding variable Julia-side if desired
+ verbose exceptions, including exception forwarding from Julia
+ wraps most of the relevant C++ std objects and types
+ multi-dimensional array interface with Julia-Style indexing 
+ `jluna` is fully documented, including tutorials and inline documentation for IDEs
+ mixing the C-API and `jluna` works no problem

(*`jluna` is in no way affiliated with the sol2 team and no code is shared between libraries)

### Table of Contents

0. [Introduction](README.md)
1. [Features](#features)<br>
  1.1 [Initialization](#initialization)<br>
  1.2 [Accessing Julia-Side Variables](#accessing-julia-side-variables)<br>
  1.3 [Modifying Julia-Side Variables](#modifying-julia-side-variables)<br>
  1.4 [Multi-Dimensional Arrays](#mulit-dimensional-arrays)<br>
  1.5 [Exception Forwarding](#exception-forwarding)<br>
2. [Planned Features](#and-more)<br>
3. [Dependencies](#dependencies)<br>
   3.1 [Julia 1.7.0+](#dependencies)<br>
   3.2 [g++10](#dependencies)<br>
   3.3 [cmake 3.19+](#dependencies)<br>
   3.3 [Linux / Mac OS](#dependencies)
4. [Installation](#installation)<br>
  4.1 [Linking](#installation)<br>
  4.2 [Troubleshooting](#troubleshooting)<br>
5. [Documentation](#documentation)<br>
    5.1 [Manual](./docs/docs.md)
    5.2 [Quick & Dirty Overview](#documentation)

## Features:

For a tour of `jluna`s functionality, see the following examples:

### Initialization
```cpp
jluna::State::initialize(); // loads Julia and jluna
```
```
[JULIA][LOG] initialization successfull.
```

### Accessing Julia-Side Variables
```cpp
// execute arbitrary string
jluna::State::safe_script(R"(
    module MyModule
        mutable struct MutableType
            _field
        end

        instance = MutableType(123)
    end
)");

// access variables with [] syntax
std::cout << (int) jluna::Main["MyModule"]["instance"]["_field"] << std::endl;

// or through script return
std::cout << (int) jluna::State::script("return MyModule.instance._field") << std::endl;
```
```
123
123
```

### Calling Julia Functions
```cpp
auto println = jluna::Base["println"];

// all primitives, all jluna:: and most std:: types can be used directly
println(std::string("this is a string "), "\n", 
        println, "\n",
        std::make_pair("first", 42), "\n",
        State::script("return [1, 2, 3]"), "\n");

// alternatively you can do it as a one-liner...
jluna::Main["Base"]["println"]("...like this");
```
```
this is a string 
println
first => 42
[1, 2, 3]

...like this
```

### Modifying Julia-Side Variables

```cpp
jluna::State::safe_script(R"(
    module MyModule
        mutable struct MutableType
            _field
        end

        instance = MutableType(123)
    end
)");
jluna::State::safe_script(R"(println("before: ", MyModule.instance._field))");

// proxy of value only: use "return"
auto by_value = jluna::State::script("return MyModule.instance._field");
by_value = 456; // only modifies proxy value, not MyModule.instance._field

jluna::State::safe_script(R"(println("now: ", MyModule.instance._field))");

// proxy of variable: use operator[]
auto by_reference = jluna::Main["MyModule"]["instance"]["_field"];
by_reference = 789; // does modify Julia-side variable by name

State::safe_script(R"(println("after: ", MyModule.instance._field))");

// it is possible to transform a variable proxy to a value proxy by using:
by_reference.set_mutating(false);
// now operator= does *not* mutate julia-side variable MyModule.instance._field
```
```
before: 123
now: 123
after: 789
```

### Mulit-Dimensional Arrays
```cpp
using namespace jluna;

State::safe_script("array = Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))");

// array of arbitrary type and rank
Array<jluna::Int64, 3> array = Main["array"];

// access element
array[3] = 8888;          // 0-based linear indexing
array.at(0, 1, 2) = 9999; // 0-based multi-dimensional indexing

State::safe_script("vector = [1, 2, 3, 4, 5, 6, 7, 8, 9]");

// vectors are an Array<T, 1> typedef with some extra functionality
Vector<int> vector = Main["vector"];
vector.push_front(0);
vector.push_back(10);

// both arrays and vectors are iterable
for (auto it : vector)
    it = it.operator int() + 10; // also assigns the Julia-side array

State::safe_script(R"(println("array: ", array))");
State::safe_script(R"(println("vector: ", vector))");
```
```
array: [1 8888 7; 2 5 8; 3 6 9;;; 10 13 16; 11 14 17; 12 15 18;;; 19 9999 25; 20 23 26; 21 24 27]
vector: [10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20]
```

### Exception Forwarding
```cpp
jluna::State::safe_script("return this_value_is_undefined");
```
```
exception in jluna::State::safe_script for expression:
"return this_value_is_undefined"

terminate called after throwing an instance of 'jluna::JuliaException'
  what(): UndefVarError: this_value_is_undefined not defined
Stacktrace:
 [1] top-level scope
   @ none:1
 [2] eval
   @ ./boot.jl:373 [inlined]
 [3] eval
   @ ./Base.jl:68 [inlined]
 [4] safe_call(expr::Expr, m::Module)
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/Julia/exception_handler.jl:39
 [5] safe_call(expr::Expr)
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/Julia/exception_handler.jl:33
 [6] top-level scope
   @ none:1

signal (6): Aborted
in expression starting at none:0
gsignal at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
abort at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
unknown function (ip: 0x7f4fd2678a30)
unknown function (ip: 0x7f4fd26844fb)
_ZSt9terminatev at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
__cxa_throw at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
forward_last_exception at /home/Workspace/jluna/./.src/exceptions.inl:39
safe_script at /home/Workspace/jluna/./.src/state.inl:101
main at /home/Workspace/jluna/.test/main.cpp:16
__libc_start_main at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
_start at /home/Workspace/jluna/cmake-build-debug/TEST (unknown line)
Allocations: 1624652 (Pool: 1623704; Big: 948); GC: 2

Process finished with exit code 134 (interrupted by signal 6: SIGABRT)
```
### And more!

### Planned (but not yet implemented):
In order of priority, highest first:
+ expression proxy, access to meta features via C++
+ Julia-side wrapper for C++ Functions, similar to `@ccall`
+ creating new modules and datatypes completely C++-Side
+ save-states, restoring a previous Julia state
+ clang support
+ Julia-side macro expansion during C++ compile time 

---

# Dependencies

`jluna` aims to be as modern as is practical. It uses C++20 features extensively and aims to support the newest Julia version, rather than focusing on backwards compatibility. If you are looking for a C++ library that supports Julia 1.5 or lower, consider checking out [CxxWrap](https://github.com/JuliaInterop/CxxWrap.jl) instead.

For `jluna` you'll need:
+ [**Julia 1.7.0**](https://julialang.org/downloads/#current_stable_release) (or higher)
+ [**g++10**](https://askubuntu.com/questions/1192955/how-to-install-g-10-on-ubuntu-18-04) (or higher)
  - including `-fconcepts`
+ [**cmake 3.19**](https://cmake.org/download/) (or higher)
+ unix-based operating system

Currently, only g++10 is supported, clang support is planned in the future.

# Installation

`jluna` is header only, to add it to your application, simply use

```bash
git clone https://github.com/Clemapfel/jluna.git
```

or if you already have a library:
```bash
git submodule add https://github.com/Clemapfel/jluna.git
```

add the header to your C++ code:
```cpp
#include <jluna.hpp>
```

then link against `jluna` in your `CMakeList.txt` like so:
```CMAKE
include_directories("/path/to/.../jluna/")
add_executable(MY_EXECUTABLE path/to/my/main.cpp)
target_link_libraries(MY_EXECUTABLE jluna)
```
---

## Troubleshooting

### CMake cannot find Julia

`jluna` detects your Julia version and build parameters using the `julia` command in bash, if this command is not available on a system level, you will need to manually supply the path for the Julia executable to `jluna`. To do this:

Open `jluna/CMakeLists.txt` in an editor and modify the following statement in line 10:

```cmake
10: set(JULIA_EXECUTABLE /path/to/your/.../Julia/bin/Julia) # add the path to Julia here 
```

During `make` jluna should now be able to determine all the information to build jluna and link Julia properly

### State::initialize() fails

jluna assumes that `Julia` is installed on a system level. If this is not the case, you will need to manually specify the path to your image during the initialization step in C++. when calling `jluna::State::initialize()` at the start of your C++ main, instead of the no-argument call, use this overload and provide the full path to your Julia image like so:

```cpp
jluna::State::initialize("/path/to/your/.../Julia/bin");
```

Make sure that the image is uncompressed as `.zip` or `.tar` files cannot be used for initialization.

## Documentation

A full manual that introduces all major features in `jluna` step-by-step is available [here](/docs/docs.md). If you feel like you don't have the time or you're just here remind yourself of specific syntax, feel free to consult this example `main.cpp`:

```cpp
#include <jluna.hpp>

using namespace jluna;
int main()
{
  // always initialize first
  jluna::State::initialize();
}
```












