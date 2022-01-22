# jluna: Quick & Dirty

This page will give a fly-by, abbriviated overview of all of `jluna`s relevant features and syntax. While it is useful for referencing back to, it should not be considered proper documentation and it is thus recommended that you read the tutorial on [~~jluna::Proxy~~]() at some point.

Please navigate to the appropriate section by clicking the links below:

## Table Of Contents

1. [Initialization & Shutdown](#initialization)<br>
2. [Executing Code](#executing-code)<br>
3. [Controlling the Garbage Collector](#garbage-collector-gc)<br>
  3.1 [Enabling/Disabling GC](#enablingdisabling-gc)<br>
  3.2 [Manual Collection](#manually-triggering-gc)<br>
4. [Boxing / Unboxing](#boxing--unboxing)<br>
  4.1 [Manual](#manual)<br>
  4.2 [(Un)Boxable as Concepts](#concepts)<br>
  4.3 [List of (Un)Boxables](#list-of-unboxables)<br>
5. [Accessing Variable](#accessing-variables)<br>
  5.1 [Mutating Variables](#mutating-variables)<br>
  5.2 [Accessing Fields](#accessing-fields)<br>
6. [Functions](#functions)<br>
   6.1 [Accessing Julia Functions from C++](#accessing-julia-functions)<br>
   6.2 [Calling Julia Functions from C++](#calling-julia-functions)<br>
   6.3 [Accessing C++ Functions from Julia](#registering-functions)<br>
   6.4 [Calling C++ Functions from Julia](#calling-c-functions-from-julia)<br>
   6.5 [Allowed Function Signatures](#possible-signatures)<br>
   6.6 [Using arbitrary Objects in Julia Functions](#using-non-julia-objects-in-functions)<br>
7. [Arrays](#arrays)<br>
  7.1 [Multi-Dimensional Arrays](#multi-dimensional-arrays)<br>
  7.2 [Vectors](#vectors)<br>
8. [~~Expressions~~]()<br>
9. [~~Usertypes~~]()<br>
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

Before any interaction with `jluna` or Julia can take place, the julia state needs to be initialized:

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

When a program exists regularly, all julia-side values allocated through `jluna` are safely deallocated, however we can force this behavior early by calling `State::shutdown`

## Executing Code

`jluna` has two ways of executing code (represented C++ side as string): *with* exception forwarding and *without* exception forwarding. Code being called without exception forwarding will not report any failures except hard errors such as trying to dereference invalid memory, which will crash the entire program.

```cpp
// without exception forwarding
State::safe_script("your inline code");
State::safe_script(R"(
    your
    multi-line
    code
)");
jl_eval_string("your inline code");

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

The Julia-side garbage collector operator completely independently just like it would in a pure Julia program, however sometimes it is necessary to disable or control it manually. To do this, we can use the following `jluna::State` member functions:

#### Enabling/Disabling GC
```cpp
bool is_enabled = State::is_garbage_collector_enabled();
State::set_garbage_collector_enabled(not is_enabled);
```

#### Manually Triggering GC
```cpp
State::collect_garbage();
// always works, if currently disabled, enables, then collects, then disables again
```

## Boxing / Unboxing

The act of taking a C++-side value, converting it and then allocating it Julia-side is called *boxing*. The reverse process is similarly called *unboxing*. In `jluna`, these two functions always have the following signatures:

```cpp
template<typename T>
jl_value_t* box(T);

template<typename T>
T unbox(jl_value_t*);
```

The property of being (un)boxable is represented in C++ as two concepts:

### Concepts
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

### Manuall (Un)Boxing
```cpp
// C++ -> Julia: boxing
size_t cpp_side = 1001;
jl_value_t* jl_side = box(cpp_side);

// Julia -> C++: unboxing
size_t back_cpp_side = unbox<size_t>(jl_side);

// verify nothing was lost in the conversion
std::cout << back_cpp_side << std::endl;
```
```
1001
```

Though, because `jluna` functions will implicitly box/unbox their arguments, doing so manually is often (but not always) unnecessary.

### List of (Un)Boxables

The following types are both boxable and unboxable out-of-the-box. After Julia-side allocation, the objects type is noted after the `->`:

```cpp
jl_value_t*              -> /* type deduced during runtime */
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

jluna::Proxy<State>      -> /* type deduced during runtime */
jluna::Symbol            -> Symbol
jluna::Type              -> Type
jluna::Array<T, R>       -> Array{T, R}     *
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

* where T, U are (Un)Boxables
```
## Accessing Variables

Julia-side variables and values are stored C++-side by `jluna::Proxy`. This calls holds the address of the Julia-side memory and optionally the symbol of the variables name. While at least one proxy referencing a particular block of memory Julia-side is in scope C++-side, the Julia-side memory is safe from being garbage collected, this is also true of Julia-side temporaries and anonymous functions. Multiple proxies can hold ownership over the same memory, and the memory is only released once the last proxy pointing to it calls its destructor.

### Mutating Variables

To get access to a value allocated Julia-side, we can do so in two distinct ways:

#### By Value

To access a variable *by value*, use `script("return variable")`

```cpp
State::safe_script("variable = 123");

auto by_value = State::safe_script("return variable");

std::cout << "before assignment:" << std::endl;
State::safe_script("println(\"julia: \", variable)");
std::cout << "cpp  : " << (Int64) by_value << std::endl;

by_value = 456;

std::cout << "after assignment:" << std::endl;
State::safe_script("println(\"julia: \", variable)");
std::cout << "cpp  : " << (Int64) by_value << std::endl;
```
```
before assignment:
julia: 123
cpp  : 123

after assignment:
julia: 123
cpp  : 456
```

Mutating the C++-side proxy reassigns it a new value but **does not** mutate the original Julia-side variable. The proxy holds no information about where it value came from.

#### By Name

To access a variable *by name*, use `operator["variable"]`:

```cpp
auto by_name = Main["variable"];

std::cout << "before assignment:" << std::endl;
State::safe_script("println(\"julia: \", variable)");
std::cout << "cpp  : " << (Int64) by_name << std::endl;

by_name = 789;

std::cout << "after assignment:" << std::endl;
State::safe_script("println(\"julia: \", variable)");
std::cout << "cpp  : " << (Int64) by_name << std::endl;
```
```
before assignment:
julia: 123
cpp  : 123

after assignment:
julia: 789
cpp  : 789
```

**Mutating a named proxy also mutates the corresponding Julia-side variable of the same name**. We can verify whether a proxy will exhibit this behavior using `is_mutating()`

#### Named and Unnamed Proxies

We can check a proxies name using `.get_name()`

```cpp
std::cout << by_value.get_name() << std::endl;
std::cout << by_name.get_name() << std::endl;
```
```
                // empty string
Main.variable
```
A proxy whos name is an empty string (or `Symbol("")` Julia-side) is called an *unnamed proxy*. A proxy is mutating if and only if it has a name. A proxy does not mutate if and only if it does not have a name. <br>
While we cannot delete a proxies name, we can create a new proxy using only the named proxies value. `jluna` offers a convenient member function `Proxy::value() const` for this:

```cpp
State::script("variable = 123");

auto named = Main["variable"];
std::cout << named.get_name();

auto as_value = named.value();
std::cout << as_value.get_name() << std::endl;
```
```
Main.variable
              // empty string
```

Note that these proxies have no relation to each other and ownership over the memory is shared, just as with any other proxy creation method.

### Accessing Fields

Using `operator[](std::string)`, we can access both names in a module (including `Main`) and fields of a structtype:

```cpp
State::safe_script(R"(
    struct StructType
        _field
    end

    instance = StructType(123);
)");

auto field_proxy = Main["instance"]["_field"];
field_proxy = 321;

State::safe_script("println(Main.instance._field)");
```
```
321
```

Because the proxies were created by `operator[]`, they are named and mutate it's corresponding variable (the field of the specific instance). If the structtype is not mutable, a Julia-side exception will be thrown on assignment.

## Functions

### Calling Julia Functions from C++
#### Accessing Julia Functions

We can obtain a proxy pointing to a Julia-side function just as with any other Julia-side value:

```cpp
State::safe_script("f(x) = sqrt(x^x^x)");

auto f = Main["f"];

// or
auto f = State::safe_script("return f");
```

#### Calling Julia Functions

If the proxy is a function, it will be callable via `operator()` or `.call` and `.safe_call`, where `operator()` simply forwards the arguments to `.safe_call`:

```cpp
auto println = Main["println"];

// any boxable type can be used as arguments directly

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

### Calling C++ Functions from Julia
The previous section dealt with calling Julia-side functions in C++. This section deals is about the inverse, calling C++-Side functions in Julia.<br>
To do this, we first need *register* a function. Only lambdas can be registered. Furthermore, it is recommended to always manually specify the Lambda return type using the `->` trailing return type operator:

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
#### Calling Functions

We can then call the function (from within Julia) using `cppcall` which is exported from `jluna.cppcall` into the global scope and has the following signature:

```julia
cppcall(::Symbol, xs...) -> Any
```

We see it is a parametric function so unlike Julias `ccall`, we call using only the name of the C++ function and any arguments to be forwarded. Argument- and return-type are deduced automatically.<br>

Calling our above function which first prints the entire vector, then adds `10` to each element:

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

#### Possible Signatures

The following signatures for lambdas to be bound via `register_function` are allowed (this is enforced at compile time):

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

#### Using Non-Julia Objects in Functions

```cpp
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

#### Calling Non-Lambda Functions

While only lambdas of the mentioned signatures can be bound, we can call an arbitrary function with arbitrary arguments (see above) by wrapping it in one or more lambdas:

```cpp
template<typename T>
decltype(auto) fancy_non_lambda_func(T&& t)
{
    do_something_with(t);   
    return std::forward<T>(t);
}

// can be registered like so:
register_function("fancy_f_int32", [](jl_value_t* jl_int) -> jl_value_t* {
    
    auto res = fancy_non_lambda_func<int>(unbox<int>(jl_int));
    return box(res);
});

register_function("fancy_f_string", [](jl_value_t* jl_string) -> jl_value_t* {
    
    auto res = fancy_non_lambda_func<std::string>(unbox<std::string>(jl_string));
    return box(res);
});
```
## Arrays

Julia-side objects of type `T <: AbstractArray` are represented C++-side by their own proxy type: `jluna::Array<T, R>` where `T` is the value_type and `R` is the rank or dimensionality of the array. <br>

Julia array types and `jluna` array types correspond to each other like so:

| Julia          | jluna         |
|----------------|---------------|
| `Vector{T}`    | `Vector<T>`   |
| `Matrix{T}`    | `Array<T, 2>` |
| `Array{T, R}`  | `Array<T, R>` | 

Where `jluna::Vector` inherits from `jluna::Array` and is thus functionally equivalent, but furthermore includes all additional functionality inherent to Julia `Vector{T}`

### Multi-Dimensional Arrays

#### CTORs

We can create an array proxy like so:

```cpp
State::safe_script("array = Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))");

// unnamed:
jluna::Array<Int64, 3> by_value = State::script("return array");
auto by_value = State::script("return array").as<Array<Int64, 3>>();

// named:
jluna::Array<Int64, 3> array = Main["array"];
auto array = Main["array"].as<Array<Int64, 3>>();
```
Where, just as before, only named proxies will mutate julia-side variables.

#### Indexing

There are two way to index a multi-dimensional array:

+ *linear* indexing treats the array as 1-dimensional and returns the n-th value in column-major order (this is the same as in Julia)
+ *multi-dimensional* indexing requires one index per dimension

To keep with C-convention, indices in `jluna` are 0-based.

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

To closer illustrate the relationship between indexing in `jluna` and indexing in Julia, consider the following table (where `M` is an n-dimensional array)

| Rank | Julia |jluna |
|------|-------|--------------------------|
| 1    | `M[1]`| `M.at(0)` or `M[0]`|
| 2    | `M[1, 2]`  | `M.at(0, 1)`|
| 3    | `M[1, 2, 3]`  | `M.at(0, 1)`|


### Vectors

```cpp
State::safe_script("vector = collect(1:10)");
jluna::Vector<size_t> = Main["vector"];

vector.push_front(9999);
vector.push_back(9999);
vector.insert(12, 0);
vector.erase(12);

for (auto e : vector)
    e = static_cast<size_t>(e) + 1; // array iterator assignable

Base["println"](vector);
```
```
[10000, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 10000]
```

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
terminate called after throwing an instance of 'jluna::JuliaException'
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
// box<T> with exact equivalent of T Julia
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






