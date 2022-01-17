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

        if (failed == false)
            std::cout << "[SUCCESSFUL]";
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
    }

    void conclude()
    {
        std::cerr << "Number of tests unsuccesfull: " << _failed.size();

        for (auto& pair : _failed)
        {
            std::cout << "__________________________________\n";
            std::cout << "| " << pair.first << ": \n\n";
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