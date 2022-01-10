// 
// Copyright 2021 Clemens Cords
// Created on 17.12.21 by clem (mail@clemens-cords.com)
//

#include <iostream>
#include <jluna.hpp>

using namespace jluna;

int main()
{
    // initialize state, always needs to be called first
    State::initialize();

    // running code:
    State::safe_script("println(\"hello jluna\")"); // hello jluna

    // accessing variables by value: use State::safe_script()
    auto value_123 = State::safe_script("return 123");
    int as_int = value_123;

    std::cout << as_int << std::endl; // 123

    // accessing variables by reference: use operator[]
    State::safe_script("julia_value_456 = 456");
    auto value_456 = Main["julia_value_456"];
    std::cout << ((int) value_456) << std::endl; // 456
    make_mutating(value_456);
    value_456 = 789; // this modifies the variable in main
    State::safe_script("println(Main.julia_value_456)"); // prints: 789

    State::safe_script(R"(
        mutable struct StructType
            _field

            StructType() = new(undef)
        end

        struct_type_instance = StructType();
    )");

    auto field = Main["struct_type_instance"]["_field"];
    make_mutating(field);
    field = 101112;
    forward_last_exception();
    State::safe_script("println(struct_type_instance._field)"); // prints: 101112

}