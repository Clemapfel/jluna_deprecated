# `jluna`: A modern and convenient C++ ⇔ Julia Interface

## (this documentation and codebase is still incomplete. For the version actively being worked on, see: https://github.com/Clemapfel/jlwrap/tree/import)

While Julias design and documentation are beautiful and amazing, the design of the C-Side of Julias C-API is much less beautiful and much, much less... documented. `jlwrap` aims to give C++ users a way to easily integrate Julia into their C++ project while taking full advantage of the languages perks and performance, all without having to deal with a single line of pure C code.

Features:
+ **Fast!** 

  Everything is written to achieve (compared to the C-API) the minimum amount of overhead possible
  
+ **Safe!** 

  Exception handling is great, Julia has it, C doesn't but C++ does so `jlwrap` will too. This means you will be able to catch errors before they break things julia-side. Futhermore any memory managed by C++ is safe from being garbage collected julia-side while it is in use.
  
+ **Easy!**

  jlwrap detects julia on your system and automatically builds and links it correctly, all you need to do is have julia installed, jlwrap handles the rest.
  
+ **Well Documented!** 

  Full tutorials and in-line documentation intended for humans mean that you will always be able to just quickly look up how things work. Julia-side code is furthermore fully documented for use with `?` (help)

More Features:
+ Wraps all of Julias Datatypes into their modern C++20 Equivalents, including `a => b` to `std::pair`, `Array` to std::vector, `mutable struct` to `struct`, etc. 
+ C-macros in Julia are directly supported and affect the C++ Side just the same but new jlwrap-specific macros are also provided
