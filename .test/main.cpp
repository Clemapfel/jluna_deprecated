// 
// Copyright 2021 Clemens Cords
// Created on 17.12.21 by clem (mail@clemens-cords.com)
//

#include <iostream>

#include <primitive_proxy.hpp>
#include <state.hpp>
#include <julia_extension.h>


using namespace jlwrap;

int main()
{
    State::initialize();
    State::script("x = function() print(1) end");

    for (auto& s : State::get_module_names())
        std::cout << s << std::endl;

}
