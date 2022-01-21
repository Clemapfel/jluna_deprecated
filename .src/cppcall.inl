// 
// Copyright 2022 Clemens Cords
// Created on 20.01.22 by clem (mail@clemens-cords.com)
//

#include <cppcall.hpp>
#include <.c_wrapper/c_adapter.hpp>
#include <typedefs.hpp>
#include <exceptions.hpp>

namespace jluna
{
    namespace detail
    {
        template<typename Lambda_t, typename Return_t, typename... Args_t, std::enable_if_t<std::is_same_v<Return_t, void>, Bool> = true>
        jl_value_t* detail::invoke_lambda(const Lambda_t* func, Args_t... args)
        {
            (*func)(args...);
            return jl_nothing;
        }

        template<typename Lambda_t, typename Return_t, typename... Args_t, std::enable_if_t<std::is_same_v<Return_t, jl_value_t*>, Bool> = true>
        jl_value_t* detail::invoke_lambda(const Lambda_t* func, Args_t... args)
        {
            jl_value_t* res = (*func)(args...);
            return res;
        }
    }

    template<detail::LambdaType<> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        THROW_IF_UNINITIALIZED;

        c_adapter::register_function(name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t>>(
                    &lambda
            );
        });
    }

    /// @brief register lambda with signature void(jl_value_t*) or jl_value_t*(jl_value_t*)
    template<detail::LambdaType<jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        THROW_IF_UNINITIALIZED;

        c_adapter::register_function(name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*>, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0)
            );
        });
    }

    /// @brief register lambda with signature void(jl_value_t*, jl_value_t*) or jl_value_t*(jl_value_t*, jl_value_t*)
    template<detail::LambdaType<jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1)
            );
        });
    }

    /// @brief register lambda with signature void(3x jl_value_t*) or jl_value_t*(3x jl_value_t*)
    template<detail::LambdaType<jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1),
                    jl_tupleref(tuple, 2)
            );
        });
    }

    /// @brief register lambda with signature void(4x jl_value_t*) or jl_value_t*(4x jl_value_t*)
    template<detail::LambdaType<jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1),
                    jl_tupleref(tuple, 2),
                    jl_tupleref(tuple, 3)
            );
        });
    }

    /// @brief register lambda with signature void(5x jl_value_t*) or jl_value_t*(5x jl_value_t*)
    template<detail::LambdaType<jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1),
                    jl_tupleref(tuple, 2),
                    jl_tupleref(tuple, 3),
                    jl_tupleref(tuple, 4)
            );
        });
    }

    /// @brief register lambda with signature void(std::vector<jl_value_t*>) or jl_value_t*(std::vector<jl_value_t*>)
    template<detail::LambdaType<std::vector<jl_value_t*>> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, [lambda](jl_value_t* tuple) -> jl_value_t* {

            std::vector<jl_value_t*> wrapped;

            for (size_t i = 0; i < jl_tuple_len(tuple); ++i)
                wrapped.push_back(jl_tupleref(tuple, i));

            return detail::invoke_lambda<
                Lambda_t,
                std::invoke_result_t<Lambda_t, std::vector<jl_value_t*>>,
                std::vector<jl_value_t*>>(
                    &lambda, wrapped
            );
        });
    }
}