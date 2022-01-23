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
    State::initialize();

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

        State::safe_script(R"(

            struct FieldStruct
                _a
                _b
                _☻
            end
        )");

        auto proxy = State::safe_script("return FieldStruct");
        auto names = proxy.get_field_names();

        Test::assert_that(names.at(0) == "_a" and names.at(1) == "_b" and names.at(2) == "_☻");
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

    Test::test("array: ctor", [](){

        State::safe_script("vector = [999, 2, 3, 4, 5]");
        Vector<int> vec = Main["vector"];

        Test::assert_that(vec.at(0).operator int() == 999);
    });

    Test::test("array: reject wrong type", []()
    {
        Test::assert_that(false);
        try
        {
            Array<size_t, 1> arr = State::script(R"(return ["abc", "def"])");
            arr.at(0) = "string";
        }
        catch(...)
        {}
    });

    Test::test("array: front/back", []()
    {
        State::safe_script("vector = [999, 2, 3, 4, 666]");
        Array1d vec = Main["vector"];
        Test::assert_that(vec.front().operator int() == 999 and vec.back().operator int() == 666);
    });

    Test::test("array: empty", []()
    {
        State::safe_script("vector = []");
        Array1d vec = Main["vector"];
        Test::assert_that(vec.empty());
    });

    Test::test("array: Nd at", [](){

        State::safe_script("array = reshape(collect(1:27), 3, 3, 3)");
        Array3d vec = Main["array"];

        static auto getindex = [&](size_t a, size_t b, size_t c) -> size_t
        {
            jl_function_t* _getindex = jl_get_function(jl_base_module, "getindex");
            std::vector<jl_value_t*> args;
            args.push_back((jl_value_t*) vec);
            args.push_back(jl_box_uint64(a));
            args.push_back(jl_box_uint64(b));
            args.push_back(jl_box_uint64(c));

            return jl_unbox_int64(jl_call(_getindex, args.data(), args.size()));
        };

        Test::assert_that(getindex(1, 2, 3) == (size_t) vec.at(0, 1, 2));
        Test::assert_that(getindex(3, 3, 3) == (size_t) vec.at(2, 2, 2));
    });

    Test::test("array: out of range", [](){
        State::safe_script("array = reshape(collect(1:27), 3, 3, 3)");
        Array3d arr = Main["array"];

        static auto test = [&](size_t a, size_t b, size_t c) -> bool
        {
            try
            {
                arr.at(a, b, c);
            }
            catch (...)
            {
                return true;
            }

            return false;
        };

        Test::assert_that(test(-1, 2, 2));
        Test::assert_that(test(3, 2, 2));
        Test::assert_that(test(2, 3, 2));
        Test::assert_that(test(2, 2, 3));
    });

    Test::test("array_iterator: +/-", [](){

        State::safe_script("array = reshape(collect(1:27), 3, 3, 3)");
        Array3d arr = Main["array"];

        auto it = arr.begin();
        it++;
        Test::assert_that((int) it == 2);

        it = arr.end();
        it--;
        Test::assert_that((int) it == 27);
    });

    Test::test("array_iterator: on owner reassignment", [](){

        // check behavior if owner proxy gets reassigned during iteration
        // unlike proxy, should segfault

        State::safe_script("vec =[1:20]");
        Array3d arr = Main["vec"];

        auto it = arr.begin();
        {
            size_t i = 0;
            while (i < 10)
            {
                i++;
            }
        }

        State::safe_script("vec = [1, 2, 3, 4, 5]");

        bool thrown = false;
        try
        {
            size_t as = *it;
        }
        catch(jluna::JuliaException& )
        {
            thrown = true;
        }

        Test::assert_that(thrown);
    });

    Test::test("array_iterator: comparison", []()
    {
        State::safe_script("array = reshape(collect(1:27), 3, 3, 3)");
        Array3d arr = Main["array"];

        auto a = arr.begin();
        auto b = arr.begin();
        auto c = arr.end();

        Test::assert_that(a == b and a != c);
        b++;
        Test::assert_that(a != b and b != c);
    });

    Test::test("array_iterator: cast to value", [](){

        State::safe_script("array = reshape(collect(1:27), 3, 3, 3)");
        Array3d arr = Main["array"];

        bool thrown = false;
        try
        {
            arr.begin().operator std::vector<std::string>();
        }
        catch (...)
        {
            thrown = true;
        }

        Test::assert_that(thrown);

        Test::assert_that(arr.begin().operator Int64() == 1);
    });

    Test::test("array_iterator: cast to proxy", [](){

        State::safe_script("array = reshape(collect(1:27), 3, 3, 3)");
        Array3d arr = Main["array"];

        auto it = arr.at(0, 0, 0);
        Proxy<State> as_proxy = it;

        Test::assert_that(as_proxy.get_name() == "array[1]");
    });

    Test::test("vector: insert", [](){

        State::safe_script("vector = [1, 2, 3, 4]");
        Vector<size_t> vec = Main["vector"];

        vec.insert(0, 16);
        Test::assert_that((int) vec.at(0) == 16);
    });

    Test::test("vector: erase", [](){

        State::safe_script("vector = [1, 99, 3, 4]");
        Vector<size_t> vec = Main["vector"];

        vec.erase(0);
        Test::assert_that((int) vec.at(0) == 99 and vec.size() == 3);
    });

    Test::test("vector: append", [](){
        State::safe_script("vector = [1, 1, 1, 1]");
        Vector<size_t> vec = Main["vector"];

        vec.push_front(999);
        vec.push_back(666);
        Test::assert_that(vec.size() == 6 and vec.front<int>() == 999 and vec.back<int>() == 666);
    });

    Test::test("C: initialize adapter", []()
    {
        try
        {
            jluna::c_adapter::initialize();
        }
        catch(...)
        {
            Test::assert_that(false);
        }
    });

    Test::test("C: hash", [](){

        const std::string str = "(±)☻aödunÖAOA12891283912";

        size_t a = jluna::c_adapter::hash(str);
        size_t b = jl_unbox_uint64(jl_call1(jl_get_function(jl_base_module, "hash"), (jl_value_t*) jl_symbol(str.data())));

        Test::assert_that(a == b);
    });

    Test::test("C: register/unregister", [](){

        std::string name = "test";
        size_t id = c_adapter::hash(name);
        register_function(name, []() -> void {});
        Test::assert_that(c_adapter::is_registered(id));

        c_adapter::unregister_function(name);
        Test::assert_that(not c_adapter::is_registered(id));
    });

    Test::test("C: reject name", [](){

        bool thrown = false;
        try
        {
            register_function(".", []() -> void {});
        }
        catch (...)
        {
            thrown = true;
        }

        Test::assert_that(thrown);
    });

    Test::test("C: undefined symbol", [](){

        bool thrown = false;

        try
        {
            c_adapter::throw_undefined_symbol("name");
        }
        catch (...)
        {
            thrown = true;
        }

        Test::assert_that(thrown);
    });

    Test::test("C: call success", [](){

        register_function("test", [](jl_value_t* in) -> jl_value_t* {

            auto as_int = jl_unbox_int64(in);
            as_int += 11;
            return jl_box_int64(as_int);
        });

        State::safe_script("@assert cppcall(:test, 100) == 111");
    });

    Test::test("C: not registered", [](){

        bool thrown = false;
        try
        {
            State::safe_script("cppcall(:unnamed)");
        }
        catch (...)
        {
            thrown = true;
        }

        Test::assert_that(thrown);
    });

    Test::test("C: forward exception", [](){

        register_function("test", []() -> void {

            throw std::out_of_range("123");
        });

        bool thrown = false;

        try
        {
            State::safe_script("cppcall(:test)");
        }
        catch (...)
        {
            thrown = true;
        }
    });


    Test::conclude();
}