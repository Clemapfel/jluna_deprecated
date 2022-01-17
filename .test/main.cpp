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
    test::initialize();

    test::test("state_initialize", [](){

        State::initialize();
        test::assert_that(jl_is_initialized());
    });

    test::test("safe_script", [](){

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

    test::conclude();


}