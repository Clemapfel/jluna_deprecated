# jluna: A modern Julia ⭤ C++ Wrapper API (v0.5)

Julia is a beautiful language, it is well-designed and well-documented. Julias C-API is less beautiful and much less... documented. Heavily inspired in design and syntax by [**sol3**](https://github.com/ThePhD/sol2), `jluna` aims to fully replace the official julia C-API in usage in C++ projects and makes accessing Julias unique strengths through C++ easy and hassle-free.

Some advantages `jluna` has over the C-API:
+ automatically detects and links julia during make
+ expressive, generic syntax
+ mutating C++-side proxies also assigns the corresponding variable julia-side if desired
+ expressive exceptions, including exception forwarding from julia
+ wraps many C++ std objects and types
+ any value in use C++-side is safe from the garbage collector
+ mixing the C-API and `jluna` is no problem, for example for optimal performance
+ `jluna` is fully documented, including inline documentation for IDEs and tutorials

## Features:

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
using namespace jluna;

State::safe_script(R"(
    module MyModule
        mutable struct MutableType
            _field
            MutableType() = new(undef)
        end

        instance = MutableType()
    end
)");

State::safe_script(R"(println("before: ", MyModule.instance._field))");

auto field = Main["MyModule"]["instance"]["_field"];
make_mutating(field);
field = 123;

State::safe_script(R"(println("after: ", MyModule.instance._field))");
```
```
before: UndefInitializer()
after: 123
```

### Mulit-Dimensional Arrays
```cpp
jluna::Array<size_t, 3> array = State::script("Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3)");
        
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

`jluna` aims to be as modern as practical. It thus uses mostly modern C++20 features and aims to support the newest Julia version rather than focusing on backwards compatibility.

You'll need:
+ **Julia 1.7.0** (or higher)
  - no 2nd or 3rd party modules are needed
+ **g++10** (or higher)
+ unix-based operating system
+ **cmake 3.19** (or higher)

Currently only g++10 is supported as many of the C++20 features are not yet implemented on other compilers.



