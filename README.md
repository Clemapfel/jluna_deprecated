# jluna: A modern Julia ⭤ C++ Wrapper API (v0.5)

Julia is a beautiful language, it is well-designed and well-documented. Julias C-API is less beautiful and much less... documented. Heavily inspired in design and syntax by [**sol3**](https://github.com/ThePhD/sol2), `jluna` aims to fully replace the official julia C-API in usage in C++ projects and makes accessing Julias unique strengths through C++ easy and hassle-free.

Some advantages `jluna` has over the C-API:
+ automatically detects and links julia during make
+ expressive, generic syntax
+ mutating C++-side proxies also assigns the corresponding variable julia-side if desired
+ verbose exceptions, including exception forwarding from julia
+ wraps many C++ std objects and types
+ as long as a proxy managing julia-side memory is in scope C++-side, it is safe from the julia garbage collector
+ `jluna` is fully documented, including inline documentation for IDEs and tutorials
+ if you really want to go back, mixing the C-API and `jluna` is no problem

## Features:

### Accessing Julia-Side Variables
```cpp
jluna::State::safe_script(R"(
    module MyModule
        mutable struct MutableType
            _field
        end

        instance = MutableType(123)
    end
)");

std::cout << (int) jluna::Main["MyModule"]["instance"]["_field"] << std::endl;
```
```
123
```

### Calling Julia Functions
```cpp
jluna::Function println = jluna::Base["println"];
println(std::string("this is a string "), "\n", 
        println, "\n",
        int(42), "\n",
        State::script("return [1, 2, 3]"));
```
```
this is a string 
println
42
[1, 2, 3]
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

auto field = jluna::Main["MyModule"]["instance"]["_field"];

// by design, julia-side mutation needs to be explicitly turned on
jluna::make_mutating(field);
field = 456;

State::safe_script(R"(println("after: ", MyModule.instance._field))");
```
```
before: 123
after: 456
```

### Mulit-Dimensional Arrays
```cpp
jluna::Array<size_t, 3> array = jluna::State::script("Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))");
        
auto value = array.at(0, 1, 2); // 0-based indexing
std::cout << value << std::endl; 

jluna::make_mutating(value);
value = 9999;
jluna::Base["println"].as<Function>()(array);
```
```
22
[1 4 7; 2 5 8; 3 6 9;;; 10 13 16; 11 14 17; 12 15 18;;; 19 9999 25; 20 23 26; 21 24 27]
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
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/julia/exception_handler.jl:39
 [5] safe_call(expr::Expr)
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/julia/exception_handler.jl:33
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
forward_last_exception at /home/clem/Workspace/jluna/./.src/exceptions.inl:39
safe_script at /home/clem/Workspace/jluna/./.src/state.inl:101
main at /home/clem/Workspace/jluna/.test/main.cpp:16
__libc_start_main at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
_start at /home/clem/Workspace/jluna/cmake-build-debug/TEST (unknown line)
Allocations: 1624652 (Pool: 1623704; Big: 948); GC: 2

Process finished with exit code 134 (interrupted by signal 6: SIGABRT)
```
### And more!

### Planned (but not yet implemented):
In order of priority, highest first:
+ creating new modules and types completely C++-Side
+ expression proxy, access to meta features via C++
+ julia-side wrapper for C++ Functions, similar to `@ccall`
+ save-states, restoring a previous julia state
+ clang support
+ julia-side macro expansion during C++ compile time 

---

# Dependencies

`jluna` aims to be as modern as is practical. It uses C++20 features extensively and aims to support the newest Julia version, rather than focusing on backwards compatibility. If you are looking for a C++ library that supports julia 1.5 or lower, consider checking out [CxxWrap](https://github.com/JuliaInterop/CxxWrap.jl) instead.

For `jluna` you'll need:
+ **Julia 1.7.0** (or higher)
+ **g++10** (or higher)
  - including `-fconcepts`
+ **cmake 3.19** (or higher)
+ unix-based operating system

Currently only g++10 is supported though Clang support is planned in the future.

# Installation

`jluna` is header only, to add it to your application, simply use

```bash
git clone https://github.com/Clemapfel/jluna.git
```

or if you already have a library:
```bash
git submodule add https://github.com/Clemapfel/jluna.git
```

then link against `jluna` in your `CMakeList.txt` like so:
```CMAKE
include_directories("/path/to/.../jluna/")
add_executable(MY_EXECUTABLE path/to/my/main.cpp)
target_link_libraries(MY_EXECUTABLE jluna)
```
---

## Troubleshooting

### cmake julia executable cannot find julia

`jluna` detects your julia version and build parameters using the `julia` command in bash, if this command is not available on a system level, you will need to manually supply the path for the julia executable to `jluna`.

Open `jluna/CMakeLists.txt` in an editor and modify the following statement in line 10:

```cmake
10: set(JULIA_EXECUTABLE /path/to/your/.../julia/bin/julia) # add the path to julia here 
```

During `make` jluna should now be able to determine all the information to build jluna and link julia properly

### State::initialize() fails

jluna assumes that `julia` is installed on a system level. If this is not the case, you will need to manually specify the path to your image during the initialization step in C++. when calling `jluna::State::initialize()` at the start of your C++ main, instead of the no-argument call, use this overload and provide the full path to your julia image like so:

```cpp
jluna::State::initialize("/path/to/your/.../julia/bin");
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












