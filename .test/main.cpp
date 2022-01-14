// 
// Copyright 2021 Clemens Cords
// Created on 17.12.21 by clem (mail@clemens-cords.com)
//

#include <iostream>
#include <jluna.hpp>
#include <functional>
#include <proxy.hpp>
#include <global_utilities.hpp>
#include <array_proxy.hpp>

using namespace jluna;

int main()
{
    // initialize state, always needs to be called first
    State::initialize();

    using namespace jluna;

    State::safe_script("array = Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))");

// array of arbitary type and rank
    Array<jluna::Int64, 3> array = Main["array"];

// access element
    array[3] = 8888;         // 0-based linear indexing
    array.at(0, 1, 2) = 9999; // 0-based multi-dimensional indexing

    State::safe_script("vector = [1, 2, 3, 4, 5, 6, 7, 8, 9]");

// vectors are an Array<T, 1> typedef with some extra functionality
    Vector<int> vector = Main["vector"];
    vector.push_front(0);
    vector.push_back(10);

// both arrays and vectors are iterable
    for (auto it : vector)
        it = it.operator int() + 10; // also assigns the Julia-side array

    State::safe_script(R"(println("array: ", array))");
    State::safe_script(R"(println("vector: ", vector))");

    return 0;
}
    /*

    State::safe_script(R"(
        vec_1d = [1, 2, 3, 4, 5, 6]
        arr_3d = Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))
    )");

    State::safe_script(R"(

        module MyModule

            struct MyType
                _field::Vector{Module}
            end

            instance = MyType([Main, Base, Core]);
        end
    )");

    for (auto& e : Main["MyModule"]["instance"].get_field_names())
        std::cout << e << std::endl;

    return 0;


    std::cout << (std::string) Main["MyModule"]["instance"]["_field"][0]["MyModule"]["instance"]["_field"][1] << std::endl;
    return 0;

Vector<Int64> vec = Main["vec_1d"];
Array<Int64, 3> arr = Main["arr_3d"];

int sum = 0;
for (int i : vec)
    sum += i;

for (auto it : vec)
{
    it = 6;
    std::cout << it.operator Proxy<State>().get_name() << std::endl;
}


std::cout << sum << std::endl;

std::cout << "vector: " << (std::string) vec << "\n";
std::cout << "array : " << (std::string) arr << "\n";

// linear indexing
std::cout << (int) vec[3] << "\n";
std::cout << (int) arr[12] << "\n";

// multi-dimensional indexing
std::cout << (int) vec.at(0) << "\n";
std::cout << (int) arr.at(0, 1, 2) << std::endl;
return 0;

    auto no_name_proxy = State::script("return 123");

    State::script("variable = 123");
    auto named_proxy = Main["variable"];
    std::cout << (no_name_proxy.operator jl_value_t*() == named_proxy.operator jl_value_t*()) << std::endl;
    return 0;

    State::safe_script(R"(
        array = Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))
        vector = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    )");

    Array<Int64, 3> array = Main["array"];

    array[21] = 8888; // linear indexing
    array.at(0, 1, 2) = 9999; // 0-based multidimensional indexing

    Vector<int> vector = Main["vector"];

    for (auto it : vector)
        it = it.operator int() + 10;

    State::safe_script(R"(println("array: ", array))");
    State::safe_script(R"(println("vector: ", vector))");
    return 0;


    /*

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
     */
