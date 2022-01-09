// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#include <proxy.hpp>
#include <sstream>
#include <box_any.hpp>
#include <function_proxy.hpp>

namespace jluna
{
    template<typename State_t>
    jl_value_t* box(Proxy<State_t> value)
    {
        return value.operator _jl_value_t *();
    }

    template<typename State_t>
    jl_value_t* box(Proxy<State_t>& value)
    {
        return value.operator _jl_value_t *();
    }

    template<typename State_t>
    void Proxy<State_t>::setup_fields()
    {
        auto* type = (jl_datatype_t*) jl_typeof(_value);
        auto* field_names = jl_field_names(type);

        static jl_function_t* fieldcount = jl_get_function(jl_base_module, "fieldcount");

        for (size_t i = 0; i < jl_unbox_int64(jl_call1(fieldcount, (jl_value_t*) type)); ++i)
            _fields.insert(jl_symbol_name((jl_sym_t*) jl_svecref(field_names, i)));
    }

    template<typename State_t>
    Proxy<State_t>::Proxy(jl_value_t* value, jl_value_t* owner, jl_sym_t* symbol)
        : _value(value), _fields(), _owner(owner), _symbol(symbol)
    {
        if (_value == nullptr)
            return;

        THROW_IF_UNINITIALIZED;

        State_t::create_reference(_owner);
        State_t::create_reference(_value);
        State_t::create_reference((jl_value_t*) _symbol);
        setup_fields();
    }

    template<typename State_t>
    Proxy<State_t>::operator jl_value_t*()
    {
        return _value;
    }

    template<typename State_t>
    Proxy<State_t>::operator std::string() const
    {
        static jl_function_t* to_string = jl_get_function(jl_base_module, "string");
        return std::string(jl_string_data(jl_call1(to_string, _value)));
    }

    template<typename State_t>
    Proxy<State_t>::~Proxy()
    {
        State_t::free_reference(_owner);
        State_t::free_reference(_value);
        State_t::free_reference((jl_value_t*) _symbol);
        _value = nullptr;
    }

    template<typename State_t>
    auto Proxy<State_t>::get_field(const std::string& field_name)
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return Main.jluna");
        static jl_module_t* exception_module = (jl_module_t*) jl_eval_string("return Main.jluna.exception_handler");
        static jl_function_t* safe_call = jl_get_function(exception_module, "safe_call");
        static jl_function_t* dot = jl_get_function(jluna_module, "dot");

        auto symbol = jl_symbol(field_name.c_str());

        jl_value_t* args[3] = {(jl_value_t*) dot, _value, (jl_value_t*) symbol};
        auto* res = jl_call(safe_call, args, 3);
        forward_last_exception();

        return Proxy<State>(res, _value, symbol);
    }

    template<typename State_t>
    template<typename T>
    T Proxy<State_t>::get_field(const std::string& field_name) const
    {
        static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return Main.jluna");
        static jl_module_t* exception_module = (jl_module_t*) jl_eval_string("return Main.jluna.exception_handler");
        static jl_function_t* safe_call = jl_get_function(exception_module, "safe_call");
        static jl_function_t* dot = jl_get_function(jluna_module, "dot");

        auto symbol = jl_symbol(field_name.c_str());

        jl_value_t* args[4] = {(jl_value_t*) dot, _value, (jl_value_t*) symbol};
        auto* res = jl_call(safe_call, args, 4);
        forward_last_exception();

        return unbox<T>(res);
    }

    template<typename State_t>
    template<Unboxable T>
    Proxy<State_t>::operator T() const
    {
        return unbox<T>(_value);
    }

    template<typename State_t>
    template<typename T, std::enable_if_t<std::is_base_of_v<Proxy<State_t>, T>, bool>>
    Proxy<State_t>::operator T()
    {
        return T(this->_value);
    }

    template<typename State_t>
    Proxy<State_t>::Proxy(Proxy&& other) noexcept
        : _value(other._value)
    {
        other._value = nullptr;
    }

    template<typename State_t>
    Proxy<State_t>::Proxy(const Proxy& other)
        : _value(other._value)
    {
        State_t::create_reference(_value);
    }

    template<typename State_t>
    bool Proxy<State_t>::is_mutating() const
    {
        return _is_mutating;
    }

    template<typename State_t>
    void Proxy<State_t>::set_mutating(bool b)
    {
        if (std::string(jl_symbol_name_(_symbol)) == "")
            throw UnnamedVariableException(_value);

        else if (jl_isa(_owner, (jl_value_t*) jl_module_type))
        {
            if (jl_is_const((jl_module_t*) _owner, _symbol))
                throw ImmutableVariableException(_value);
            goto skip;
        }
        else if (jl_is_structtype(jl_typeof(_owner)) and jl_is_mutable_datatype(jl_typeof(_owner)))
            throw ImmutableVariableException(_owner);

        skip:
        _is_mutating = b;
    }

    template<typename State_t>
    bool Proxy<State_t>::can_mutate() const
    {
        if ((std::string(jl_symbol_name_(_symbol)) == "") or
            (jl_isa(_owner, (jl_value_t*) jl_module_type) and jl_is_const((jl_module_t*) _owner, _symbol)) or
            (jl_is_structtype(jl_typeof(_owner)) and jl_is_mutable_datatype(jl_typeof(_owner))))
            return false;
        else
            return true;
    }

    template<typename State_t>
    void Proxy<State_t>::assign_name(const std::string& name)
    {
        _symbol = jl_symbol(name.c_str());
    }

    template<typename State_t>
    Proxy<State_t>& Proxy<State_t>::operator=(Proxy&& other) noexcept
    {
        if (&other == this)
            return *this;

        _value = other._value;
        other._value = nullptr;

        return *this;
    }

    template<typename State_t>
    Proxy<State_t>& Proxy<State_t>::operator=(const Proxy& other)
    {
        if (&other == this)
            return *this;

        _value = other._value;
        State_t::create_reference(_value);

        return *this;
    }

    template<typename State_t>
    template<Boxable T>
    auto & Proxy<State_t>::operator=(T value)
    {
        return this->operator=(box<T>(value));
    }

    template<typename State_t>
    auto & Proxy<State_t>::operator=(jl_value_t* value)
    {
        auto before = jl_gc_is_enabled();
        jl_gc_enable(false);

        if (not _is_mutating)
        {
            State::free_reference(_value);
            _value = value;
            State::create_reference(value);
        }
        else
        {
            static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return Main.jluna");
            static jl_module_t* exception_module = (jl_module_t*) jl_eval_string("return Main.jluna.exception_handler");
            static jl_function_t* safe_call = jl_get_function(exception_module, "safe_call");
            static jl_function_t* assign = jl_get_function(jluna_module, "assign");

            jl_value_t* args[4] = {(jl_value_t*) assign, _owner, (jl_value_t*) _symbol, value};

            State::free_reference(_value);
            _value = jl_call(safe_call, args, 4);
            forward_last_exception();
            State::create_reference(_value);
        }

        jl_gc_enable(before);
        return *this;
    }

    template<typename State_t>
    bool Proxy<State_t>::is_struct() const
    {
        return jl_is_structtype(jl_typeof(_value));
    }

    template<typename State_t>
    const std::set<std::string> & Proxy<State_t>::get_fieldnames() const
    {
        return _fields;
    }

    template<typename State_t>
    bool Proxy<State_t>::operator==(const Proxy<State_t>& other) const
    {
        return this->_value == other._value;
    }

    template<typename State_t>
    bool Proxy<State_t>::operator!=(const Proxy<State_t>& other) const
    {
        return this->_value != other._value;
    }

    template<typename State_t>
    auto Proxy<State_t>::operator[](const std::string& field_name)
    {
        return get_field(field_name);
    }

    template<typename State_t>
    template<typename T>
    T Proxy<State_t>::operator[](const std::string& field_name) const
    {
        return get_field<T>(field_name);
    }
}