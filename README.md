# jluna: A modern Julia ⭤ C++ Wrapper API (v0.5)

Heavily inspired in design and syntax by [**sol3**](https://github.com/ThePhD/sol2), `jluna` aims to fully replace the official julia C-API in usage for C++ projects.

Some advantages `jluna` has over the C-API:
+ automatically detects and links julia during make
+ expressive, generic syntax
+ mutating C++-side proxies also assigns the corresponding variable julia-side
+ exceptions, including exception forwarding from julia
+ wraps many C++ std objects, offers extension to user types
+ any value in use C++-side is safe from the garbage collector
+ safety can be sacrificed to achieve identical performance
+ mixing the C-API and `jluna` is no problem

## Examples:

### Calling Julia Functions
```cpp
jluna::Function println = jluna::Base["println"];
println(std::string("this is a string "), "\n", 
        println, "\n",
        int(3), "\n",
        State::script("return [1, 2, 3]"));
```
```
this is a string 
println
3
[1, 2, 3]
```

### Modifying Julia-Side Variables

```cpp
using namespace jluna;

State::safe_script(R"(
    mutable struct MutableType
        _field
        MutableType() = new(undef)
    end

    instance = MutableType()
)");

State::safe_script("println(\"before: \", instance._field)");

auto field = jluna::Main["instance"]["_field"];
make_mutating(field);
field = 123;

State::safe_script("println(\"after: \", instance._field)");
```
```
before: UndefInitializer()
after: 123
```

### Exception Forwarding
```cpp
jluna::State::safe_script("return this_value_is_undefined");
```
```
exception in jluna::State::safe_script for expression:
"return this_value_is_undefined"

terminate called after throwing an instance of 'jluna::JuliaException'
  what():  [JULIA][EXCEPTION] UndefVarError: this_value_is_undefined not defined
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



