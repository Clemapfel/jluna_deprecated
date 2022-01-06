# jluna: Quick & Dirty

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

Only after this call, all functionality will be available so make sure to always initialize it first. You don't have to worry about "shutting down", all cleaning up and proper deallocation julia-side is done automatically on exit. If you wish to do so manually anyway, you do so via

```cpp
jluna::State::shutdown();
```
```
[JULIA][LOG] Shutting Down...
```

However, this is usually unnecessary unless in an heavily restricted environment where every bit of RAM counts.

## 2. State

All interaction with julia happens in some way through `jluna::State`. This is a C++ union class with only static methods so there is only exactly one state and it cannot be instanced.

To start out let us call a single line of julia code:

```cpp
jluna::State::script("println(\"hello luna\"");
```
```
hello luna
```

In julia, this function returns `::Nothing` however, often we want to access the values returned by a julia function. `State::script` actually always returns a value, in the above example we simply discarded it. If we want to capture it we do it like so:

```cpp
auto returned_value = jluna::State::script("return 123");
```

What `auto` deduces into here is relevant. All julia-values are at first bound to a *proxy*, this proxy has ownership of the julia-side value and sports some member function of it's own, however to use it as an int, we first need to convert it to one. This can be done by simply assigning a new variable:

```
int as_int = return_value;
as_int += 1;
std::cout << as_int << std::endl;
```
