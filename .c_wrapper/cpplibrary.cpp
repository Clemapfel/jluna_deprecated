#include "cpplibrary.hpp"

#ifdef __cplusplus

#include <iostream>

extern "C"
{
    void cpp_print(const char* c)
    {
        std::cout << c << std::endl;
    }

    void cpp_void()
    {
        std::cout << "void" << std::endl;
    }
}

#endif

// g++ -fpic -shared cpplibrary.cpp -o -libcpplibrary
