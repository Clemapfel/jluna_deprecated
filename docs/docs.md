# jluna: Documentation


# jluna::Proxy

## Introduction: Glossary

`Proxy<State>` is the most central class of `jluna` and most of it's C++-side functionality runs through this proxy class. Before we can discuss it, though, we need to get some nomenclature out of the way. As julia and C++ sometimes use different terms for different things (for example in C++ an "error" is always fatal while in julia an "error" such as `UndefVarError` can be catchable) it's best to get a baseline established as to not confuse some people:

+ A *value* is a chunk of memory holding arbitrary data, this memory of course has an *adress* (what would be a pointer in C/C++) which uniquely identifies the location of the memory, not it's content
+ A *name* is a julia-side value of type `Symbol`
+ A *variable*, then, is an association a name and a value. Associating an unnamed value with a name we'll call a *binding*. Unlike C++, in julia a variable name is not associated with an address but the value itself, two variables with different names but the same value usually point to the same address in memory. Consider the following example:

Consider the following example:

```cpp
jl_value_t* value_ptr = jl_eval_string("return 123");
```

Here, using the julia C-API, we created a chunk of memory holding the value `123`. If we want to access this value, we need to do so via the `value_ptr` which is best thought of as address of the value. If we now reassign `value_ptr`:

```cpp
value_ptr = // something else
```

There is still a chunk of memory holding the value `123`, we just have no way of remembering where exactly it is and in time (but not necessarily right after reassinging) the garbage collector will collect it. 

If we instead create a *variable* julia side:

```cpp
jl_value_t* value_ptr = jl_eval_string("return 123");

jl_eval_string("variable = 123");
jl_value_t* variable_value_ptr = jl_eval_string("return variable");

assert(value_ptr == variable_value_ptr); // does *not* raise an assertion
```

We still get a pointer and that pointer will still point to the original 123. This is important to realize, julias values are completely separate from julia variable names, two variable with different names can hold the same address of memory and an unbound address of memory can have valid data, be in scope, but have no julia-side name assigned to it.

Of course usually this doesn't matter, if there is valid memory that lingers around it will be caught by the garbage collector and deallocated, but this is where one of the most important features of `jluna::Proxy` comes in.

## Creating Proxies and memory management

A proxy in `jluna` is an implementation of the concept of a binding. Each proxy has two relevant properties:

+ i) it's value, which is a pointer `jl_value_t*` that is the address of julia-side memory
+ ii) it's name, which is a pointer to a julia-side symbol.

Crucially, while a proxies value always points to valid memory, the name is completely optional and is by converting kepts as a `nullptr` to signify that no name is assigned. Let's go back to our example from the section before, but this time we're using `jluna` rather than the C-API to access julias state:

```
auto no_name_proxy = State::script("return 123");

State::script("variable = 123");
auto named_proxy = Main["variable"];
std::cout << (no_name_proxy.operator jl_value_t*() == named_proxy.operator jl_value_t*()) << std::endl;

```

Here we generated two proxies (for end-users, this is usually the only way to create proxies as calling the CTORs manually can be quite cumbersome). Examining their properties we deduce from what we learned just now:

+ `no_name_proxy` has an address pointing to a julia-side `123` in memory and no name
+ `named_proxy` has an address pointing to a julia-side `123` and a julia-side name `:variable`

Becuase of how julia works, we can see if both proxies have the same primitive value by checking if they both point to the same memory:

```cpp
std::cout << (no_name_proxy.operator jl_value_t*() == named_proxy.operator jl_value_t*()) << std::endl;
```
``` 
1
```
Here we are explicitly casting both proxies to the address of their value, the comparing the c-pointers.




































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

## 1.1 C-API refresher

## 2. (Safely) executing code

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
unknown function (ip: 0x7fa109483a30)
unknown function (ip: 0x7fa10948f4fb)
_ZSt9terminatev at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
__cxa_throw at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
forward_last_exception at /home/clem/Workspace/jluna/./.src/exceptions.inl:29
main at /home/clem/Workspace/jluna/.test/main.cpp:16
__libc_start_main at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
_start at /home/clem/Workspace/jluna/cmake-build-debug/TEST (unknown line)
Allocations: 2722 (Pool: 2712; Big: 10); GC: 0

Process finished with exit code 134 (interrupted by signal 6: SIGABRT)
```

`forward_last_exception` checks the current julia state and if an exception is currently active, it forwards it to C++ withing a `JuliaException` object that is then properly thrown. While this is much nicer than just silently having things break in the background, we can see that `<no stacktrace available>` is displayed and the would furthermore manually need to call `forward_last_exception` anytime we execute julia code. To get C++-side exception identical to the great exception descriptions in julia itself, we need to call code through `jluna::State::safe_script`:

```cpp
jluna::State::safe_script("println(this_variable_is_undefined)");
```
```
exception in jluna::State::safe_script for expression:
"println(this_variable_is_undefined)"

terminate called after throwing an instance of 'jluna::JuliaException'
  what():  [JULIA][EXCEPTION] UndefVarError: this_variable_is_undefined not defined
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
unknown function (ip: 0x7fe1eaa49a30)
unknown function (ip: 0x7fe1eaa554fb)
_ZSt9terminatev at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
__cxa_throw at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
forward_last_exception at /home/clem/Workspace/jluna/./.src/exceptions.inl:38
safe_script at /home/clem/Workspace/jluna/./.src/state.inl:96
main at /home/clem/Workspace/jluna/.test/main.cpp:15
__libc_start_main at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
_start at /home/clem/Workspace/jluna/cmake-build-debug/TEST (unknown line)
Allocations: 798814 (Pool: 798379; Big: 435); GC: 1

Process finished with exit code 134 (interrupted by signal 6: SIGABRT)
```

When using `safe_script` we get a very verbose and useful error message, not only forwarding all julia-side error information but also printing the offending line of code as well as which line in C++ the exception was triggered from. This makes `safe_script` the superior way to call julia-code. 

## 3. Values & Proxies

When executing a line of code through `State`, the call to `safe_script` (which will be preferred to `script` henceforth) returns a class of type `jluna::Proxy`:

```cpp
auto result = State::safe_script("return 123");
```

`Proxy` is the most central class to jluna as it provides a way of interfacing with julia-side values in C++. It is best though of as the C++-equivalent of julias `Any`, because a proxy can bind to any julia value, be it a type, array, struct, singleton, function, usertype, etc.. 

As long as the proxy stays in scope C++-side, the julia-side memory it points to is guaranteed to not be freed by the garbage collector, regardless of whether it is in use julia-side. This is a huge advantage to the julia C-API and increases safety. Internally this is handled by keeping a reference to the value pointed to by the proxy in scope in a dictionary. When a proxy is destructed, that reference is released at which point it is completely up to julias discretion to garbage collect it.

## 3.1 (Un)Boxing

While some basic C-types can be exchanged directly, most values cannot just be transferred to julia as-is, they need to be transformed into a memory-layout julia understands. This is called *boxing*. Similarly, to acquire a value understandable to C++ from julia, we need to *unbox* it.

In `jluna` there are two concepts that describe types like these:

```cpp
template<typename T>
concept Boxable = requires(T t)
{
    {box(t)};
};

template<typename T>
concept Unboxable = requires(T t, jl_value_t* v)
{
    {unbox<T>(v)};
};
```

We see that for a type to be considered `Boxable`, there needs to be a function definition of the form `jl_value_t* box(T value)` available. Of course `jluna` provides such a function for all primitives, all `jluna` types ~~and most of the common `std::` types~~ (not yet implemented). 

Similarly for a type `T` to be considered `Unboxable`, a function definition of the form:

```cpp
template<typename U, std::enable_if_t<std::is_same_v<U, T>, bool> = true>
U unbox(jl_value_t*);
```

This definition uses [SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) to deduce which unbox overload to call for which type. The above definition will only be called for objects of type `T`.

We point these internal implementation details out explicitly because being `Boxable` and `Unboxable` are the only requirements of a type to be compatible with `jluna` and thus `julia`.

## 3.X Calling Julia Functions

Armed with a way to make C++-side values interpretable by julia, we can now use them for julia functions. There are two types of functions: `jluna::SafeFunction` which has full error handling and exception forward similar to `safe_script` and `jluna::Function` which has none of that, making it marginally faster to call.

We can access a function and store it in a C++ variable like this:

```cpp
auto println = State::get_safe_function("println", "Base");
// or 
jluna::SafeFunction println = State::safe_script("return Base.println");
// or 
jluna::SafeFunction println = jluna::Main["println"];

// then call it:
println(box<std::string>("this string was boxed manually"));
println("but we can also leave it out because it happens implicitly");
```
```
this string was boxed manually
but we can also leave it out because it happens implicitly
```

Indeed, the call operator of both functions will try their hardest to box whatever to hand them so it is usually unnecessary to manually call `box<T>` on each argument unless you want to invoke a specific method:

```cpp
State::safe_script(R"(
    f(_::Int64) = println("using 64-bit version")
    f(_::Int32) = println("using 32-bit version")
)");

auto f = State::get_safe_function("f", "Main");
uint8_t value = 0;

f(box<int64_t>(value));
f(box<int32_t>(value));
f(value);
```
```
using 64-bit version
using 32-bit version
terminate called after throwing an instance of 'jluna::JuliaException'
  what():  [JULIA][EXCEPTION] MethodError: no method matching f(::UInt8)
Closest candidates are:
  f(!Matched::Int64) at none:2
  f(!Matched::Int32) at none:3
Stacktrace:
 [1] safe_call(f::Function, args::UInt8)
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/julia/exception_handler.jl:75

signal (6): Aborted
```

`jluna::Function` and `SafeFunction` are a type of proxy

## 3.X Proxy Conversion

`jluna::Proxy` has the following operator:

```cpp
template<Unboxable T>
operator T() const;
```

This allows us to use the Proxy as if it was any unboxable type like so:

```cpp
auto this_should_be_an_int = State::safe_script("return 123");
int actually_an_int = this_should_be_an_int;
std::cout << actually_an_int << std::endl;
```
```
123
```

Unlike the C-API however, `jluna` will try it's best to make the cast possible. Using the same proxy above we can cast it to many different types no problem:

```cpp
std::string as_string = this_should_be_an_int; // calls julia-side Base.string()
char as_char = this_should_be_an_int;

jluna::Type as_type = this_should_be_an_int;
jluna::Module as_module = this_should_be_an_int;
jluna::Array<size_t, 3> as_3d_array = this_should_be_an_int;
```

All of these calls will compile just fine, however if we are trying to cast a value `v` to type `T` then the julia-side call `Base.convert(T, v)` must be successfull, otherwise a runtime assertion is raised. For example in the above example, `this_should_be_an_int` is of type `Int64` so casting it to type `Type`, type `Module` and type `Array{UInt64, 3}` are all invalid, while casting it to a `String` or `Char`, of course, works fine.

If we need to be syntactically more clear on which type we want a proxy to be cast to, the following methods are all exactly equivalent:

```cpp
auto proxy = State::safe_script("return /*...*/

auto as_t = proxy.operator T();
auto as_u = unbox<U>(proxy);
auto as_v = (v) proxy;
```

This way of casting can be useful when we don't want to open a whole new variable just for one casted result and will increase readability and brevity.

## 3.X Proxy Assignment and Mutation Julia-Side

We've seen how to convert from `Proxy` to `T`, however the other way around is maybe more important. For example, we can transform a value C++ side into a proxy like so:

```cpp
T our_value = T();
auto proxy = Proxy<State>(box<T>(our_value))
```

This expression boxes 'our_value' into a form julia can understand, allocates memory julia-side, moves our_value into said memory and then creates Proxy that takes ownership of the julia-side value, so it will stay in scope as long as it is use C++-side. 

We can now of course use this proxy as an argument for any C++ function, however how would we access it in julia? Let's consider an example:

```cpp
auto our_value = "we want to print this message via Base.println";
auto proxy = Proxy<State>(box<std::string>(our_value));
```

There are two ways to use this value, one way is to get `Base.println` from julia and call that function c++-side with 'our_value'



































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

            MyDatatype() = new(42)
            MyDatatype(a::Int64) = new(a)
    end)");
```

This type has two fields, `_field1` of type Int64 and `_field2` which is a reference to another datatype. It has two constructors, one "default constructor" (in C++ terminology) and one that wraps its second argument into a reference.

We can now instance this type and bind it to a `jluna::Proxy`:

```cpp
State::safe_script("instance = MyDatatype()");
auto instance = State::safe_script("return instance");
```
This proxy is not very useful because we have no way to access its fields. To do so we need to convert it to a `jluna::Struct` or `jluna::MutableStruct`, where mutable structs can bind to immutable proxies but immutable structs cannot bind to mutable proxies as this would break const-correctness.
We can then access fields using `operator[](const std::string&)`:

```cpp
MutableStruct as_struct = instance;
auto field_proxy = as_struct["_field1"];

std::cout << field_proxy.operator int() << std::endl;
```
```
42
```

`field_proxy` holds ownership of part of the memory of `instance`, this means assigning to `field_proxy` also modifies the field of it's owner `instance`:

```
field_proxy = 123;
State::safe_script("println(instance._field1)");
```
```
123
```

This means we have full access to any field and can modify to our hearts content, that is as long as the struct is mutable.

To close this section, while we intentionally spread things out for the sake of clarifying the way `operator[]` returns itself a proxy, the following syntax will often be more convenient and works just the same:

```cpp
MutableStruct as_struct = State::safe_script("return instance");
as_struct["_field1"] = 789;

State::safe_script("println(instance._field1)");
```
```
789
```



