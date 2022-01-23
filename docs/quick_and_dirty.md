# jluna: Quick & Dirty

This page will give a fly-by, abbreviated overview of all of `jluna`s relevant features and syntax. While it is useful for referencing back to, it should not be considered a proper manual. It is recommended that you read the tutorial on [~~jluna::Proxy~~]() at some point.

Please navigate to the appropriate section by clicking the links below:

## Table Of Contents

1. [Initialization & Shutdown](#initialization)<br>
2. [Executing Code](#executing-code)<br>
3. [Controlling the Garbage Collector](#garbage-collector-gc)<br>
  3.1 [Enabling/Disabling GC](#enablingdisabling-gc)<br>
  3.2 [Manual Collection](#manually-triggering-gc)<br>
4. [Boxing / Unboxing](#boxing--unboxing)<br>
  4.1 [Manual](#manual-unboxing)<br>
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
   6.5 [Allowed Function Names](#allowed-function-names)<br>
   6.6 [Allowed Function Signatures](#possible-signatures)<br>
   6.7 [Using arbitrary Objects in Julia Functions](#using-non-julia-objects-in-functions)<br>
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

Before any interaction with `jluna` or Julia can take place, the Julia state needs to be initialized:

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

When a program exists regularly, all Julia-side values allocated through `jluna` are safely deallocated, however on an exit code other than `0`, it is recommended to manually call `State::shutdown()`.

## Executing Code

`jluna` has two ways of executing code (represented C++ side as string): *with* exception forwarding and *without* exception forwarding. Code called without exception forwarding will not only not report any errors but simply appear to "do nothing" unless it is a fatal error, in which case the entire application will crash. Because of this, it is highly recommended to always air on the side of safety by using the `safe_` overloads whenever possible:

```cpp
// without exception forwarding
State::script("your inline code");
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

The Julia-side garbage collector operates completely independently, just like it would in a pure Julia program. However, sometimes it is necessary to disable or control its behavior manually. To do this, `jluna::State` offers the following member functions:

#### Enabling/Disabling GC
```cpp
State::set_garbage_collector_enabled(false);
```

#### Manually Triggering GC
```cpp
State::collect_garbage();
// always works, if currently disabled, enables, then collects, then disables again
```

We can access whether the GC is currently enabled using `State::is_garbage_collector_enabled`.

## Boxing / Unboxing

Julia and C++ do not share any memory, often objects that have conceptually the same type have wildly different memory layouts. Because of this, we need to not only transfer memory from one language's state to the others but convert them to a memory layout understandable at the destination.

Converting, then sending a value from C++ to Julia is called *boxing*. Similarly, converting then sending a value from Julia to C++ is called *unboxing*. Any value needs to be (un)boxed, there are no exceptions. We can do so using the following functions:

```cpp
template<typename T>
jl_value_t* box(T);

template<typename T>
T unbox(jl_value_t*);
```

where `jl_value_t*` is an address of Julia-side memory.

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

### Manual (Un)Boxing
```cpp
size_t cpp_side = 1001;

// C++ -> Julia: boxing
jl_value_t* jl_side = box(cpp_side);

// Julia -> C++: unboxing
size_t back_cpp_side = unbox<size_t>(jl_side);

// verify nothing was lost in the conversion
std::cout << back_cpp_side << std::endl;
```
```
1001
```

Any type fulfilling the above requirements is accepted by most `jluna` functions. Usually these functions will implicitly (un)box their arguments and return types so most of the time, we don't have to worry about manually calling `box`/`unbox<T>`.

### List of (Un)Boxables

The following types are both boxable and unboxable out-of-the-box. 

```cpp
// cpp type              // Julia-side type after boxing

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

Instead of passing around unsafe pointers to Julia-side memory, it is instead managed C++-side by `jluna::Proxy`. Each proxy holds exactly one pointer and (optionally) a symbol, it's *name*. A proxy with a symbol is called a *named proxy*, a proxy who's symbol is `nullptr` is called an *unnamed proxy*.<br>

The main advantage of proxies is that the memory they point to is safe from the Julia garbage collector, as long as a proxy that needs the memory to stay valid is in scope C++-side, the memory is safe.
Multiple proxies can take ownership of the same Julia-side value, its memory will only be freed once there are exactly 0 proxies pointing to it.

### Mutating Variables

We can access Julia-side variables as proxies in two distinct ways:

#### By Value

To access a variable *by value*, use `script("return variable")`

```cpp
State::safe_script("variable = 123"); // access

auto by_value = State::safe_script("return variable");

std::cout << "before assignment:" << std::endl;
State::safe_script("println(\"Julia: \", variable)");
std::cout << "cpp  : " << (Int64) by_value << std::endl;

by_value = 456; // assign proxy

std::cout << "after assignment:" << std::endl;
State::safe_script("println(\"Julia: \", variable)");
std::cout << "cpp  : " << (Int64) by_value << std::endl;
```
```
before assignment:
Julia: 123
cpp  : 123

after assignment:
Julia: 123
cpp  : 456
```

While mutating a C++-side proxy assigns it a new value, it **does not** mutate the original Julia-side variable. The proxy holds no information about where its value came from because it is unnamed. 

#### By Name

To access a variable *by name*, use `operator["variable"]`:

```cpp
auto by_name = Main["variable"]; // access

std::cout << "before assignment:" << std::endl;
State::safe_script("println(\"Julia: \", variable)");
std::cout << "cpp  : " << (Int64) by_name << std::endl;

by_name = 789;  // assign

std::cout << "after assignment:" << std::endl;
State::safe_script("println(\"Julia: \", variable)");
std::cout << "cpp  : " << (Int64) by_name << std::endl;
```
```
before assignment:
Julia: 123
cpp  : 123

after assignment:
Julia: 789
cpp  : 789
```

Mutating a **named** proxy also mutates the corresponding Julia-side variable (of the same name). We can verify whether a proxy has this behavior using the member function `is_mutating()`.

#### Named and Unnamed Proxies

To reiterate because it is important to be aware of this at all times:

+ `State::script` returns an *unnamed* proxy that does not mutate
+ `operator[] returns a *named* proxy 
that does mutate

We can check a proxies name using `.get_name()`

```cpp
std::cout << by_value.get_name() << std::endl;
std::cout << by_name.get_name() << std::endl;
```
```
                // empty string
Main.variable
```
An unnamed proxies name will be an empty string (or `Symbol("")`)) Julia-side.<br><br>
While we cannot delete a proxies name, it is possible to create a new proxy using only the named proxies value. `jluna` offers a convenient member function for this: `Proxy::value() const`

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

Note that both of these proxies point to the exact same memory Julia-side, that is, Julias operator `(===)` would return true.

### Accessing Fields

Using `operator[](std::string)`, we can access both names in a module (including `Main`, like above) and fields of a structtype:

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
<br>
`jluna`s `operator[]` is designed to be equivalent to Julias dot-operator, however we cannot mix the two:

```cpp
Main["instance._field"];
```
```
terminate called after throwing an instance of 'jluna::JuliaException'
  what():  UndefVarError: instance._field not defined
Stacktrace:
  (...)
```
It is good practice to never use the character `.` anywhere in the string that is `operator[]`s argument.

## Functions

### Calling Julia Functions from C++
#### Accessing Julia Functions

Proxies can point to any value and this includes functions.
We can obtain such a proxy using:

```cpp
State::safe_script("f(x) = sqrt(x^x^x)");

// with operator[]
auto named_f = Main["f"];

// or via return
auto unnamed_f = State::safe_script("return f");
```



#### Calling Julia Functions

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

### Calling C++ Functions from Julia
The previous section dealt with calling Julia-side functions from C++. Now we will learn how to call C++-side functions from Julia.<br>
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

    // return as jluna object or boxed, both will be forwarded to Julia
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

After having registered a function, we then call the function (from within Julia) using `cppcall`. Instead of a string we're using a `Symbol` that has the exact same contents as the string name used C++-side. `cppcall` has the following signature:

```julia
cppcall(::Symbol, xs...) -> Any
```

Unlike Julias `ccall` we do not supply anything but the functions name and the arguments to be forwarded. The return type and type of the arguments is deduced automatically and we can use any Julia type as arguments, not just C-friendly ones.<br> <br>
Calling our `print_vector` which first prints the entire vector, then adds `10` to each element in Julia would look like this:

```julia
# in julia
result = cppcall(:print_vector, [1, 2, 3, 4])
println("Julia prints: ", result)
```
```
cpp prints:
1 
2 
3 
4

Julia prints: [11, 12, 13, 14]
```

We see that we can use the result of the function directly, as if it was a regular Julia-side function.

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

#### Using Non-Julia Objects in Functions

While this may seem limiting at first, it is not. For example we can forward any C++ objects as as arguments not through the lambdas actual arguments but it's capture:

```cpp
// a C++-object, uncompatible with Julia
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
    
    // convert from Julia-value to cpp int
    int as_int = unbox<int>(jl_int); 
    
    // call cpp function
    auto res = fancy_non_lambda_func<int>(as_int);
    
    // then convert from cpp-value to Julia-value and return
    return box(res);
});

register_function("fancy_f_string", [](jl_value_t* jl_string) -> jl_value_t* {
    
    // same for strings
    auto res = fancy_non_lambda_func<std::string>(unbox<std::string>(jl_string));
    return box(res);
});
```
Through tricks like this, any function and any object can be called from Julia, however if we want to directly transfer memory between the two states, we will need to call `box`/`unbox<T>`.

## Arrays

Julia-side objects of type `T <: AbstractArray` are represented C++-side by their own proxy type: `jluna::Array<T, R>`. Just like in Julia, `T` is the value_type and `R` the rank or dimensionality of the array. <br>

Julia array types and `jluna` array types correspond to each other like so:

| Julia          | jluna         |
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
Where, just as before, only named proxies will mutate Julia-side variables.

### Indexing

There are two ways to index a multidimensional array:

+ *linear* indexing treats the array as 1-dimensional and returns the n-th value in column-major order
+ *multidimensional* indexing requires one index per dimension

While both of these styles of indexing are available in Julia, to keep with C-convention, indices in `jluna` are 0-based instead of 1-based.

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

While `jluna` cannot offer list comprehension, `jluna::Array` does allow for Julia-style indexing using a collection:

```cpp
auto sub_array = array.at({2, 13, 1}); // any iterable collection can be used
Base["println"](sub_array)
```
```
[3, 14, 2]
```

To closer illustrate the relationship between indexing in `jluna` and indexing in Julia, consider the following table (where `M` is a N-dimensional array)

| N | Julia |jluna |
|------|-------|--------------------------|
| 1    | `M[1]`| `M.at(0)` or `M[0]`|
| 2    | `M[1, 2]`  | `M.at(0, 1)`|
| 3    | `M[1, 2, 3]`  | `M.at(0, 1)`|
|      |       | |
| Any  | `M[ [1, 13, 7] ]`| `M[ {0, 12, 6} ]` |

### Iterating

In `jluna`, arrays of any dimensionality are iterable in column-major order (just as in Julia):

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

Vector are just arrays, however similarly to `Vector{T}` in Julia, their 1-dimensionality gives them access to additional functions:

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
#include <Julia.h>
```

### Meaning of C-Types

+ `jl_value_t*`: address Julia-side value of arbitrary type
+ `jl_function_t*`: address of Julia-side function
+ `jl_sym_t*`: address of Julia-side symbol
+ `jl_module_t*`: address of Julia-side singleton module
    - `jl_main_module`: Main
    - `jl_base_module`: Base
    - `jl_core_module`: Core
+ `jl_datatype_t*`: address of Julia-side singleton type
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






