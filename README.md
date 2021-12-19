# `jlwrap`: A modern and convenient C++ ⇔ Julia Wrapper and Interface

(this documentation is still incomplete)

While Julias design and documentation are beautiful and amazing, the design of the C-Side of Julias C-API is much less beautiful and much, much less... documented. `jlwrap` aims to give C++ users a way to easily integrate Julia into their C++ project while taking full advantage of the languages perks and performance, all without having to deal with a single line of pure C code.

Features:
+ Modifying C++-Side Proxies modify the Julia-Side Variables at the same Time, as well as vice versa
+ Fast! Everything is written to achieve the minimum amount of overhead possible, compared to the C-API
+ All Julia-Side Memory that is owned only by a C++-Side reference will not be Garbage Collected until safely released
+ Wraps all of Julias Datatypes into their modern C++20 Equivalents, rather than C
+ C-macros in Julia are directly supported and affect the C++ Side just the same but new jlwrap-specific macros are also provided
+ Exception handling! Julia has it, C doesn't but C++ does so `jlwrap` will have it too making executing scripts of unknown origin much safer
+ 
