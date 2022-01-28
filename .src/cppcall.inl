// 
// Copyright 2022 Clemens Cords
// Created on 20.01.22 by clem (mail@clemens-cords.com)
//

#include <cppcall.hpp>
#include <.c_adapter/c_adapter.hpp>
#include <typedefs.hpp>
#include <exceptions.hpp>

namespace jluna
{
    namespace detail
    {
        template<typename Lambda_t, typename Return_t, typename... Args_t, std::enable_if_t<std::is_same_v<Return_t, void>, Bool> = true>
        jl_value_t* detail::invoke_lambda(const Lambda_t* func, Args_t... args)
        {
            //try
            {
                (*func)(args...);
                return jl_nothing;
            }

            /*catch (JuliaException& e)
            {
                static auto print = [](auto arg, size_t i)
                {
                    std::cerr << jl_verbose_typeof_str(arg);
                    if (i != sizeof...(Args_t))
                        std::cerr << ", ";
                };

                std::cerr << "[C++][EXCEPTION] in cppcall with arguments of type: {";
                {
                    size_t i = 1;
                    (print(args, i++), ...);
                }
                std::cerr << "}" << std::endl;
                throw e;
            }*/
        }

        template<typename Lambda_t, typename Return_t, typename... Args_t, std::enable_if_t<std::is_same_v<Return_t, jl_value_t*>, Bool> = true>
        jl_value_t* detail::invoke_lambda(const Lambda_t* func, Args_t... args)
        {
            //try
            {
                jl_value_t* res = (*func)(args...);
                return res;
            }

            /*catch (JuliaException& e)
            {
                static auto print = [](auto arg, size_t i)
                {
                    std::cerr << jl_verbose_typeof_str(arg);
                    if (i != sizeof...(Args_t))
                        std::cerr << ", ";
                };

                std::cerr << "[C++][EXCEPTION] in cppcall with arguments of type: {";
                {
                    size_t i = 1;
                    (print(args, i++), ...);
                }
                std::cerr << "}" << std::endl;
                throw e;
            }*/
        }
    }

    template<detail::LambdaType<> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        THROW_IF_UNINITIALIZED;

        c_adapter::register_function(name, 0, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t>>(
                    &lambda
            );
        });
    }

    template<detail::LambdaType<jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        THROW_IF_UNINITIALIZED;

        c_adapter::register_function(name, 1, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*>, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0)
            );
        });
    }

    template<detail::LambdaType<jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, 2, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1)
            );
        });
    }

    template<detail::LambdaType<jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, 3, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1),
                    jl_tupleref(tuple, 2)
            );
        });
    }

    template<detail::LambdaType<jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, 4, [lambda](jl_value_t* tuple) -> jl_value_t* {

            return detail::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1),
                    jl_tupleref(tuple, 2),
                    jl_tupleref(tuple, 3)
            );
        });
    }

    template<detail::LambdaType<jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, 5, [lambda](jl_value_t* tuple) -> jl_value_t* {

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

    template<detail::LambdaType<std::vector<jl_value_t*>> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        c_adapter::register_function(name, 1, [lambda](jl_value_t* tuple) -> jl_value_t* {

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

    namespace detail
    {
        template<detail::LambdaType<jl_value_t*> Lambda_t>
        jl_value_t* box(Lambda_t && lambda)
        {
            std::string id = "#" + std::to_string(++detail::_internal_function_id_name);
            register_function(id, std::forward<Lambda_t&&>(lambda));

            static jl_function_t* new_unnamed_function = get_function("_cppcall", "new_unnamed_function");
            return jl_call1(new_unnamed_function, (jl_value_t*) jl_symbol(id.c_str()));
        }
    }
}