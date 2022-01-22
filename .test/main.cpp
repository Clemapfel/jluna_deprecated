// 
// Copyright 2021 Clemens Cords
// Created on 17.12.21 by clem (mail@clemens-cords.com)
//

#include <julia.h>
#include ".c_wrapper/c_adapter.hpp"
#include <state.hpp>
#include <cppcall.hpp>
#include <jluna.hpp>
#include <array_proxy.hpp>

using namespace jluna;

template<typename T>
decltype(auto) fancy_non_lambda_func(T&& t)
{
    do_something_with(t);
    return std::forward<T>(t);
}

int main()
{
    State::initialize();

    State::safe_script("array = Array{Int64, 3}(reshape(collect(1:(3*3*3)), 3, 3, 3))");

   Array<size_t, 2> array = State::script("return [1:2 3:4 5:6]");
Base["println"](array);

for (auto it : array)
    it = 1;

Base["println"](array);
    return 0;
}

/*

#include <iostream>
#include <jluna.hpp>
#include <functional>
#include <proxy.hpp>
#include <global_utilities.hpp>
#include <array_proxy.hpp>
#include <symbol_proxy.hpp>
#include <type_proxy.hpp>
#include <array_proxy.hpp>

#include <thread>

#include <.test/test.hpp>
#include <type_traits>
#include <.src/julia_extension.h>

#include <cppcall.hpp>

using namespace jluna;

int main()
{
    jl_init();

    jl_eval_string(R"(ccall((:initialize, "./libjluna_c_adapter.so"), Cvoid, ()))");
    jl_eval_string("println(cppcall(:test_function, 1, 2, 3))");
    return 0;

    // TEST #############################################################

    Test::initialize();
    Test::test("safe_script: exception forwarding", [](){

        bool thrown = false;
        try
        {
            State::safe_script("sqrt(-1)");
        }
        catch (const JuliaException& e)
        {
            thrown = true;
        }

        Test::assert_that(thrown);
    });

    Test::test("safe_script: syntax error", [](){

        bool thrown = false;
        try
        {
            State::safe_script(R"(test = """\\"\""\)");
        }
        catch (const JuliaException& e)
        {
            thrown = true;
        }

        Test::assert_that(thrown);
    });

    Test::test("create_reference", []() {

        jl_eval_string(R"(
            struct StructType
                any
            end
        )");

        auto hold = Proxy<State>(jl_eval_string("return StructType([99, 2, 3, 4])"), nullptr);

        State::script("jluna.exception_handler._refs[]");
        State::collect_garbage();
        Test::assert_that(((int) hold["any"][0]) == 99);
    });

    Test::test("proxy ctor", [](){

        jl_value_t* val = jl_eval_string("return [1, 2, 3, 4]");
        auto proxy = Proxy<State>(val, nullptr);

        Test::assert_that(jl_unbox_bool(jl_call2(jl_get_function(jl_base_module, "=="), val, (jl_value_t*) proxy)));
    });

    Test::test("proxy trivial dtor", [](){

        jl_value_t* val = jl_eval_string("return [1, 2, 3, 4]");
        size_t n = 0;
        {
            auto proxy = Proxy<State>(val, nullptr);
            n = jl_unbox_int64(jl_eval_string("return length(jluna.memory_handler._refs.x)"));
        }

        Test::assert_that(n - jl_unbox_int64(jl_eval_string("return length(jluna.memory_handler._refs.x)")) == 1);
    });

    Test::test("proxy inheritance dtor", [](){

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

        Test::assert_that((bool) inner.get());
    });

    Test::test("proxy reject as non-vector", [](){

        jl_eval_string(R"(
            struct NonVec
                _field
            end
        )");

        auto vec = Proxy<State>(jl_eval_string("return [1, 2, 3, 4]"), nullptr);
        auto non_vec = Proxy<State>(jl_eval_string("return NonVec([9, 9, 9, 9])"), nullptr);

        try
        {
            Test::assert_that((int) vec[0] == 1);
        }
        catch (...)
        {
            Test::assert_that(false);
        }

        try
        {
            Test::assert_that((int) non_vec[0] == 1);
            Test::assert_that(false);   // fails if no exception thrown
        }
        catch (...)
        {}
    });

    Test::test("proxy reject as non-function", [](){

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
            Test::assert_that((int) func(1, 2, 3, 4) == 10);
        }
        catch (...)
        {
            Test::assert_that(false);
        }

        try
        {
            non_func();
            Test::assert_that(false);   // fails if no exception thrown
        }
        catch (...)
        {}
    });

    Test::test("proxy reject as non-struct", [](){

        State::safe_script(R"(
            struct NewStructType
                _field
                NewStructType() = new(true)
            end

            f(xs...) = return sum([xs...])
            instance = NewStructType()
        )");

        auto non_struct = Main["f"];
        auto is_struct = Main["instance"];

        try
        {
            Test::assert_that((bool) is_struct["_field"]);
        }
        catch (...)
        {
            Test::assert_that(false);
        }

        try
        {
            auto res = non_struct["_field"];
            Test::assert_that(false);   // fails if no exception thrown
        }
        catch (...)
        {}
    });

    Test::test("proxy fieldnames", [](){

        Test::assert_that(false);
     });

    Test::test("proxy mutation", [](){

        jl_eval_string("variable = [1, 2, 3, 4]");

        auto mutating_proxy = Main["variable"];

        Test::assert_that(mutating_proxy.is_mutating());
        mutating_proxy[0] = 9999;

        Test::assert_that(mutating_proxy[0].operator Int64() == 9999);
        Test::assert_that(jl_unbox_int64(jl_eval_string("variable[1]")) == 9999);


        auto non_mutating_proxy = State::script("return variable");
        non_mutating_proxy = 8888;

        Test::assert_that(non_mutating_proxy.operator Int64() == 8888);
        Test::assert_that(jl_unbox_int64(jl_eval_string("variable[1]")) != 8888);
    });

    Test::test("proxy cast", []() {

        State::safe_script(R"(
            symbol = Symbol("")
            array = [1, 2, 3, 4]
            type = Type
        )");

        Symbol s = Main["symbol"].as<Symbol>();
        Test::assert_that(s.operator std::string() == "");

        Array<Int64, 1> a = Main["array"].as<Array<Int64, 1>>();
        Test::assert_that((int) a.at(0) == 1);
    });

    auto test_box_unbox = []<typename T>(const std::string type_name, T&& value)
    {
        Test::test("box/unbox " + type_name , [value]() {

            jl_value_t* boxed = box<T>(value);
            Test::assert_that(unbox<T>(boxed) == value);

            boxed = box<T>(T());
            Test::assert_that(unbox<T>(boxed) == T());
        });
    };

    test_box_unbox("Bool", Bool(true));
    test_box_unbox("Char", Char(12));
    test_box_unbox("String", std::string("abc"));
    test_box_unbox("Int8", Int8(12));
    test_box_unbox("Int16", Int16(12));
    test_box_unbox("Int32", Int32(12));
    test_box_unbox("Int64", Int64(12));
    test_box_unbox("UInt8", UInt8(12));
    test_box_unbox("UInt16", UInt16(12));
    test_box_unbox("UInt32", UInt32(12));
    test_box_unbox("UInt64", UInt64(12));
    test_box_unbox("Float32", Float32(0.01));
    test_box_unbox("Float64", Float64(0.01));
    test_box_unbox("Complex", std::complex<double>(0, 1));

    test_box_unbox("Pair", std::pair<size_t, std::string>(12, "abc"));
    test_box_unbox("Tuple3", std::tuple<size_t, std::string, float>(12, "abc", 0.01));

    auto test_box_unbox_iterable = []<typename T>(const std::string& name, T&& value){

        Test::test("box/unbox " + name, [&value](){

            jl_value_t* boxed = box(value);
            auto unboxed = unbox<T>(boxed);

            Test::assert_that(value == unboxed);
        });
    };

    test_box_unbox_iterable("Vector", std::vector<size_t>{1, 2, 3, 4});
    test_box_unbox_iterable("IdDict", std::map<size_t, std::string>{{12, "abc"}});
    test_box_unbox_iterable("Dict", std::unordered_map<size_t, std::string>{{12, "abc"}});
    test_box_unbox_iterable("Set", std::set<size_t>{1, 2, 3, 4});

    Test::test("vector: ctor", [](){

    });

    Test::test("vector: insert", [](){

    });

    Test::test("vector: erase", [](){

    });

    Test::test("vector: append", [](){

    });

    Test::test("array: Nd at", [](){

    });

    Test::test("array: out of range", [](){
        // check ouf of range in all dimensions
    });

    Test::test("array_iterator: +/-", [](){

        // check forward and backwards iteration
    });

    Test::test("array_iterator: on owner reassignment", [](){

        // check behavior if owner proxy gets reassigned during iteration
        // unlike proxy, should segfault
    });

    Test::test("array_iterator: cast to value", [](){

        // check unboxing call corresponds to value_t
    });

     Test::test("array_iterator: cast to proxy", [](){

         // check name and assignment behavior
    });

    Test::conclude();
}
 */