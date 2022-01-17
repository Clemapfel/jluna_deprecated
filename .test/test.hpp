// 
// Copyright 2022 Clemens Cords
// Created on 17.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <exception>
#include <string>
#include <iostream>

namespace test
{
    static std::map<std::string, std::string> _failed;

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
    void test(const std::string name, Lambda_t&& lambda)
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
            what = "test::assertion failed";
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
            test::_failed.insert({name, what});
        }

        std::cout << std::endl;
    }

    void initialize()
    {
        std::cerr << "starting test..." << std::endl;
        _failed = std::map<std::string, std::string>();

        // disable julia cout
        //jl_eval_string(R"(Base.eval(Meta.parse("println(xs...) = begin end")))");
    }

    void conclude()
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

    void assert_that(bool b)
    {
        if (not b)
            throw AssertionException("");
    }
}