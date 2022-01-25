## Accessing Variables

Let's say we have a variable `var` julia-side:

```cpp
State::script("var = 1234")
```

To access the value of that variable, we can use `State::safe_return<T>`:
```cpp
// access as int
auto as_int = State::safe_return<int>("var");
std::cout << as_int << std::endl;
```
```
1234
```

Or we can use the C-API, receive a pointer to the memory `var` holds and then unbox that pointer:

```cpp
// get raw address
jl_value_t* var_ptr = jl_eval_string("return var");
auto as_int = unbox<int>(var_ptr);

std::cout << as_int << std::endl;
```
```
1234
```

While both ways get us the desired value, neither is a good way to actually manage the variable itself. How do we reassign it? Can we dereference that pointer? Who has ownership of the memory? All these questions are hard to manage using the C-API, however `jluna` offers a one-stop-shop solution for all of these problems: `jluna::Proxy`.

A proxy holds two things: the memory address of its value and a symbol. We'll get to the symbol later, for now let's focus on the memory:<br>
Memory held by a proxy is safe from the julia garbage collector (GC) and assured to be valid. This means you don't have to worry about keeping a reference or pausing the GC. Any memory, be it temporary or something explicitly referenced by a julia-side variable, is guaranteed to be safe to dereference. How do we do that? First, we need to create a proxy, this is most often done implicitly like so:

```cpp
State::script("var = 1234")
auto proxy = State::script("return var")
```
Use of `auto`, here, simplifies the declaration and is encouraged whenever possible.<br>

Now that we have the proxy we need to convert it to a value, unlike the C-APIs `jl_value_t*`, this is done implicitly or via `operator T()`:

```cpp
// all following ways are exactly equivalent:

int as_int = proxy;   // recommended

auto as_int = proxy.operator int();

auto as_int = static_cast<int>(proxy);

auto as_int = (int) proxy;
```

Where the first version is encouraged for style reasons. `jluna` handles implicit conversion behind-the scene, this makes it so we don't have to worry what the actual type of the julia-value is:

```cpp
size_t as_size_t = proxy;
std::string as_string = proxy;

std::cout << "size_t: " << as_size_t << std::endl;
std::cout << "string: " << as_string << std::endl;
```
```
1234
"1234"
```

Of course if the type of the julia variable cannot be converted to the target type, an exception is thrown:

```cpp
std::complex<double> as_what = proxy;
```
```
terminate called after throwing an instance of 'jluna::JuliaException'
  what():  AssertionError: expected Complex but got an object of type Int64
Stacktrace:
 [1] assert_isa(x::Int64, type_name::Symbol)
   @ Main.jluna ~/Workspace/jluna/.src/julia/common.jl:116
 [2] safe_call(::Function, ::Int64, ::Symbol)
   @ Main.jluna.exception_handler ~/Workspace/jluna/.src/julia/exception_handler.jl:80

signal (6): Aborted
```

Now that we know how to "dereference" the proxy and get its value, we can learn how to 

## Mutating Variables






