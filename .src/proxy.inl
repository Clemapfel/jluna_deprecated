// 
// Copyright 2022 Clemens Cords
// Created on 11.01.22 by clem (mail@clemens-cords.com)
//

#include <proxy.hpp>
#include <sstream>
#include <deque>
#include <sstream>

namespace jluna
{
    template<typename State_t>
    Proxy<State_t>::ProxyValue::ProxyValue(jl_value_t* value, std::shared_ptr<ProxyValue>& owner, jl_sym_t* symbol)
        : _is_mutating(symbol != nullptr)
    {
        if (value == nullptr)
            return;

        _owner = owner;
        _value_key = State_t::create_reference(value);
        _value_ref = State_t::get_reference(_value_key);

        if (symbol == nullptr)
        {
            std::stringstream str;
            str << jl_id_marker << _value_key << "[]";
            symbol = jl_symbol(str.str().c_str());
        }

        _symbol_key = State_t::create_reference((jl_value_t*) symbol);
        _symbol_ref = State_t::get_reference(_symbol_key);
    }

    template<typename State_t>
    Proxy<State_t>::ProxyValue::ProxyValue(jl_value_t* value, jl_sym_t* symbol)
        : _owner(nullptr), _is_mutating(symbol != nullptr)
    {
        if (value == nullptr)
            return;

        _value_key = State_t::create_reference(value);
        _value_ref = State_t::get_reference(_value_key);

        if (symbol == nullptr)
        {
            std::stringstream str;
            str << jl_id_marker << _value_key;
            symbol = jl_symbol(str.str().c_str());
        }

        _symbol_key = State_t::create_reference((jl_value_t*) symbol);
        _symbol_ref = State_t::get_reference(_symbol_key);
    }

    template<typename State_t>
    Proxy<State_t>::ProxyValue::~ProxyValue()
    {
        State_t::free_reference(_value_key);
        State_t::free_reference(_symbol_key);
    }

    template<typename State_t>
    jl_value_t * Proxy<State_t>::ProxyValue::value()
    {
        return jl_ref_value(_value_ref);
    }

    template<typename State_t>
    jl_value_t * Proxy<State_t>::ProxyValue::symbol()
    {
        return jl_ref_value(_symbol_ref);
    }

    template<typename State_t>
    size_t Proxy<State_t>::ProxyValue::value_key()
    {
        return _value_key;
    }

    template<typename State_t>
    size_t Proxy<State_t>::ProxyValue::symbol_key()
    {
        return _symbol_key;
    }

    template<typename State_t>
    const jl_value_t * Proxy<State_t>::ProxyValue::value() const
    {
        return jl_ref_value(_value_ref);
    }

    template<typename State_t>
    const jl_value_t * Proxy<State_t>::ProxyValue::symbol() const
    {
        return jl_ref_value(_symbol_ref);
    }

    template<typename State_t>
    jl_value_t * Proxy<State_t>::ProxyValue::get_field(jl_sym_t* symbol)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return Main.jluna");
        static jl_module_t* exception_module = (jl_module_t*) jl_eval_string("return Main.jluna.exception_handler");
        static jl_function_t* safe_call = jl_get_function(exception_module, "safe_call");
        static jl_function_t* dot = jl_get_function(jluna_module, "dot");

        jl_value_t* args[3] = {(jl_value_t*) dot, value(), (jl_value_t*) symbol};
        auto* res = jl_call(safe_call, args, 3);
        forward_last_exception();

        return res;
    }

    /// ####################################################################

    template<typename State_t>
    Proxy<State_t>::Proxy(jl_value_t* value, std::shared_ptr<ProxyValue>& owner, jl_sym_t* symbol)
        : _content(new ProxyValue(value, owner, symbol))
    {}

    template<typename State_t>
    Proxy<State_t>::Proxy(jl_value_t* value, jl_sym_t* symbol)
        : _content(new ProxyValue(value, symbol))
    {}

    template<typename State_t>
    Proxy<State_t> Proxy<State_t>::operator[](const std::string& field)
    {
        jl_sym_t* symbol = jl_symbol(field.c_str());
        return Proxy<State_t>(_content.get()->get_field(symbol), _content, _content->symbol() == nullptr ? nullptr : symbol);
    }

    template<typename State_t>
    template<Unboxable T>
    T Proxy<State_t>::operator[](const std::string& field)
    {
        jl_sym_t* symbol = jl_symbol(field.c_str());
        return unbox<T>(_content.get()->get_field(symbol));
    }

    template<typename State_t>
    auto Proxy<State_t>::operator[](size_t i)
    {
        static jl_function_t* getindex = jl_get_function(jl_base_module, "getindex");
        return Proxy<State_t>(
                State_t::safe_call(getindex, _content->value(), i + 1),
                _content,
                jl_symbol(("[" + std::to_string(i + 1) + "]").c_str())
        );
    }

    template<typename State_t>
    template<Unboxable T>
    T Proxy<State_t>::operator[](size_t i)
    {
        static jl_function_t* getindex = jl_get_function(jl_base_module, "getindex");
        return unbox<T>(State_t::safe_call(getindex, _content->value(), i + 1));
    }

    template<typename State_t>
    Proxy<State_t>::operator jl_value_t*()
    {
        return _content->value();
    }

    template<typename State_t>
    Proxy<State_t>::operator std::string() const
    {
        static jl_function_t* to_string = jl_get_function(jl_base_module, "string");
        return std::string(jl_string_data(jl_call1(to_string, _content->value())));
    }

     template<typename State_t>
     template<Unboxable T, std::enable_if_t<not std::is_same_v<T, std::string>, bool>>
    Proxy<State_t>::operator T() const
    {
        return unbox<T>(_content->value());
    }

    template<typename State_t>
    template<typename T, std::enable_if_t<std::is_base_of_v<Proxy<State_t>, T>, bool>>
    Proxy<State_t>::operator T()
    {
        return T(_content->value(), _content->_owner, (jl_sym_t*) _content->symbol());
    }

    template<typename State_t>
    template<typename T, std::enable_if_t<std::is_base_of_v<Proxy<State_t>, T>, bool>>
    T Proxy<State_t>::as()
    {
        return this->operator T();
    }

    template<typename State_t>
    std::deque<jl_sym_t*> Proxy<State_t>::assemble_name() const
    {
        const ProxyValue* ptr = _content.get();
        std::deque<jl_sym_t*> name;

        while (ptr != nullptr and ptr->symbol() != nullptr)
        {
            name.push_front((jl_sym_t*) ptr->symbol());
            ptr = ptr->_owner.get();
        }

        return name;
    }

    template<typename State_t>
    std::string Proxy<State_t>::get_name() const
    {
        std::deque<jl_sym_t*> name = assemble_name();
        std::stringstream str;

        for (size_t i = 0; i < name.size(); ++i)
        {
            std::string sname = jl_symbol_name(name.at(i));

            if (i != 0 and sname.at(0) != '[')
                str << ".";

            if (sname.at(0) == jl_id_marker)
                str << "<unnamed proxy " << jl_symbol_name(name.at(i)) << ">";
            else
                str << jl_symbol_name(name.at(i));
        }

        return str.str();
    }

    template<typename State_t>
    std::vector<std::string> Proxy<State_t>::get_field_names() const
    {

        auto* svec = jl_field_names((jl_datatype_t*) (jl_isa(_content->value(), (jl_value_t*) jl_datatype_type) ? _content->value() : jl_typeof(_content->value())));
        std::vector<std::string> out;
        for (size_t i = 0; i < jl_svec_len(svec); ++i)
            out.push_back(std::string(jl_symbol_name((jl_sym_t*) jl_svecref(svec, i))));

        return out;
    }

    template<typename State_t>
    auto Proxy<State_t>::get_type() const
    {
        return Proxy<State_t>(jl_typeof(_content->value()), nullptr);
    }

    template<typename State_t>
    template<Boxable... Args_t>
    auto Proxy<State_t>::call(Args_t&&... args)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return jluna");
        static jl_function_t* invoke = jl_get_function(jluna_module, "invoke");

        return Proxy<State>(State_t::call(invoke, _content->value(), std::forward<Args_t>(args)...), nullptr);
    }

    template<typename State_t>
    template<Boxable... Args_t>
    auto Proxy<State_t>::safe_call(Args_t&&... args)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return jluna");
        static jl_function_t* invoke = jl_get_function(jluna_module, "invoke");

        return Proxy<State>(State_t::safe_call(invoke, _content->value(), std::forward<Args_t>(args)...), nullptr);
    }

    template<typename State_t>
    template<Boxable... Args_t>
    auto Proxy<State_t>::operator()(Args_t&&... args)
    {
        return this->safe_call(args...);
    }

    template<typename State_t>
    bool Proxy<State_t>::is_mutating() const
    {
        return _content->_is_mutating;
    }

    template<typename State_t>
    auto & Proxy<State_t>::operator=(jl_value_t* new_value)
    {
        static jl_function_t* safe_call = jl_get_function((jl_module_t*) jl_eval_string("return Main.jluna.exception_handler"), "safe_call");
        static jl_function_t* set_reference = jl_get_function((jl_module_t*) jl_eval_string("return Main.jluna.memory_handler"), "set_reference");

        auto before = jl_gc_is_enabled();
        jl_gc_enable(false);

        //if (_content->_is_mutating)
        {
            _content->_value_ref = jl_call3(safe_call, (jl_value_t*) set_reference, jl_box_uint64(_content->value_key()), new_value);
            forward_last_exception();
        }
        //else
        {
            /*
            static jl_function_t* assemble_assign = jl_get_function((jl_module_t*) jl_eval_string("return Main.jluna.memory_handler"), "assemble_assign");

            auto name = assemble_name();
            std::vector<jl_value_t*> args = {(jl_value_t*) assemble_assign, new_value};

            for (auto* n : name)
                args.push_back((jl_value_t*) n);

            // assign
            jl_value_t* res = jl_call(safe_call, args.data(), args.size());
            forward_last_exception();

            // update value
            State_t::free_reference(_content->value_key());

            _content->_value_key = State_t::create_reference(res);
            _content->_value_ref = State_t::get_reference(_content->value_key());
             */
        }

        jl_gc_enable(before);
        return *this;
    }

    template<typename State_t>
    auto Proxy<State_t>::value() const
    {
        return Proxy<State_t>(_content->value(), nullptr);
    }

    template<typename State_t>
    template<Boxable T>
    auto & Proxy<State_t>::operator=(T value)
    {
        return this->operator=(box<T>(value));
    }

    template<typename State_t>
    void Proxy<State_t>::update()
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return Main.jluna");
        static jl_module_t* exception_module = (jl_module_t*) jl_eval_string("return Main.jluna.exception_handler");
        static jl_function_t* safe_call = jl_get_function(exception_module, "safe_call");
        static jl_function_t* assemble_eval = jl_get_function(jluna_module, "assemble_eval");

        auto name = assemble_name();
        std::vector<jl_value_t*> args = {(jl_value_t*) assemble_eval};

        for (auto* n : name)
            args.push_back((jl_value_t*) n);

        jl_value_t* new_value = jl_call(safe_call, args.data(), args.size());
        _content.reset(new ProxyValue(new_value, _content->_owner, (jl_sym_t*) _content->symbol()));
    }
}