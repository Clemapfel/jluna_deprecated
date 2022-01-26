# jluna: An Introduction

This page will give an abbreviated overview of most of `jluna`s relevant features and syntax. While it is useful for referencing back to, and as an introduction to `jluna`, it should not be considered a proper manual. 

### Table Of Contents

Please navigate to the appropriate section by clicking the links below:

1. [Initialization & Shutdown](#initialization)<br>
2. [Executing Code](#executing-code)<br>
3. [Controlling the Garbage Collector](#garbage-collector-gc)<br>
  3.1 [Enabling/Disabling GC](#enablingdisabling-gc)<br>
  3.2 [Manual Collection](#manually-triggering-gc)<br>
4. [Boxing / Unboxing](#boxing--unboxing)<br>
  4.1 [Manual](#manual-unboxing)<br>
  4.2 [(Un)Boxable as Concepts](#concepts)<br>
  4.3 [List of (Un)Boxables](#list-of-unboxables)<br>
5. [Accessing Variables through Proxies](#accessing-variables)<br>
  5.1 [Mutating Variables](#mutating-variables)<br>
  5.2 [Accessing Fields](#accessing-fields)<br>
  5.3 [Named Proxies](#named-proxies)<br>
  5.4 [Unnamed Proxies](#unnamed-proxy)<br>
  5.5 [Detached Proxies](#detached-proxies)<br>
  5.6 [Making a Named Proxy Unnamed](#making-a-named-proxy-unnamed)<br>
6. [Functions](#functions)<br>
   6.1 [Accessing julia Functions from C++](#accessing-julia-functions)<br>
   6.2 [Calling julia Functions from C++](#calling-julia-functions)<br>
   6.3 [Accessing C++ Functions from julia](#registering-functions)<br>
   6.4 [Calling C++ Functions from julia](#calling-c-functions-from-julia)<br>
   6.5 [Allowed Function Names](#allowed-function-names)<br>
   6.6 [Allowed Function Signatures](#possible-signatures)<br>
   6.7 [Using arbitrary Objects in julia Functions](#using-non-julia-objects-in-functions)<br>
7. [Arrays](#arrays)<br>
  7.1 [Constructing Arrays](#ctors)<br>
  7.2 [Indexing](#indexing)<br>
  7.3 [Iterating](#iterating)<br>
  7.4 [Vectors](#vectors)
8. [~~Expressions~~](#expressions)<br>
9. [~~Usertypes~~](#usertypes)<br>
10. [C-API](#c-api)<br>
  10.1 [Meaning of C-Types](#meaning-of-c-types)<br>
  10.2 [Executing Code](#executing-code)<br>
  10.3 [Forwarding Exceptions](#forwarding-exceptions-in-c)<br>
  10.4 [Accessing Values](#accessing-values-in-c)<br>
  10.5 [Functions](#functions-in-c)<br>
  10.5.1 [Accessing Functions](#accessing-functions-in-c)<br>
  10.5.2 [Calling Functions](#calling-functions-in-c)<br>
  10.6 [Arrays](#arrays-in-c)<br>
  10.6.1 [Accessing & Indexing Arrays](#accessing--indexing-arrays-in-c)<br>
  10.6.2 [Mutating Arrays](#mutating-arrays-in-c)<br>
  10.7 [Strings](#accessing-strings-in-c)<br>
  10.8 [Initialization & Shutdown](#initialization--shutdown-in-c)

## Initialization

Before any interaction with `jluna` or julia can take place, the julia state needs to be initialized:

```cpp
#include <jluna.hpp>

using namespace jluna;
int main() 
{
    State::initialize();
    
    // all your application here
    
    State::shutdown();
}
```

When a program exits regularly, all julia-side values allocated through `jluna` are safely deallocated. On an exit with a code other than `0`, it is recommended to manually call `State::shutdown()`.

## Executing Code

`jluna` has two ways of executing code (represented C++ side as string): *with* exception forwarding and *without* exception forwarding. Code called without exception forwarding will not only not report any errors, but simply appear to "do nothing". If a fatal error occurrs, the entire application wil crash without warning. <br>
Because of this, it is highly recommended to always air on the side of safety by using the `safe_` overloads whenever possible:

```cpp
// without exception forwarding
State::script("your unsafe inline code");
State::script(R"(
    your
    multi-line
    code
)");
jl_eval_string("your unsafe code");

// with exception forwarding
State::safe_script("your safe inline code");
State::safe_script(R"(
    your
    safe
    multi-line
    code
)");
```

## Garbage Collector (GC)

The julia-side garbage collector operates completely independently, just like it would in a pure julia program. However, sometimes it is necessary to disable or control its behavior manually. To do this, `jluna::State` offers the following member functions:

#### Enabling/Disabling GC
```cpp
State::set_garbage_collector_enabled(bool);
```

#### Manually Triggering GC
```cpp
State::collect_garbage();
// always works, if currently disabled, enables, then collects, then disables again
```
#### Checking if the GC is Enabled
```cpp
bool State::is_garbage_collector_enabled();
```

When using `jluna` and not pure C-API, most objects are safe from being garbage collected. It is therefore rarely necessary to manually disable the GC. See the section on [proxies](#accessing-variables) for more information.

## Boxing / Unboxing

Julia and C++ do not share any memory. Objects that have the same conceptual type can have very different memory layouts. For example, `Char` in julia is a 32-bit value, while it is 8-bits in C++. Comparing `std::set` to `Base.set` will of course be even more of a difference.<br>
Because of this, when transferring memory from one languages state to the others, we're not only moving memory but converting it by reformating its layout. 

**Boxing** is the process of taking C++-side memory, converting it and then allocating the now julia-compatible memory julia-side. Conversely, **unboxing** is the process of taking julia-side memory, converting it and then allocating it C++-side. Boxing/Unboxing are handled by an overload corresponding of the following functions:

```cpp
template<typename T>
jl_value_t* box(T);

template<typename T>
T unbox(jl_value_t*);
```
where `jl_value_t*` is an address of julia-side memory.


All box/unbox functions have exactly this signature. For `unbox<T>`, ambiguity C++-side is resolved using [SFINAE](https://en.cppreference.com/w/cpp/types/enable_if):

```cpp
// declaring box/unbox for fictional type "MyType"

jl_value_t* box(MyType value)
{
    /*...*/
}

template<typename T, std::enable_if_t<std::is_same_v<T, MyType>, bool> = true>
T unbox(jl_value_t* value)
{
    /*...*/
}
```

### Concepts
The property of being (un)boxable is represented in C++ as two concepts:

```cpp
// an "unboxable" is any T for whom unbox<T>(jl_value_t*) -> T is defined
template<typename T>
concept Unboxable = requires(T t, jl_value_t* v)
{
    {unbox<T>(v)};
};

/// a "boxable" is any T for whom box(T) -> jl_value_t* is defined
template<typename T>
concept Boxable = requires(T t)
{
    {box(t)};
};
```
Given this, we can box/unbox any object that fulfills the above requirements like so:

### Manual (Un)Boxing
```cpp
size_t cpp_side = 1001;

// C++ -> julia: boxing
jl_value_t* jl_side = box(cpp_side);

// julia -> C++: unboxing
size_t back_cpp_side = unbox<size_t>(jl_side);

// verify nothing was lost in the conversion
std::cout << back_cpp_side << std::endl;
```
```
1001
```

Any type fulfilling the above requirements is accepted by most `jluna` functions. Usually, these functions will implicitly (un)box their arguments and return types so, most of the time, we don't have to worry about manually calling `box`/`unbox<T>`. 

This also means that any 3rd party user only needs to defined `box` and `unbox<T>` for their class and it will work with all `jluna` functionality, making their C++ type julia-compatible. We will learn more about this in the [section on usertypes](#usertypes).

### List of (Un)Boxables

The following types are both boxable and unboxable out-of-the-box. 

```cpp
// cpp type              // julia-side type after boxing

jl_value_t*              -> Any
jl_module_t*             -> Module
jl_function_t*           -> Function
jl_sym_t*                -> Symbol

bool                     -> Bool
char                     -> Char
int8_t                   -> Int8
int16_t                  -> Int16
int32_t                  -> Int32
int64_t                  -> Int64
uint8_t                  -> UInt8
uint16_t                 -> UInt16
uint32_t                 -> UInt32
uint64_t                 -> UInt64
float                    -> Float32
double                   -> Float64

jluna::Any               -> Any
jluna::Proxy<State>      -> /* value-type deduced during runtime */
jluna::Symbol            -> Symbol
jluna::Type              -> Type
jluna::Array<T, R>       -> Array{T, R}     * °
jluna::Vector<T>         -> Vector{T}       *
jluna::JuliaException    -> Exception

std::string              -> String
std::complex<T>          -> Complex{T}      *
std::vector<T>           -> Vector{T}       *
std::array<T, R>         -> Vector{T}       *
std::pair<T, U>          -> Pair{T, U}      *
std::tuple<Ts...>        -> Tuple{Ts...}    *
std::map<T, U>           -> IdDict{T, U}    *
std::unordered_map<T, U> -> Dict{T, U}      *
std::set<T>              -> Set{T, U}       *

* where T, U are also (Un)Boxables
° where R is the rank of the array
```

## Accessing Variables

Let's say we have a variable `var` julia-side:

```cpp
State::script("var = 1234")
```

To access the value of this variable we can use the C-API. We receive a pointer to the memory `var` holds using `jl_eval_string`, then `unbox` that pointer:

```cpp
jl_value_t* var_ptr = jl_eval_string("return var");
auto as_int = unbox<int>(var_ptr);

std::cout << as_int << std::endl;
```
```
1234
```

`State::safe_return<T>` essentially does the same except it will forward any exception thrown during `return var` (such as `UndefVarError`):

```cpp
auto as_int = State::safe_return<int>("var");
std::cout << as_int << std::endl;
```
```
1234
```

While both ways get us the desired value, neither is a good way to actually manage the variable itself. How do we reassign it? Can we dereference the c-pointer? Who has ownership of the memory? All these questions are hard to manage using the C-API, however `jluna` offers a one-stop-shop solution for all of these problems: `jluna::Proxy`.

A proxy holds two things: the **memory address of its value** and a **symbol**. We'll get to the symbol later, for now, let's focus on the memory:<br>
Memory held by a proxy is safe from the julia garbage collector (GC) and assured to be valid. This means we don't have to worry about keeping a reference or pausing the GC when modifying the variable. Any memory, be it temporary or something explicitly referenced by a julia-side variable or `Base.Ref`, is guaranteed to be safe to access. 

We rarely create a proxy ourself, most of the time it will be generated for us by `State::(safe_)script`:

```cpp
State::script("var = 1234")
auto proxy = State::script("return var")
```
Use of `auto` simplifies the declaration and is encouraged whenever possible.<br>

Now that we have the proxy, we need to convert it to a value. Unlike the C-APIs `jl_value_t*` we do not need to call `box`/`unbox<T>`:

```cpp
// all following statements are exactly equivalent:

int as_int = proxy;   // recommended

auto as_int = proxy.operator int();

auto as_int = static_cast<int>(proxy);

auto as_int = (int) proxy;

auto as_int = unbox<int>(proxy) // discouraged
```

Where the first version is encouraged for style reasons. `jluna` handles implicit conversion behind the scenes, this makes it so we don't have to worry what the actual type of the julia-value is. `jluna` will try it's hardest to make our declaration work:

```cpp
State::script("var = 1234")
auto proxy = State::script("return var")

size_t as_size_t = proxy;
std::string as_string = proxy;
std::complex<double> as_complex = proxy;

std::cout << "size_t : " << as_size_t << std::endl;
std::cout << "string : " << as_string << std::endl;
std::cout << "complex: " << as_complex.real() << " | " << as_complex.imag() << std::endl;
```
```
1234
"1234"
1234 | 0
```

Of course, if the type of the julia variable cannot be converted to the target type, an exception is thrown:

```cpp
std::vector<double> as_vec = proxy;
```
```
terminate called after throwing an instance of 'jluna::JuliaException'
  what():  MethodError: Cannot `convert` an object of type Int64 to an object of type Vector
  (...)
```

This is already much more convenient than manually unboxing c-pointers, however the true usefulness of proxies lies in their ability to *mutate* julia-side values.

## Mutating Variables

As stated before, a proxy holds exactly one pointer to julia-side memory and exactly one symbol. There are two types of symbols:

+ a symbol starting with the character `#` is called an *internal id*
+ any other symbol is called a *name*

The behavior of proxies changes, depending on wether their symbol is a name or not. A proxies whos symbol is a name is called a **named proxy**, a proxy whos symbol is an internal id is called an **unnamed proxy**. 

To generate an unnamed proxy, we use `State::(safe_)script`.<br> To generate a named proxy we use `Proxy::operator[]`.

```cpp
State::safe_script("var = 1234");

auto unnamed_proxy = State::safe_script("return var");
auto named_proxy = Main["var"];
```

where `Main`, `Base`, `Core` are global, static, pre-initialized proxies holding the corresponding julia-side modules.

We can check a proxies name using `.get_name()`:

```cpp
std::cout << unnamed_proxy.get_name() << std::endl;
std::cout << named_proxy.get_name() << std::endl;
```
```
<unnamed proxy #9>
Main.var
```
We see that `unnamed_proxy`s symbol is `#9`, while `named_proxy`s symbol is `Main.var`, the same name as the julia-side variable `var` that we used to create it.

### Named Proxies

If and only if a proxy is named, assigning it will change the corresponding variable julia-side:

```cpp
State::safe_script("var = 1234")
auto named_proxy = Main["var"];

std::cout << "// before:" << std::endl;
std::cout << "cpp   : " << named_proxy.operator int() << std::endl;
State::safe_script("println(\"julia : \", Main.var)");

named_proxy = 5678; // assign

std::cout << "// after:" << std::endl;
std::cout << "cpp   : " << named_proxy.operator int() << std::endl;
State::safe_script("println(\"julia : \", Main.var)");
```
```
// before:
cpp   : 1234
julia : 1234
// after:
cpp   : 5678
julia : 5678
```

We see that after assignment, both the value `named_proxy` is pointing to, and the variable of the same name "`Main.var`" were affected by the assignment. This is somewhat atypical for julia but familiar to C++-users. A named proxy acts like a reference to the julia-side variable. <br><br> Because of this behavior, it lets us do things like:

```cpp
State::safe_script("vector_var = [1, 2, 3, 4]")
Main["vector_var"][0] = 999;   // indices are 0-based in C++
Base["println"]("julia prints: ", Main["vector_var"]);
```
```
julia prints: [999, 2, 3, 4]
```

Which is highly convenient. 

### Unnamed Proxy

Mutating an unnamed proxy will only mutate its value, **not** the value of any julia-side variable:
```cpp
State::safe_script("var = 1234")
auto unnamed_proxy = State::safe_script("return var");

std::cout << "// before:" << std::endl;
std::cout << "cpp   : " << named_proxy.operator int() << std::endl;
State::safe_script("println(\"julia : \", Main.var)");

unnamed_proxy = 5678; // assign

std::cout << "// after:" << std::endl;
std::cout << "cpp   : " << named_proxy.operator int() << std::endl;
State::safe_script("println(\"julia : \", Main.var)");
```
```
// before:
cpp   : 1234
julia : 1234
// after:
cpp   : 5678
julia : 1234
```

We see that `unnamed_proxy` was assigned a new memory address pointing to the new value `5678`. Meanwhile, `Main.var` is completely unaffected by this change. This makes sense, if we check `unnamed_proxy`s symbol again:

```cpp
std::cout << unnamed_proxy.get_name() << std::endl;
```
```
<unnamed proxy #9>
```

We see that is does not have a name, just an internal id. Therefore, it has no way to know where its value came from and thus has no way to mutate anything but the C++ variable.

When we called `return var`, julia did not return the variable itself but the value of said variable. An unnamed proxy thus behaves like a deepcopy of the value, **not** like a reference.

#### In Summary

+ We create a **named proxy** using `Main["var"]`.
  - Assigning a named proxy mutates its value and mutates the corresponding julia-side variable of the same name
+ We create an **unnamed proxy** using `State::(safe_)script("return var")`
  - Assigning an unnamed proxy mutates its value but does not mutate any julia-side variable

This is important to realize and is the basis of much of `jluna`s syntax and functionality.

### Detached Proxies

Consider the following code:

```cpp
State::safe_script("var = 1234");
auto named_proxy = Main["var"];

State::safe_script(R"(var = ["abc", "def"])");

std::cout << named_proxy.operator std::string() << std::endl;
```

What will this print? We know `named_proxy` is a named proxy so it should correspond to the variable `var`, however we reassigned `var` to a completely different value using only julia. `named_proxy` was never made aware of this, so it still currently points to its old value:

```
std::cout << named_proxy.operator std::string() << std::endl;
```
```
1234
```

While still retaining its name:

```
std::cout << named_proxy.get_name() << std::endl;
```
```
Main.var
```

This proxy is in what's called a *detached state*. Even though it is a named proxy, its current value does not correspond to the value of it's julia-side variable. This may have unforeseen consequences:

```cpp
State::safe_script("var = 1234");
auto named_proxy = Main["var"];
State::safe_script(R"(var = ["abc", "def"])");

std::cout << named_proxy[1].operator std::string() << std::endl;
```
```
terminate called after throwing an instance of 'jluna::JuliaException'
  what():  BoundsError
Stacktrace:
 [1] getindex(x::Int64, i::UInt64)
   @ Base ./number.jl:98
 [2] safe_call(::Function, ::Int64, ::UInt64)
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/julia/exception_handler.jl:80

signal (6): Aborted
```

Even though `var` is a vector julia-side, accessing the second index of `named_proxy` throws a BoundsError because `name_proxy`s value is still `Int64(1234)`, which does not have a second index.

Assigning a detached proxy will still mutate the corresponding variable:

```cpp
State::safe_script("var = 1234");
auto named_proxy = Main["var"];
State::safe_script(R"(var = ["abc", "def"])"); // assign julia-side

State::safe_script("println(\"before:\", var)");

named_proxy = 5678; // assign cpp-side

State::safe_script("println(\"after :\"var)");
```
```
before: ["abc", "def"]
after : 5678
```

While this behavior is internally consistent, it may cause unintentional bugs when reassigning the same variable frequently in both C++ and julia. To alleviate this, `jluna` offers a member function `Proxy::update()`, which evaluates the proxies name and replaces its value with value of the correspondingly named variable:

```cpp
State::safe_script("var = 1234");
auto named_proxy = Main["var"];
State::safe_script(R"(var = ["abc", "def"])");

named_proxy.update();

std::cout << named_proxy.operator std::string() << std::endl;
```
```
["abc", "def"]
```
While not necessary to do everytime an assignment happens, it is a convenient way to fix a detached proxy.

### Making a Named Proxy Unnamed

Sometimes it is desirable to stop a proxy from mutating the corresponding variable, even though it is a named proxy. While we cannot change a proxies name, we can generate a new unnamed proxy using the member function `Proxy::value()`. This functions returns a "fresh" unnamed proxy pointing to a deepcopy of the value of the original proxy. 

```cpp
State::script("var = 1234");
auto named_proxy = Main["var"];

auto value = named_proxy.value();   // create nameless deepcopy

// assigning either does not affect the other
value = 9999;
named_proxy = 4567;

std::cout << "named: " << named_proxy.operator int() << std::end,
std::cout << "value: " << named_proxy.operator int() << std::end,
```
```
named: 4567
value: 9999
```

Therefore, to make a named proxy unnamed, we can simply assign its `value` to itself:

```cpp
auto proxy = /*...*/
proxy = proxy.value()
```

Calling `.value()` on a proxy that is already unnamed simply creates a deepcopy with a new internal id.

### Accessing Fields

For a proxy whos value is a `structtype`, we can access any field using `operator[]`:

```cpp
State::safe_script(R"(
    mutable struct StructType
        _field
    end
    
    instance = StructType(1234)
)");

auto instance = Main["instance"];

std::cout << instance["_field"].operator int() << std::endl;
```
```
1234
```

As before `operator[]` returns a named proxy and assigning a named proxy also assigns the corresponding variable. This means we can assign fields just like we assigned variables in module-scope:

```cpp
State::safe_script(R"(
    mutable struct StructType
        _field
    end
    
    instance = StructType(1234)
)");

auto instance = Main["instance"];
auto instance_field = instance["_field"];

instance_field = 5678;

State::script("println(instance)");
```
```
StructType(5678)
```

Of course, we could also do the above inline:

```cpp
Main["instance"]["_field"] = 9999;
State::script("println(instance)");
```
```
StructType(5678)
```

Which is, again, highly convenient.

## Functions

### Calling julia Functions from C++
#### Accessing julia Functions

Proxies can hold any julia-side value. This includes functions:

```cpp
State::safe_script("f(x) = sqrt(x^x^x)");

// with operator[]
auto named_f = Main["f"];

// or via return
auto unnamed_f = State::safe_script("return f");
```



#### Calling julia Functions

If the proxy is a function, it will be callable via `operator()`, `.call` and `.safe_call`, where `operator()` simply forwards the arguments to `.safe_call`:

```cpp
auto println = Main["println"];

// with exception forwarding
println(/* ... */);
println.safe_call(/* ... */);

// without exception forwarding
println.call(/* ... */);

// inline:
Base["println"](Base["typeof"](std::set<std::map<size_t, std::pair<size_t, std::vector<int>>>>({{{1, {1, {1, 1, 1}}}}})));
```
```
Set{IdDict{UInt64, Pair{UInt64, Vector{Int32}}}}
```

As mentioned before, any boxable type including proxies themself can be used as arguments directly, without manually having to call `box` or `unbox<T>`.

### Calling C++ Functions from julia
The previous section dealt with calling julia-side functions from C++. Now we will learn how to call C++-side functions from julia.<br>
We first need *register* a function. Only lambdas can be registered:

#### Registering Functions
```cpp
// always specify trailing return type manually
register_function("print_vector", [](jl_value_t* in) -> jl_value_t* {
    
    // convert jl_value_t* to jluna objects via box/unbox
    auto as_vector = unbox<Vector<size_t>>(in);
    std::cout << "cpp prints:" << std::endl;
    for (auto e : as_vector)
    {
        std::cout << (size_t) e << std::endl;
        e = (int e) + 10;
    }

    // return as jluna object or boxed, both will be forwarded to julia
    return as_vector;
});
```

Note the explicit trailing return type `-> jl_value_t*`. It is recommended to always specify it when using `register_function`.<br>

#### Allowed Function Names

While arbitrary julia-valid characters (except `.`) in the functions name are allowed, it is recommended to stick to C++ style convention when naming functions. However, do feel free to specifically use postfix `!` for mutating functions, as it is customary in julia. 

```julia
# good:
"print_vector", "add", "(=>)", "modify!"

# discouraged but allowed:
"h!e!l!p!", "get_∅", "écoltäpfel", "(☻)"

# illegal:
"anything.withadot", "(..)", "0012", "help?"
```

See the [julia manual entry on variable names](https://docs.julialang.org/en/v1/manual/variables/#man-allowed-variable-names) for more information about strictly illegal names.

#### Calling Functions

After having registered a function, we then call the function (from within julia) using `cppcall`. Instead of a string we're using a `Symbol` that has the exact same contents as the string name used C++-side. `cppcall` has the following signature:

```julia
cppcall(::Symbol, xs...) -> Any
```

Unlike julias `ccall` we do not supply anything but the functions name and the arguments to be forwarded. The return type and type of the arguments is deduced automatically and we can use any julia type as arguments, not just C-friendly ones.<br> <br>
Calling our `print_vector` which first prints the entire vector, then adds `10` to each element in julia would look like this:

```julia
# in julia
result = cppcall(:print_vector, [1, 2, 3, 4])
println("julia prints: ", result)
```
```
cpp prints:
1 
2 
3 
4

julia prints: [11, 12, 13, 14]
```

We see that we can use the result of the function directly, as if it was a regular julia-side function.

#### Possible Signatures

Only the following signatures for lambdas to be bound via `register_function` are allowed (this is enforced at compile time):

```cpp
() -> void
(jl_value_t*) -> void
(std::vector<jl_value_t*>) -> void
(jl_value_t*, jl_value_t*) -> void
(jl_value_t*, jl_value_t*, jl_value_t*) -> void
(jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*) -> void
(jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*) -> void

() -> jl_value_t*
(jl_value_t*) -> jl_value_t*
(std::vector<jl_value_t*>) -> jl_value_t*
(jl_value_t*, jl_value_t*) -> jl_value_t*
(jl_value_t*, jl_value_t*, jl_value_t*) -> jl_value_t*
(jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*) -> jl_value_t*
(jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*) -> jl_value_t*
```

Templated lambdas are not allowed. 

#### Using Non-julia Objects in Functions

While this may seem limiting at first, it is not. For example we can forward any C++ objects as as arguments not through the lambdas actual arguments but it's capture:

```cpp
// a C++-object, uncompatible with julia
struct Object
{
    void operator()() // not const
    {
        _field = 456;
        std::cout << "object called " << _field << std::endl;
    }

    size_t _field = 123;
};

Object instance;

// wrap instance in mutable std::ref and hand it to lambda via capture
register_function("call_object", [instance_ref = std::ref(instance)]() -> void 
{
    instance_ref.operator()();
});

State::safe_script("cppcall(:call_object)");
```
```
object called 456
```

This makes it possible to call C++-only members function of objects by simply wrapping them first.

#### Calling Non-Lambda Functions

While only lambdas of the mentioned signatures can be bound via `register_function`, we can still call an arbitrary function with arbitrary arguments (see above) by similarly wrapping it in one or more lambdas:

```cpp
// a templated c++ function returning auto ...
template<typename T>
decltype(auto) fancy_non_lambda_func(const T& t)
{
    do_something_with(t);   
    return std::forward<T>(t);
}

// ... can be passed by wrapping it like so:
register_function("fancy_f_int32", [](jl_value_t* jl_int) -> jl_value_t* {
    
    // convert from julia-value to cpp int
    int as_int = unbox<int>(jl_int); 
    
    // call cpp function
    auto res = fancy_non_lambda_func<int>(as_int);
    
    // then convert from cpp-value to julia-value and return
    return box(res);
});

register_function("fancy_f_string", [](jl_value_t* jl_string) -> jl_value_t* {
    
    // same for strings
    auto res = fancy_non_lambda_func<std::string>(unbox<std::string>(jl_string));
    return box(res);
});
```
Through tricks like this, any function and any object can be called from julia, however if we want to directly transfer memory between the two states, we will need to call `box`/`unbox<T>`.

## Arrays

julia-side objects of type `T <: AbstractArray` are represented C++-side by their own proxy type: `jluna::Array<T, R>`. Just like in julia, `T` is the value_type and `R` the rank or dimensionality of the array. <br>

julia array types and `jluna` array types correspond to each other like so:

| julia          | jluna         |
|----------------|---------------|
| `Vector{T}`    | `Vector<T>`   |
| `Matrix{T}`    | `Array<T, 2>` |
| `Array{T, R}`  | `Array<T, R>` | 

Where `jluna::Vector` inherits from `jluna::Array` and thus provides all of the same functionality while offering some vector-only methods in addition.

### CTORs

We can create an array proxy like so:

```cpp
// make 3d array with values 1:27
State::safe_script("array = Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))");

// unnamed array proxy
jluna::Array<Int64, 3> by_value = State::script("return array");
auto by_value = State::script("return array").as<Array<Int64, 3>>();

// named array proxy
jluna::Array<Int64, 3> array = Main["array"];
auto array = Main["array"].as<Array<Int64, 3>>();
```
Where, just as before, only named proxies will mutate julia-side variables.

We can use the generic value type `Any` to make it possible for the array proxy to attach any julia-side array, regardless of value type. `jluna` provides 3 convenient typedefs for this:

```
using Array1d = Array<Any, 1>;
using Array2d = Array<Any, 2>;
using Array3d = Array<Any, 3>;
```

This is useful when the value type of the array is not know at the point of proxy declaration.


### Indexing

There are two ways to index a multidimensional array:

+ *linear* indexing treats the array as 1-dimensional and returns the n-th value in column-major order
+ *multidimensional* indexing requires one index per dimension

While both of these styles of indexing are available in julia, to keep with C-convention, indices in `jluna` are 0-based instead of 1-based.

```cpp
jluna::Array<Int64, 3> array = Main["array"];
Main["println"]("before ", array);

// 0-based linear indexing
array[12] = 9999;

// 0-based multi-dimensional indexing
array.at(0, 1, 2) = 9999;

Main["println"]("after ", array);
```
```
before [1 4 7; 2 5 8; 3 6 9;;; 10 13 16; 11 14 17; 12 15 18;;; 19 22 25; 20 23 26; 21 24 27]
after [1 4 7; 2 5 8; 3 6 9;;; 10 9999 16; 11 14 17; 12 15 18;;; 19 9999 25; 20 23 26; 21 24 27]
```

While `jluna` cannot offer list comprehension, `jluna::Array` does allow for julia-style indexing using a collection:

```cpp
auto sub_array = array.at({2, 13, 1}); // any iterable collection can be used
Base["println"](sub_array)
```
```
[3, 14, 2]
```

To closer illustrate the relationship between indexing in `jluna` and indexing in julia, consider the following table (where `M` is a N-dimensional array)

| N | julia |jluna |
|------|-------|--------------------------|
| 1    | `M[1]`| `M.at(0)` or `M[0]`|
| 2    | `M[1, 2]`  | `M.at(0, 1)`|
| 3    | `M[1, 2, 3]`  | `M.at(0, 1)`|
|      |       | |
| Any  | `M[ [1, 13, 7] ]`| `M[ {0, 12, 6} ]` |

### Iterating

In `jluna`, arrays of any dimensionality are iterable in column-major order (just as in julia):

```cpp
Array<size_t, 2> array = State::script("return [1:2 3:4 5:6]");
Base["println"](array);

// iterate using value type
for (size_t i : array)
  std::cout << i << std::endl;
```
```
[1 3 5; 2 4 6]
1
2
3
4
5
6
```

We can create an assignable iterator by doing the following (note the use of `auto` instead of `auto&`)

```cpp
Array<size_t, 2> array = State::script("return [1:2 3:4 5:6]");
Base["println"]("before: ", array);

for (auto it : array)
    it = static_cast<size_t>(it) + 1;

Base["println"]("after : ", array);
```
```
before: [1 3 5; 2 4 6]
after : [2 4 6; 3 5 7]
```

Here, `auto` is deduced to a special iterator type that basically acts like a regular `jluna::Proxy` (for example, we need to manually cast it to `size_t` in the above example) but with faster, no-overhead read/write-access to the array data.

### Vectors

Vector are just arrays, however similarly to `Vector{T}` in julia, their 1-dimensionality gives them access to additional functions:

```cpp
State::safe_script("vector = collect(1:10)");
jluna::Vector<size_t> = Main["vector"];

vector.push_front(9999);
vector.push_back(9999);
vector.insert(12, 0);
vector.erase(12);

for (auto e : vector)
    e = static_cast<size_t>(e) + 1;

Base["println"](vector);
```
```
[10000, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 10000]
```

Note that `Array<T, R>::operator[](Range_t&&)` (linear indexing with a range) always returns a vector of the corresponding value type, regardless of the original arrays dimensionality.

## Matrices
(this feature is not yet implemented, simply use `Array<T, 2>` until then)

## Expressions

(this feature is not yet implemented)

## Usertypes

(this feature is not yet implemented)

## C-API

This section will focus on using the C-API in addition to jluna and from within C++

```cpp
#include <julia.h>
```

### Meaning of C-Types

+ `jl_value_t*`: address julia-side value of arbitrary type
+ `jl_function_t*`: address of julia-side function
+ `jl_sym_t*`: address of julia-side symbol
+ `jl_module_t*`: address of julia-side singleton module
    - `jl_main_module`: Main
    - `jl_base_module`: Base
    - `jl_core_module`: Core
+ `jl_datatype_t*`: address of julia-side singleton type
    - `jl_any_type`: Any
    - `jl_type_type`: Type
    - `jl_module_type`: Module
    - `jl_integer_type`: Integer
    - etc.
    
### Executing Code

```cpp
jl_eval_string("your code goes here")
jl_eval_string(R"(
    your 
    multi-line
    code
    goes
    here
)");
```

### Forwarding Exceptions in C
```cpp
jl_eval_string("sqrt(-1)"); 
// nothing happens

jluna::forward_last_exception();
// exception thrown
```
```
terminate called after throwing an instance of 'jluna::juliaException'
  what():  DomainError(-1.0, "sqrt will only return a complex result if called with a complex argument. Try sqrt(Complex(x)).")
Stacktrace: <no stacktrace available>
```

### Accessing Values in C

```cpp
// by address:
jl_value_t* address = jl_eval_string("return value_name");

// by value
T value = unbox<T>(jl_eval_string("return value_name"));

// there is no memory-safe, GC-safe way to modify variables from within C only
```
### Functions in C

#### Accessing Functions in C

```cpp
jl_function_t* function = (jl_function_t*) jl_eval_string("return function_name");
// or
jl_function_t* function = jl_get_function((jl_module_t*) jl_eval_string("return module_name"), "function_name"))
```

#### Calling Functions in C

```cpp
std::vector<T> cpp_arguments = /* ... */ // T is any boxable type

auto* function = jl_get_function(jl_base_module, "println");
std::vector<jl_value_t*> args;

for (auto a : cpp_arguments)
    args.push_back(box(a));
    
jl_value_t* res = jl_call(function, args, args.size());
```

### Arrays in C
#### Accessing & Indexing Arrays in C

```cpp
jl_array_t* array = (jl_array_t*) jl_eval_string("return [[1, 2, 3]; [2, 3, 4]; [3, 4, 5]]");

for (size_t i = 0; i < jl_array_len(array); ++i)    // 0-based
    std::cout << unbox<jluna::Int64>(jl_arrayref(array, i));    

// only linear indexing is available through the C-API
```
```
1 2 3 2 3 4 3 4 5
```

#### Mutating Arrays in C
```cpp
jl_array_t* array = (jl_array_t*) jl_eval_string("return [1, 2, 3, 4]");

jl_arrayset(array, box<Int64>(999), 0); 
// box<T> with exact equivalent of T julia
// jluna does fuzzy conversion behind the scenes, C doesn't

jl_function_t* println = jl_get_function(jl_base_module, "println");
jl_call1(println, (jl_value_t*) array);
```

### Strings in C
#### Accessing Strings in C
```cpp
jl_value_t* jl_string = jl_eval_string("return \"abcdef\"");
std::string std_string = std::string(jl_string_data(jl_string));
std::cout << std_string << std::endl;
```
```
abcdef
```
To modify strings, we need to cast them `jl_array_t` and modify them like arrays

### Initialization & Shutdown in C

```cpp
#include <julia.h>

int main() 
{
    jl_init();
    // or
    jl_init_with_image("path/to/your/image", NULL);
    
    // all your application
    
    jl_atexit_hook(0);
}
```






