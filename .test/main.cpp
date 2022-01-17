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

#include <thread>

#include <.test/test.hpp>

using namespace jluna;

int main()
{
    State::initialize();

    test::initialize();

    /*
    test::test("state_initialize", [](){

        State::initialize();
        test::assert_that(jl_is_initialized());
    });
     */

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

    test::test("free_reference", [](){

        auto* val = jl_eval_string("return [1, 2, 3]");
        size_t key = State::create_reference(val);

        size_t before = jl_unbox_int64(jl_eval_string("return length(jluna.memory_handler._refs.x)"));

        State::free_reference(key);
        size_t after = jl_unbox_int64(jl_eval_string("return length(jluna.memory_handler._refs.x)"));

        test::assert_that(abs(static_cast<int>(before) - static_cast<int>(after)) == 1);
    });

    test::conclude();


}