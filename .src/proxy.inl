// 
// Copyright 2022 Clemens Cords
// Created on 11.01.22 by clem (mail@clemens-cords.com)
//

#include <proxy.hpp>
#include <sstream>
#include <deque>

namespace jluna
{

    template<typename State_t>
    Proxy<State_t>::ProxyValue::ProxyValue(jl_value_t* value, std::shared_ptr<ProxyValue>& owner, jl_sym_t* symbol)
    {
        if (value == nullptr)
            return;

        _owner = owner;
        _value = State_t::create_reference(value);
        _symbol = (jl_sym_t*) State_t::create_reference((jl_value_t*) symbol);
    }

    template<typename State_t>
    Proxy<State_t>::ProxyValue::ProxyValue(jl_value_t* value, jl_sym_t* symbol)
        : _owner(nullptr)
    {
        if (value == nullptr)
            return;

        _value = State_t::create_reference(value);
        _symbol = (jl_sym_t*) State_t::create_reference((jl_value_t*) symbol);
    }

    template<typename State_t>
    Proxy<State_t>::ProxyValue::~ProxyValue()
    {
        State_t::free_reference(_value);
        State_t::free_reference((jl_value_t*) _symbol);
    }

    template<typename State_t>
    jl_value_t * Proxy<State_t>::ProxyValue::get_field(jl_sym_t* symbol)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return Main.jluna");
        static jl_module_t* exception_module = (jl_module_t*) jl_eval_string("return Main.jluna.exception_handler");
        static jl_function_t* safe_call = jl_get_function(exception_module, "safe_call");
        static jl_function_t* dot = jl_get_function(jluna_module, "dot");

        jl_value_t* args[3] = {(jl_value_t*) dot, _value, (jl_value_t*) symbol};
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
        return Proxy<State_t>(_content.get()->get_field(symbol), _content, symbol);
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
                State_t::safe_call(getindex, _content.get()->_value, i + 1),
                _content,
                jl_symbol(("[" + std::to_string(i) + "]").c_str())
        );
    }

    template<typename State_t>
    template<Unboxable T>
    T Proxy<State_t>::operator[](size_t i)
    {
        static jl_function_t* getindex = jl_get_function(jl_base_module, "getindex");
        return unbox<T>(State_t::safe_call(getindex, _content.get()->_value(), i + 1));
    }

    template<typename State_t>
    jl_value_t * Proxy<State_t>::value()
    {
        return _content->_value;
    }

    template<typename State_t>
    jl_sym_t * Proxy<State_t>::symbol()
    {
        return _content->_symbol;
    }

    template<typename State_t>
    typename Proxy<State_t>::ProxyValue * Proxy<State_t>::owner()
    {
        return _content->_owner.get();
    }

    template<typename State_t>
    Proxy<State_t>::operator jl_value_t*()
    {
        return _content.get()->_value;
    }

    template<typename State_t>
    Proxy<State_t>::operator std::string() const
    {
        static jl_function_t* to_string = jl_get_function(jl_base_module, "string");
        return std::string(jl_string_data(jl_call1(to_string, _content->_value)));
    }

     template<typename State_t>
    template<Unboxable T>
    Proxy<State_t>::operator T() const
    {
        return unbox<T>(_content->_value);
    }

    template<typename State_t>
    template<typename T, std::enable_if_t<std::is_base_of_v<Proxy<State_t>, T>, bool>>
    Proxy<State_t>::operator T()
    {
        return T(value(), _content->_owner, symbol());
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

        while (ptr != nullptr and ptr->_symbol != nullptr)
        {
            name.push_front(ptr->_symbol);
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

            str << jl_symbol_name(name.at(i));
        }

        return str.str();
    }

    template<typename State_t>
    std::vector<std::string> Proxy<State_t>::get_field_names() const
    {
        auto* svec = jl_field_names((jl_datatype_t*) jl_typeof(_content->_value));
        std::vector<std::string> out;
        for (size_t i = 0; i < jl_svec_len(svec); ++i)
            out.push_back(std::string(jl_symbol_name((jl_sym_t*) jl_svecref(svec, i))));

        return out;
    }

    template<typename State_t>
    template<Boxable... Args_t>
    auto Proxy<State_t>::call(Args_t&&... args)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return jluna");
        static jl_function_t* invoke = jl_get_function(jluna_module, "invoke");

        return Proxy<State>(State_t::call(invoke, value(), std::forward<Args_t>(args)...), nullptr);
    }

    template<typename State_t>
    template<Boxable... Args_t>
    auto Proxy<State_t>::safe_call(Args_t&&... args)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return jluna");
        static jl_function_t* invoke = jl_get_function(jluna_module, "invoke");

        return Proxy<State>(State_t::safe_call(invoke, value(), std::forward<Args_t>(args)...), nullptr);
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
        return _is_mutating;
    }

    template<typename State_t>
    void Proxy<State_t>::set_mutating(bool b)
    {
        if (symbol() == nullptr or std::string(jl_symbol_name_((jl_sym_t*) symbol())) == "")
            throw UnnamedVariableException(value());

        else if (jl_isa(owner()->_value, (jl_value_t*) jl_module_type))
        {
            if (jl_is_const((jl_module_t*) owner()->_value, (jl_sym_t*) symbol()))
                throw ImmutableVariableException(value());
            goto skip; //because modules are structtypes, skip next else if on success
        }
        else if (jl_is_structtype(jl_typeof(owner()->_value)) and not jl_is_mutable_datatype(jl_typeof(owner()->_value)))
            throw ImmutableVariableException(owner()->_value);

        skip:
        _is_mutating = b;
    }

    template<typename State_t>
    bool Proxy<State_t>::can_mutate() const
    {
        if ((symbol() == nullptr) or
            (std::string(jl_symbol_name_(symbol())) == "") or
            (jl_isa(owner()->_value, (jl_value_t*) jl_module_type) and jl_is_const((jl_module_t*) owner()->_value, symbol())) or
            (jl_is_structtype(jl_typeof(owner()->_value)) and jl_is_mutable_datatype(jl_typeof(owner()->_value))))
            return false;
        else
            return true;
    }

    template<typename State_t>
    auto & Proxy<State_t>::operator=(jl_value_t* v)
    {
        auto before = jl_gc_is_enabled();
        jl_gc_enable(false);

        if (not _is_mutating)
        {
            State_t::free_reference(value());
            _content->_value = v;
            State_t::create_reference(value());
        }
        else
        {
            static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return Main.jluna");
            static jl_module_t* exception_module = (jl_module_t*) jl_eval_string("return Main.jluna.exception_handler");
            static jl_function_t* safe_call = jl_get_function(exception_module, "safe_call");
            static jl_function_t* assemble_assign = jl_get_function(jluna_module, "assemble_assign");

            auto name = assemble_name();
            std::vector<jl_value_t*> args = {(jl_value_t*) assemble_assign, v};

            for (auto* n : name)
                args.push_back((jl_value_t*) n);

            State_t::free_reference(value());
            _content->_value = jl_call(safe_call, args.data(), args.size());
            forward_last_exception();
            State_t::create_reference(value());
        }

        jl_gc_enable(before);
        return *this;
    }

    template<typename State_t>
    template<Boxable T>
    auto & Proxy<State_t>::operator=(T value)
    {
        return this->operator=(box<T>(value));
    }
}