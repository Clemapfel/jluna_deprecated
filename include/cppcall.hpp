// 
// Copyright 2022 Clemens Cords
// Created on 20.01.22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <state.hpp>

namespace jluna::cppcall
{
    static inline std::unordered_map<size_t, std::unique_ptr<std::function<jl_value_t*(jl_value_t*)>>> _functions = []() -> decltype(_functions) {

        std::unordered_map<size_t, std::unique_ptr<std::function<jl_value_t*(jl_value_t*)>>> to_return;

       to_return.insert({1443994487737173028, std::make_unique<std::function<jl_value_t*(jl_value_t*)>>([](jl_value_t*) -> jl_value_t* {
            std::cout << "works" << std::endl;
            return nullptr;
        })});

       return to_return;
    }();

    jl_value_t* jl_tupleref(jl_value_t* tuple, size_t n)
    {
        static jl_function_t* get = jl_get_function(jl_base_module, "get");
        return jl_call3(get, tuple, jl_box_uint64(n + 1), jl_undef_initializer());
    }

    size_t jl_tuple_len(jl_value_t* tuple)
    {
        static jl_function_t* length = jl_get_function(jl_base_module, "length");
        return jl_unbox_int64(jl_call1(length, tuple));
    }

    size_t jl_hash(const std::string& str)
    {
        static jl_function_t* hash = jl_get_function(jl_base_module, "hash");

        return jl_unbox_uint64(jl_call1(hash, (jl_value_t*) jl_symbol(str.c_str())));
    }

    template<typename T, typename... Args_t>
    concept LambdaType = requires(T lambda)
    {
        std::is_invocable<T, Args_t...>::value;
        typename std::invoke_result<T, Args_t...>::type;
    };

    template<typename Lambda_t, typename Return_t, typename... Args_t, std::enable_if_t<std::is_same_v<Return_t, void>, Bool> = true>
    jl_value_t* invoke_lambda(const Lambda_t* func, Args_t... args)
    {
        (*func)(args...);
        return jl_nothing;
    }

    template<typename Lambda_t, typename Return_t, typename... Args_t, std::enable_if_t<std::is_same_v<Return_t, jl_value_t*>, Bool> = true>
    jl_value_t* invoke_lambda(const Lambda_t* func, Args_t... args)
    {
        jl_value_t* res = (*func)(args...);
        return res;
    }

    /// @brief call from c++ side
    template<typename... Ts>
    jl_value_t* call_from_cpp(const std::string& name, Ts... in)
    {
        static jl_function_t* tuple = jl_get_function(jl_base_module, "tuple");

        std::vector<jl_value_t*> args;
        args.reserve(sizeof...(in));
        (args.push_back(in), ...);

        return _functions.at(jl_hash(name)).get()->operator()(jl_call(tuple, args.data(), args.size()));
    }

    /// @brief call from c++ side
    jl_value_t* call_from_julia(size_t id)
    {
        THROW_IF_UNINITIALIZED;

        jl_value_t* tuple = jl_eval_string("return jluna.cpp_call._state._arguments");
        jl_value_t* state_id = jl_eval_string("return jluna.cpp_call._state._id");

        auto* res = _functions.at(id).get()->operator()(tuple);

        static jl_function_t* set_result = jl_eval_string("return jluna.cpp_call.set_result");
        jl_call1(set_result, res);

        return jl_nothing;
    }

    /// @brief check if lambda has specified return and value type
    /// @tparam Lambda_t: lambda type
    /// @tparam Return_t: return type
    /// @tparam Args_t...: argument types

    /// @brief register lambda with signature void() or jl_value_t*()
    template<LambdaType<> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        THROW_IF_UNINITIALIZED;

        _functions.insert({jl_hash(name), std::make_unique<std::function<jl_value_t*(jl_value_t*)>>([lambda](jl_value_t* tuple) -> jl_value_t* {

            return cppcall::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t>>(
                    &lambda
            );
        })});
    }

    /// @brief register lambda with signature void(jl_value_t*) or jl_value_t*(jl_value_t*)
    template<LambdaType<jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        THROW_IF_UNINITIALIZED;

        _functions.insert({jl_hash(name), std::make_unique<std::function<jl_value_t*(jl_value_t*)>>([lambda](jl_value_t* tuple) -> jl_value_t* {

            return cppcall::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*>, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0)
            );
        })});
    }

    /// @brief register lambda with signature void(jl_value_t*, jl_value_t*) or jl_value_t*(jl_value_t*, jl_value_t*)
    template<LambdaType<jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        _functions.insert({jl_hash(name), std::make_unique<std::function<jl_value_t*(jl_value_t*)>>([lambda](jl_value_t* tuple) -> jl_value_t* {

            return cppcall::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1)
            );
        })});
    }

    /// @brief register lambda with signature void(3x jl_value_t*) or jl_value_t*(3x jl_value_t*)
    template<LambdaType<jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        _functions.insert({jl_hash(name), std::make_unique<std::function<jl_value_t*(jl_value_t*)>>([lambda](jl_value_t* tuple) -> jl_value_t* {

            return cppcall::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1),
                    jl_tupleref(tuple, 2)
            );
        })});
    }

    /// @brief register lambda with signature void(4x jl_value_t*) or jl_value_t*(4x jl_value_t*)
    template<LambdaType<jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        _functions.insert({jl_hash(name), std::make_unique<std::function<jl_value_t*(jl_value_t*)>>([lambda](jl_value_t* tuple) -> jl_value_t* {

            return cppcall::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1),
                    jl_tupleref(tuple, 2),
                    jl_tupleref(tuple, 3)
            );
        })});
    }

    /// @brief register lambda with signature void(5x jl_value_t*) or jl_value_t*(5x jl_value_t*)
    template<LambdaType<jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        _functions.insert({jl_hash(name), std::make_unique<std::function<jl_value_t*(jl_value_t*)>>([lambda](jl_value_t* tuple) -> jl_value_t* {

            return cppcall::invoke_lambda<Lambda_t, std::invoke_result_t<Lambda_t, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*, jl_value_t*>(
                    &lambda,
                    jl_tupleref(tuple, 0),
                    jl_tupleref(tuple, 1),
                    jl_tupleref(tuple, 2),
                    jl_tupleref(tuple, 3),
                    jl_tupleref(tuple, 4)
            );
        })});
    }

    /// @brief register lambda with signature void(std::vector<jl_value_t*>) or jl_value_t*(std::vector<jl_value_t*>)
    template<LambdaType<std::vector<jl_value_t*>> Lambda_t>
    void register_function(const std::string& name, Lambda_t&& lambda)
    {
        _functions.insert({jl_hash(name), std::make_unique<std::function<jl_value_t*(jl_value_t*)>>([lambda](jl_value_t* tuple) -> jl_value_t* {

            std::vector<jl_value_t*> wrapped;

            for (size_t i = 0; i < jl_tuple_len(tuple); ++i)
                wrapped.push_back(jl_tupleref(tuple, i));

            return cppcall::invoke_lambda<
                Lambda_t,
                std::invoke_result_t<Lambda_t, std::vector<jl_value_t*>>,
                std::vector<jl_value_t*>>(
                    &lambda, wrapped
            );
        })});
    }


}