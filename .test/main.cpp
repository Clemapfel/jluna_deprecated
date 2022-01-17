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

#include <.test/test.hpp>

using namespace jluna;

int main()
{
    State::initialize();
    assert(jl_is_initialized());


    jl_eval_string(R"(
        struct StructType
            any
        end
    )");

    auto p = Proxy<State>(jl_eval_string("return StructType([99, 2, 3, 4])"), nullptr)["any"][0];

    State::collect_garbage();

    std::cout << (Int64) p << std::endl;

    State::safe_script(R"(

    function println(dict::Dict{T, U}) where {T, U}

        for e in dict
            Base.println(e)
        end
    end

    Main.println(jluna.memory_handler._refs.x);
    )");

    return 0;


    // ##################

    test::initialize();

    test::test("safe_script: exception forwarding", [](){

        bool thrown = false;
        try
        {
            State::safe_script("sqrt(-1)");
        }
        catch (const JuliaException& e)
        {
            thrown = true;
        }

        test::assert_that(thrown);
    });

    test::test("safe_script: syntax error", [](){

        bool thrown = false;
        try
        {
            State::safe_script(R"(test = """\\"\""\)");
        }
        catch (const JuliaException& e)
        {
            thrown = true;
        }

        test::assert_that(thrown);
    });

    test::test("create_reference", []() {

        jl_eval_string(R"(
            struct StructType
                any
            end
        )");

        auto hold = Proxy<State>(jl_eval_string("return StructType([99, 2, 3, 4])"), nullptr);

        State::script("jluna.exception_handler._refs[]");
        State::collect_garbage();
        test::assert_that(((int) hold["any"][0]) == 99);
    });

    test::conclude();


}