# jlwrap: A modern and convenient C++ <-> Julia Wrapper

(this documentation is still in development)

While Julias design and documentation are beautiful and amazing, the design of the C-Side of Julias C-API is much less beautiful and much, much less... documented. `jlwrap` aims to give C++ users a way to easily integrate Julia into their C++ project while taking full advantage of the languages perks and performance, all without having to deal with a single line of pure C code.

Features:
+ Modifying C++-Side Proxies modify the Julia-Side Variables at the same Time
+ All Julia-Side Memory with a C++-Side Variable holding Ownership will not be Garbage Collected until safely released
+ Fast! Everything is written to achieve the minimum amount of overhead possible, compared to the C-API
+ Wraps all of Julias Datatypes into their modern C++20 Equivalents, rather than C. This included pointers, smart points and references
+ C-macros in Julia are directly supported and affect the C++ Side just the same
+ Exception handling! Julia has it, C doesn't but C++ does so `jlwrap` will too
+ 
