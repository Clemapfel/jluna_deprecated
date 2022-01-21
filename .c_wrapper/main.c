#include <julia.h>
#include "c_adapter.hpp"

int main()
{
    initialize();

    jl_eval_string("ccall((:initialize, \"./libjluna_c_adapter.so\"), Cvoid, ()))");
    jl_eval_string("println(cppcall(:test_function, 1, 2, 3))");

    jl_atexit_hook(0);
    return 0;
}