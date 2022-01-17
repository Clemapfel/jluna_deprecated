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

    test::test("proxy ctor", [](){

        jl_value_t* val = jl_eval_string("return [1, 2, 3, 4]");
        auto proxy = Proxy<State>(val, nullptr);

        test::assert_that(jl_unbox_bool(jl_call2(jl_get_function(jl_base_module, "=="), val, (jl_value_t*) proxy)));
    });

    test::test("proxy trivial dtor", [](){

        jl_value_t* val = jl_eval_string("return [1, 2, 3, 4]");
        size_t n = 0;
        {
            auto proxy = Proxy<State>(val, nullptr);
            n = jl_unbox_int64(jl_eval_string("return length(jluna.memory_handler._refs.x)"));
        }

        test::assert_that(n - jl_unbox_int64(jl_eval_string("return length(jluna.memory_handler._refs.x)")) == 1);
    });

    test::test("proxy inheritance dtor", [](){

        jl_eval_string(R"(
            struct Inner
                _field
            end

            struct Outer
                _inner::Inner
            end

            instance = Outer(Inner(true))
        )");

        std::unique_ptr<Proxy<State>> inner;

        {
            auto outer = Main["instance"];
            inner = std::make_unique<Proxy<State>>(outer["_inner"]["_field"]);
        }
        State::collect_garbage();

        test::assert_that((bool) inner.get());
    });

    test::test("proxy reject as non-vector", [](){

        jl_eval_string(R"(
            struct NonVec
                _field
            end
        )");

        auto vec = Proxy<State>(jl_eval_string("return [1, 2, 3, 4]"), nullptr);
        auto non_vec = Proxy<State>(jl_eval_string("return NonVec([9, 9, 9, 9])"), nullptr);

        try
        {
            test::assert_that((int) vec[0] == 1);
        }
        catch (...)
        {
            test::assert_that(false);
        }

        try
        {
            test::assert_that((int) non_vec[0] == 1);
            test::assert_that(false);   // fails if no exception thrown
        }
        catch (...)
        {}
    });

    test::test("proxy reject as non-function", [](){

        jl_eval_string(R"(
            struct NonVec
                _field
                NonVec() = new(1)
            end

            f(xs...) = return sum([xs...])
            non_f = NonVec()
        )");

        auto func = Main["f"];
        auto non_func = Main["non_f"];

        try
        {
            test::assert_that((int) func(1, 2, 3, 4) == 10);
        }
        catch (...)
        {
            test::assert_that(false);
        }

        try
        {
            non_func();
            test::assert_that(false);   // fails if no exception thrown
        }
        catch (...)
        {}
    });




    test::conclude();


}