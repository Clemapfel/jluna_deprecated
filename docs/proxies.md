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

Or we can use the C-API, receive a pointer and then unbox that pointer:

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

