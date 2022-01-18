// 
// Copyright 2022 Clemens Cords
// Created on 17.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <exception>
#include <string>
#include <iostream>

struct Test
{
    static inline std::map<std::string, std::string> _failed = {};

    class AssertionException : public std::exception
    {
        public:
            AssertionException(const std::string& s)
                : message(s)
            {}

            virtual const char* what() const noexcept override final
            {
                return message.c_str();
            }

        private:
            std::string message;
    };

    template<typename Lambda_t>
    static void test(const std::string name, Lambda_t&& lambda)
    {
        std::cout << name << ": ";

        std::cout.setstate(std::ios_base::failbit);
        std::cerr.setstate(std::ios_base::failbit);

        bool failed = false;
        std::string what;
        try
        {
            lambda();
        }
        catch (const AssertionException& e)
        {
            failed = true;
            what = "Test::assertion failed";
        }
        catch (const std::exception& e)
        {
            failed = true;
            what = e.what();
        }

        std::cout.clear();
        std::cerr.clear();

        if (failed == false)
            std::cout << "[OK]";
        else
        {
            std::cout << "[FAILED]";
            Test::_failed.insert({name, what});
        }

        std::cout << std::endl;
    }

    static void initialize()
    {
        std::cout << "starting test...\n" << std::endl;
        _failed = std::map<std::string, std::string>();

        // disable julia cout
        assert(jl_is_initialized());
        jl_eval_string(R"(Base.eval(Base, Meta.parse("println(xs...) = return nothing")))");
    }

    static void conclude()
    {
        std::cout << std::endl;
        std::cout << "Number of tests unsuccessful: " << _failed.size() << std::endl;

        for (auto& pair : _failed)
        {
            std::cout << "__________________________________\n";
            std::cout << "| " << pair.first << ": \n|\n";
            std::cout << "| " << pair.second << "\n";
            std::cout << "|_________________________________\n\n" << std::endl;
        }
    }

    static void assert_that(bool b)
    {
        if (not b)
            throw AssertionException("");
    }
};