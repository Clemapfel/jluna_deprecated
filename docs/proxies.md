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

Where the first version is encouraged for style reasons. `jluna` handles implicit conversion behind-the scene, this makes it so we don't have to worry what the actual type of the julia-value is. `jluna` will try it's hardest to make your declaration work:

```cpp
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

Of course if the type of the julia variable cannot be converted to the target type, an exception is thrown:

```cpp
std::vector<double> as_what = proxy;
```
```
terminate called after throwing an instance of 'jluna::JuliaException'
  what():  MethodError: Cannot `convert` an object of type Int64 to an object of type Vector
```

This is already much more convenient than manually unboxing c-pointers, however the true usefulness of proxies comes in their ability to mutate julia-side values.

## Mutating Variables

As state before, a proxies holds exactly one pointer to julia-side memory and exactly one symbol. There are two types of symbols:

+ a symbol starting with the character `#` is called an *internal id*
+ any other symbol is called a *name*

The behavior of proxies changes, depending on wether their symbol is a name or not. A proxies whos symbol is a name is called an **named proxy**, a proxy whos symbol is an internal id is called an **unnamed proxy**. 

To generate an unnamed proxy, we use `State::(safe_)script`, to generate a named proxy we use `operator[]`

```cpp
State::safe_script("var = 1234");

auto unnamed_proxy = State::safe_script("return var");
auto named_proxy = Main["var"];
```

where `Main`, `Base`, `Core` are global, static, pre-initialized proxies holding their corresponding julia-side module.

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

#### Named Proxies

**If a proxy is named, assigning it will change the corresponding variable julia-side**:

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

We see that both the value `named_proxy` is pointing to and the variable of the same name "`Main.var`" were affected by the assignment. This is somewhat atypical for julia but very familiar to C++-users. A named proxy acts like a reference. <br><br> Because of this behavior, it lets us do things like:

```cpp
State::safe_script("vector_var = [1, 2, 3, 4]")
Main["vector_var"][0] = 999;   // indices are 0-based in C++
Base["println"]("julia prints: ", Main["vector_var"]);
```
```
julia prints: [999, 2, 3, 4]
```

Which is highly convenient. 

#### Unnamed Proxy

What happens if we mutate an unnamed proxy?

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

We see that it does mutate the C++-side proxy `unnamed_proxy` but does not mutate the julia-side `var`. This makes sense, if we check `unnamed_proxy`s symbol again:

```cpp
std::cout << unnamed_proxy.get_name() << std::endl;
```
```
<unnamed proxy #9>
```

We see that is does not have a name, just an internal id. Therefore it has no way to know where it's value came from and thus does not mutate anything but the C++ variable.

#### In Summary

+ We create a **named proxy** using `Main["var"]`. Assigning a named proxy mutates its value and mutates the corresponding julia-side variable of the same name
+ We create an **unnamed proxy** using `State::(safe_)script("return var")`. Assigning an unnamed proxy mutates it's value but does not mutate any julia-side variable

This is important to realize and is the basis of much of `jluna`s syntax.

#### Detached Proxies

Consider the following code:

```cpp
State::safe_script("var = 1234")

auto named_proxy = Main["variable"];

State::safe_script(R"(var = ["abc", "def"])");

std::cout << name_proxy.operator std::string() << std::endl;
```

What will this print? We know it is a named proxy so it should correspond to the variable `var`, however we reassigned `var` to a completely different value using only julia. `named_proxy` was never made aware of it, so it still currently points to its old value:

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

This proxy is in what's called a *detached state*. Even though it is a named proxy, it's current value does not correspond to the value of it's julia-side variable. This may have unforeseen consequences:












