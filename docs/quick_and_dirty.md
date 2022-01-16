# jluna: Quick & Dirty

This page will give a fly-by, cheat-sheet-like overview of all relevant features and syntax. While it is useful for referencing back to, it is still recommended that you read the tutorials on [jluna::State](./state.md) and [jluna::Proxy](./proxies.md) at some point.

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
   6.1 [Accessing Functions](#accessing-functions)<br>
  6.2 [Calling Functions](#calling-functions)<br>
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

## Executing Code

```cpp
// without exception forwarding
State::script("your inline code");
State::script(R"(
    your
    multi-line
    code
)");

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

### Manual
```cpp
// to send a C++ value to Julia, it needs to be boxed:
size_t cpp_side = 1001;
auto* jl_side = box(cpp_side); // this value is now allocated in Julia

// to send a Julia value to C++, it needs to be unboxed:
size_t back_cpp_side = unbox<size_t>(jl_side);

std::cout << back_cpp_side << std::endl;
```
```
1001
```

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

### List of (Un)Boxables

The following types are both boxable and unboxable out-of-the-box and appear julia-side as:
```cpp
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

jluna::Proxy<State>      -> /* type deduced during runtime */
jluna::Symbol            -> Symbol
jluna::Type              -> Type
jluna::Array<T, R>       -> Array{T, R} 
jluna::Vector<T>         -> Vector{T}
jluna::JuliaException    -> Exception

std::string              -> String
std::complex<T>          -> Complex{T}
std::vector<T>           -> Vector{T}
std::array<T, R>         -> Vector{T}
std::pair<T, U>          -> Pair{T, U}
std::tuple<Ts...>        -> Tuple{Ts...}
std::map<T, U>           -> IdDict{T, U}
std::unordered_map<T, U> -> Dict{T, U}
std::set<T>              -> Set{T, U}

where T, U are also (Un)Boxables
```
## Accessing Variables
### Mutating Variables

```cpp
State::script("variable_name = 123");

auto by_name = Main["variable_name"];
by_name = 456;  // modifies julia-side variable
auto by_value = State::script("return variable_name");
by_value = 789; // does not modify

State::script("println(variable_name));
```
```
456
```

(see the [tutorial on proxies](./proxies.md) if this behavior seems unintuitive to you)

### Accessing Fields

```cpp
State::script(R"(
    struct StructType
        _field
    end

    instance = StructType(123);
)");

auto field_proxy = Main["instance"]["_field"];
field_proxy = 321;

State::script("println(Main.instance._field)");
```
```
321
```

## Functions

### Accessing Functions
```cpp
State::script("f(x) = sqrt(x^x^x)");

auto f = Main["f"];

// or
auto f = State::script("return f");
```

### Calling Functions
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

## Arrays

### Multi-Dimensional Arrays

```cpp
State::script("array = Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))");

jluna::Array<Int64, 3> array = Main["array"];

// 0-based linear indexing
array[12] = 9999;

// 0-based multi-dimensional indexing
array.at(0, 1, 2) = 9999;

// column-major order iterable
for (Int64 i : array)
    std::cout << i << " ";
```
```
1 2 3 4 5 6 7 8 9 10 11 12 9999 14 15 16 17 18 19 20 21 9999 23 24 25 26 27 
```

### Vectors

```cpp
State::script("vector = collect(1:10)");
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






