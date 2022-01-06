# jluna: Documentation

This tutorial will give a brief overview of most of `jluna`s functionalities

## 1. Initialization

Before any julia-side computation can happen, we need to initiliaze the julia state. We do this using:

```cpp
#include <jluna.hpp>

int main()
{
    jluna::State::initialize();
}
```
```
[JULIA][LOG] initialization successfull.
```

Only after this call, all functionality will be available so make sure to always initialize it first. If `#ifdef NDEBUG` evaluates to true, an assertion is raised when trying to use `jluna` before it is initialized. 
You don't have to worry about "shutting down", all cleaning up and proper deallocation julia-side is done automatically on exit. If you wish to do so manually anyway, you do so via

```cpp
jluna::State::shutdown();
```
```
[JULIA][LOG] Shutting Down...
```

However, this is usually unnecessary.

## X. (Safely) executing code

All interaction with julia happens in some way through `jluna::State`. This is a C++ union class with only static methods. This means there is only ever exactly one state and it cannot be instanced, in julia we would call this a singleton.

To start out let us call a single line of julia code:

```cpp
jluna::State::script("println(\"hello luna\")");
```
```
hello luna
```

One of the most central features of `jluna` is safety, so we will address this as early as possible. What happens if you execute a command (on purpose or by accident) that contains a syntax error?

```cpp
jluna::State::script("println(\"\"clearly something is wrong\"");
```
```

```
Nothing happens. This is because `State::script` offers no additional safety features over julias C-API `jl_eval_string`, it is exactly equivalent.  
To ammeliate this, `jluna` offers two ways of safely execute arbitrary lines of code. The first involves calling `forward_last_exception()` from the header `jluna/include/exceptions.hpp` right after code is run

```cpp
jluna::State::script("println(\"\"clearly something is wrong\"");
forward_last_exception();
```
```
terminate called after throwing an instance of 'jluna::JuliaException'
  what():  [JULIA][EXCEPTION] ErrorException("syntax: cannot juxtapose string literal")
Stacktrace: <no stacktrace available>


signal (6): Aborted
in expression starting at none:0
gsignal at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
abort at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
unknown function (ip: 0x7f48deb04a30)
unknown function (ip: 0x7f48deb104fb)
_ZSt9terminatev at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
__cxa_throw at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
forward_last_exception at /home/clem/Workspace/jluna/./.src/exceptions.inl:22
main at /home/clem/Workspace/jluna/.test/main.cpp:15
__libc_start_main at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
_start at /home/clem/Workspace/jluna/cmake-build-debug/TEST (unknown line)
Allocations: 2720 (Pool: 2712; Big: 8); GC: 0

Process finished with exit code 134 (interrupted by signal 6: SIGABRT)
```

`forward_last_exception` checks the current julia state and if an exception is currently active, it forwards it to C++ withing a `JuliaException` object that is then properly thrown. While this is much nicer than just silently having things break in the background, we can see that `<no stacktrace available>` is displayed and the would furthermore manually need to call `forward_last_exception` anytime we execute julia code. To get C++-side exception identical to the great exception descriptions in julia itself, we need to call code through `jluna::State::safe_script`:

```cpp
jluna::State::safe_script("println(this_variable_is_undefined)");
```
```
terminate called after throwing an instance of 'jluna::JuliaException'
  what():  [JULIA][EXCEPTION] UndefVarError: this_variable_is_undefined not defined
Stacktrace:
 [1] top-level scope
   @ none:1
 [2] eval
   @ ./boot.jl:373 [inlined]
 [3] eval
   @ ./client.jl:453 [inlined]
 [4] safe_call(command::String)
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/julia/exception_handler.jl:36
 [5] top-level scope
   @ none:1

signal (6): Aborted
in expression starting at none:0
gsignal at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
abort at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
unknown function (ip: 0x7f04b33b2a30)
unknown function (ip: 0x7f04b33be4fb)
_ZSt9terminatev at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
__cxa_throw at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
forward_last_exception at /home/clem/Workspace/jluna/./.src/exceptions.inl:31
safe_script at /home/clem/Workspace/jluna/./.src/state.inl:107
main at /home/clem/Workspace/jluna/.test/main.cpp:14
__libc_start_main at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
_start at /home/clem/Workspace/jluna/cmake-build-debug/TEST (unknown line)
Allocations: 800005 (Pool: 799570; Big: 435); GC: 1

Process finished with exit code 134 (interrupted by signal 6: SIGABRT)
```

Through `safe_script` we forward all information necessary to C++, making this way of calling julia functions well worth the slight performance overhead from the exception handling.

## X. Proxies

In `jluna`, julia-side values are managed by a proxy class. This calls has memory-ownership over whatever it is attached to, this means as long as the proxy remains in scope C++-wise, it's attached value is safe to access and safe from being garbage collected. As soon as all proxies that hold ownership over a certain value have called their destructors, the value is free to be garbage collected at julias convenience. This means temporary values that, in the REPL, would usually be unnaccessible can be stored in C++:

```cpp
auto temp_value = State::safe_script("return 123");
```

Again, as long as `temp_value` remains in scope, the pointer to the memory that the julia-side integer `123` is stored in will remain safe to access.

Conceptually we know that this value should be an integer but to actually use it as such, we need to cast it to int:

```cpp
int as_int_01 = temp_value;
int as_int_02 = temp_value.operator int();
int as_int_03 = (int) temp_value;
```

Note how we stated `auto` rather than `auto&` or `auto*`. This is important because the proxy is not a reference or pointer to the julia memory, it is more like an iterator, it knows which value belongs to it but acts completely independently otherwise. Thus:

```cpp
auto first_proxy = State::safe_script("return 123");
auto second_proxy = State::
```

TODO: boxable
// in proxy.hpp
template<typename T>
concept Boxable = requires(T t, jl_value_t* v)
{
    {box(v)};
};

TODO

While the more general Proxy class provides a number of features, it does not offers the exact same functionality through members as julia-side values do. To address this, `jluna` has a special proxy class for each major type of julia object, most of these inherit from jluna::Proxy and thus have the same features as described above, along with additional member methods

## X.Y Function Proxies

Function proxies attach to julia-side objects of type `T where T <: Function`. There are two types of Function, `jluna::Function` and `jluna::SafeFunction`.

```cpp
State::safe_script("function my_function() println(\"I'm a function\") end");
jluna::Function as_function = State::safe_script("return my_function");
```

As with any of the proxies, if the value being attached to proxy of type `jluna::Function` is not a function, an assertion is raised if in debug mode. 

Functions only sport one additional member function over regular proxies:

```cpp
// in function.hpp, member of Function/SafeFunction:
template<Boxable... Args_t>
auto operator()(Args_t&&...);
```

We see that each function can be called via `operator()`. Any type that fullfills the concept `Boxable` which we recognize from the previous section. If we want to call `my_function` from C++, we can do so by simply using:

```cpp
as_function();
```
```
I'm a function
```

Of course one of the most central aspects to julia is *multiple dispatch* and indeed, an object of type `jluna::Function` hold a reference to a julia function, not just a method. This means if we define an additional method, julia will call the appropriate method just as it would if we were using only julia:

```cpp
State::safe_script(R"(
    function my_function() ::Nothing
        println("I'm a function")
        return nothing;
    end)");

State::safe_script(R"(
    function my_function(b::Bool) ::Nothing
        if !b
            println("I'm a dangerous function")
        else
            throw(ErrorException("I told you I was dangerous"))
        end
        return nothing;
    end)");

Function unsafe_function = State::safe_script("return my_function");
SafeFunction safe_function = State::safe_script("return my_function");

unsafe_function();
safe_function(false);
safe_function(true);
```
```
I'm a function 
I'm a dangerous function

terminate called after throwing an instance of 'jluna::JuliaException'
  what():  [JULIA][EXCEPTION] I told you I was dangerous
Stacktrace:
 [1] my_function(b::Bool)
   @ Main ./none:6
 [2] safe_call(f::Function, args::Bool)
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/julia/exception_handler.jl:57

signal (6): Aborted
in expression starting at (...)
```

This may throw up the question: How can we make sure which method is called? Consider the following:

```cpp
State::safe_script(R"(float_function(f::Float32) = println("float 32 version called"))");
State::safe_script(R"(float_function(f::Float64) = println("float 64 version called"))");

SafeFunction f = State::safe_script("return float_function");
f(1.);
```
Which version will this function call? In our case it will call the Float64 version as the literal `1.` is always defined as a `double` in C++ whos julia-equivalent is, of course, Float64. While this may seem obvious to some, in more complicated type-dependency scenarios it may not be quite so easy and either way it can be considered bad style. To make sure julia treats the value as the type we want it to, `jluna` provides the following box overload:

```
template<typename From, typename To>
concept CastableTo = requires(From t)
{
    {static_cast<To>(t)};
};

template<typename Return_t, CastableTo<Return_t> Arg_t>
jl_value_t* box(Arg_t t)
{
    return box(static_cast<Return_t>(t));
}
```
This means we can call `box<T>(arg)` if `decltype(arg)` can be statically cast to `T`. Using this overload we can make sure the julia function arguments are treated correctly:

```cpp
SafeFunction f = State::safe_script("return float_function");
f(box<float>(int(1));
f(box<double>(size_t(-1.0));
```
```
float 32 version called
float 64 version called
```

As mentioned in the Proxy section, any class implementing `Boxable` can be used as function arguments. This includes all primitive C++ types, any `jluna` type inherting from `jluna::Proxy`, `jluna::JuliaException` as well as memory pointed to by a `jl_value_t*`, `jl_array_t*`, `jl_module_t*`, `jl_datatype_t*`, `jl_sym_t*` or similar julia C-API types.

## X. Struct Proxy

If a proxy is a `struct` or `mutable struct` julia-side, we can assign a `jluna::Proxy` attached to it to `jluna::Struct` or `jluna::MutableStruct` respectively. First, let's define our own new datatype:

```cpp
State::safe_script(R"(
    mutable struct MyDatatype
            _field1::Int64
            _field2::Ref{MyDatatype}

            MyDatatype() = new(42, Ref{MyDatatype}())
            MyDatatype(a::Int64, b::MyDatatype) = new(a, Ref(b))
    end)");
```

This type has two fields, `_field1` of type Int64 and `_field2` which is a reference to another datatype. It has two constructors, one "default constructor" (in C++ terminology) and one that wraps its second argument into a reference.

We can now instance this type and bind it to a `jluna::Proxy`:

```cpp
State::safe_script("instance = MyDatatype()")
auto instance = State::safe_script("return instance");
```
This proxy is not very useful because we have no way to access its fields. To do so we need to convert it to a `jluna::Struct` or `jluna::MutableStruct`, where mutable structs can bind to immutable proxies but immutable structs cannot bind to mutable proxies as this would break const-correctness.
We can then access fields using `operator[](const std::string&)`:

```cpp
MutableStruct as_struct = instance;
auto field1_proxy = as_struct["_field1"];
```

Accessing fields this way returns a new `jluna::Proxy` that points to the data in the owners field. This means anything returned by `operator[]` can be converted just like a proxy optained via `script` can be, but we furthermore can assign such a proxy and it will modify it's original owners field values:

```
MutableStruct as_struct = instance;
auto field1_proxy = as_struct["_field1"];
std::cout << field1_proxy.operator int() << std::endl;
field1_proxy = 43;




