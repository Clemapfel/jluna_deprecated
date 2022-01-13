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

## Memory Management

A proxy in `jluna` is an implementation of the concept of a binding. Each proxy has two relevant properties:

+ i) it's value, which is a pointer `jl_value_t*` that is the address of julia-side memory
+ ii) it's name, which is a pointer to a julia-side symbol.

Crucially, while a proxies value always points to valid memory, the name is *completely optional* and is by convention kept as a `nullptr` to signify that no name is assigned. Let's go back to our example from the section before, but this time we're using `jluna` rather than the C-API to access julias state:

```
auto no_name_proxy = State::script("return 123");

State::script("variable = 123");
auto named_proxy = Main["variable"];
std::cout << (no_name_proxy.operator jl_value_t*() == named_proxy.operator jl_value_t*()) << std::endl;
```

Here we generated two proxies (for end-users, this is usually the only way to create proxies as calling the CTORs manually can be quite cumbersome). Examining their properties we deduce from what we learned just now:

+ `no_name_proxy` has an address pointing to a julia-side `123` in memory and no name
+ `named_proxy` has an address pointing to a julia-side `123` and a julia-side name `:variable`

Because of how julia works and because `123` is a primitive, we can see if both proxies have the same value by checking if they both point to the same memory:

```cpp
std::cout << (no_name_proxy.operator jl_value_t*() == named_proxy.operator jl_value_t*()) << std::endl;
```
``` 
1
```
Here we are explicitly casting both proxies to the address of their value, then comparing the c-pointers and we see that, indeed, both proxies point to the same value.

Now for why this is important, **as long as there is any proxy in scope C++-side that points to a specific memory, that memory is kept safe from the garbage collector** and it will only be freed, once the last proxy that has ownership of it calls its destructor. 

This makes proxies great because we can handle julia-side values without every having to create a julia-side variable or if we're using a julia-side variable we don't have to micro-manage scoping or potentially controlling the garbage collector, all of this is done 100% safely by `jluna`. This includes deallocation and shutdown, one `exit(0)`, `jluna` will deallocate all proxies and make all values associated with them available to the garbage collector. This doesn't mean the julia-side values get destructed immediately, rather there is no longer a safeguard to prevent julia from doing so.

## Mutating Values

We went in detail about the inner workings of how the proxy handles things because modifying the proxies values can be quite confusing without context, however in context it is consistent and clear. Consider the following:

```cpp
State::script("variable = 123");
auto proxy = Main["variable"];
State::script("variable = 456");

std::cout << (int) proxy << std::endl;
```
Here we're creating a julia-variable directly in julia and assign it the value `123`. We then create a name proxy with the same value. After reassinging a new value to the variable `julia` side, what will the proxies value be?

```
456
```
Predictably, we see that the proxies value also changed

TODO

## Accessing Fields and the (.) operator

As a proxy can hold any value, it may also hold a value which is a `structtype`, which may or may not have *fields*:

```cpp
State::safe_script(R"(

mutable struct Inner
    _inner_field
end

struct Outer
    _outer_field::Inner
end

instance = Outer(Inner(123))
")");

auto instance = Main["instance"];
```

Here we create two structs, `Outer` which is immutable has only one field which is of type `Inner`, also a structtype however this one is mutable and has again one field of arbitrary type. 

We can then access the value in instances field like so:

```cpp
auto outer_field = instance["_outer_field"];
auto inner_field = outer_field["_inner_field"];
// or chaining is also fine
auto inner_field = instance["_outer_field"]["_inner_field"];
```

`operator[](const std::string&)` itself returns a proxy which may itself be a structtype. Because of this chaining multiple `[]` after each other is valid syntax and we've been using it all throughout this tutorial already. You may have also already noticed that we also use `[]` on `Main` which is of course Module. Why is that?

To approximate julias syntax, `jluna`s `operator[]` was decided to be basically equivalent to the dot operator in julia:

```cpp
// julia:
auto res = State::script("Main.instance._outer_field._inner_field");

// cpp:
auto res = Main["instance"]["_outer_field"]["_inner_field"];
```
This means any class where the dot operator would be well-defined, `operator[]` also works on which includes modules. This, combined with arrays, makes long chains of proxies of different type (module, struct, array) possible (if discourage for legibility). Indeed, the proxy will remember it's entire name as we can verify with `get_name`.

```cpp
auto res = Main["instance"]["_outer_field"]["_inner_field"];
std::cout << res.get_name() << std::endl;
```
```
instance._outer_field._inner_field
```

Where the starting `Main.` is implicit and thus not displayed manually.

## Functions

`jluna::Proxy` has quite a lot of features but one it doesn't sport out-of-the-box is being callable:

```cpp
Function f = State::script("f(x) = sqrt(x^x^x)");
f(2);
```
```
/home/Workspace/jluna/.test/main.cpp: In function ‘int main()’:
/home/Workspace/jluna/.test/main.cpp:21:4: error: no match for call to ‘(jluna::Proxy<jluna::State>) (int)’
   21 | f(2);
      |    ^
```

To wrap a function in a way where we can call it from C++, we need to instead assign it to an object of type `jluna::Function` or `jluna::SafeFunction`:

```cpp
Function f = State::script("f(x) = sqrt(x^x^x)");
std::cout << (int) f(2) << std::endl;
```
```
4
```
Any `Boxable` argument can be used directly as arguments for `jluna::Function`, this includes other proxies including proxy functions. If a type does not fulfill the constraints of boxable you will need to manually convert it into a `jl_value_t*` using the C-API or defined your own `box(T)` functino. 

Other than the callability, function proxies are still proxies of course so all the functionality detailed so far is also available to them. 

Similar to `State::safe_script`, `SafeFunction` wraps the function call in a marginally slower but much safer, exception-forwarding call:

```cpp
Function f = State::script("f(x) = sqrt(x^x^x)");
std::cout << (int) f(-1) << std::endl;
```
```
terminate called after throwing an instance of 'jluna::JuliaException'
  what():  DomainError with -1.0:
sqrt will only return a complex result if called with a complex argument. Try sqrt(Complex(x)).
Stacktrace:
 [1] throw_complex_domainerror(f::Symbol, x::Float64)
   @ Base.Math ./math.jl:33
 [2] sqrt
   @ ./math.jl:567 [inlined]
 [3] sqrt
   @ ./math.jl:1221 [inlined]
 [4] f(x::Int32)
   @ Main ./none:1
 [5] safe_call(f::Function, args::Int32)
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/julia/exception_handler.jl:75

signal (6): Aborted
in expression starting at none:0
gsignal at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
abort at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
unknown function (ip: 0x7f9a5afe8a30)
unknown function (ip: 0x7f9a5aff44fb)
_ZSt9terminatev at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
__cxa_throw at /lib/x86_64-linux-gnu/libstdc++.so.6 (unknown line)
forward_last_exception at /home/clem/Workspace/jluna/./.src/exceptions.inl:39
safe_call<int> at /home/clem/Workspace/jluna/./.src/state.inl:157
operator()<int> at /home/clem/Workspace/jluna/./.src/function_proxy.inl:72
main at /home/clem/Workspace/jluna/.test/main.cpp:21
__libc_start_main at /lib/x86_64-linux-gnu/libc.so.6 (unknown line)
_start at /home/clem/Workspace/jluna/cmake-build-debug/TEST (unknown line)
Allocations: 1613608 (Pool: 1612710; Big: 898); GC: 2
```

It should thus be preferred to `Function` in any situation that isn't bleeding edge performance critical. 

## Arrays

Julias arrays are quite powerful and fully featured, it is thus only natural that a julia-binding library should have an intuitive and similarly fully features Array wrapper. In `jluna`, this is done by `jluna::Array<T, R>` where `T` is the value type and `R` is the *rank* (or dimensionality) of the array. This is directly equivalent to how julia declares its array types:

| Julia          | jluna         |
|----------------|---------------|
| `Vector{T}`    | `Vector<T>`   |
| `Matrix{T}`    | `Array<T, 2>` |
| `Array{T, R}`  | `Array<T, R>` | 

For any R in {1, 2, ...}. Note that `jluna::Array<T, 1>` is not the same as `jluna::Vector<T, 1>`, `Vector` actually inherits from `Array<T, 1>` and while it is functionally equivalent, it has the following additional functions:

```
void insert(size_t pos, Value_t value);
// equivalent to insert!(this, pos, value)

void erase(size_t pos);
// equivalent to deleteat!(this, pos)

template<Boxable T = Value_t>
void push_front(T value);
// equivalent to insert(this, 0, value)

template<Boxable T = Value_t>
void push_back(T value);
// equivalent to push!(this, value)
```

### Index Access

There are two way to access an element in any array, *linear indexing* and *multidimensional indexing*:

```cpp
State::safe_script(R"(
    vec_1d = [1, 2, 3, 4, 5, 6]
    arr_3d = Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))
)");

Vector<Int64> vec = Main["vec_1d"];
Array<Int64, 3> arr = Main["arr"];

// linear indexing
std::cout << vec[3] << "\n";
std::cout << arr[12] << "\n";

// multi-dimensional indexing
std::cout << vec.at(0) << "\n";
std::cout << arr.at(0, 1, 2) << std::endl;
```
```
vector: [1, 2, 3, 4, 5, 6]
array : [1 4 7; 2 5 8; 3 6 9;;; 10 13 16; 11 14 17; 12 15 18;;; 19 22 25; 20 23 26; 21 24 27]
4
13
1
22
```

We see that C++-side, indexing is 0-based. This is in keeping with the Julia C-library and should be kept in mind, for example to get the 22ths element of `arr` in C++, we use `arr.at(0, 1, 2)` but in Julia we would use `getindex(arr, [1, 2, 3])`.

### Iteration

`jluna`s array and it's daughters are iterable, just like most std::containers. We can do so using the C++ for-in loop:

```cpp
int sum = 0;
for (int i : vec)
    sum += i;

std::cout << sum << std::endl;
```
```
21
```

However `jluna` also offers a special iterator class that can be accessed by declaring the iterating variable as `auto` (*not* `auto&`). This iterator can then be assigned to which will change the corresponding value julia-side as it is always mutating. If we do not want to mutate the julia-side variable for some reason, we can instead cast the iterator to `Proxy<State>` which will only mutate it's corresponding julia value after `make_mutating`:

```cpp
for (auto it : vec)
{
    it = 6;
    std::cout << it.operator Proxy<State>().get_name() << std::endl;
}

std::cout << (std::string) vector << std::endl;
```
```
vec_1d[1]
vec_1d[2]
vec_1d[3]
vec_1d[4]
vec_1d[5]
vec_1d[6]
[6, 6, 6, 6, 6, 6]
```

Because the iterator can become a proxy, the arrays value type is truly aribtrary and each element offers the same functionality as any other proxy:
```




        
        







































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



