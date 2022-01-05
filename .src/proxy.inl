// 
// Copyright 2021 Clemens Cords
// Created on 18.12.21 by clem (mail@clemens-cords.com)
//

#include <proxy.hpp>
#include <sstream>
#include <.src/box_any.hpp>
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
    void Proxy<State_t>::setup_field_to_index()
    {
        auto* type = (jl_datatype_t*) jl_typeof(_value);
        auto* field_names = jl_field_names(type);

        static jl_function_t* fieldcount = jl_get_function(jl_base_module, "fieldcount");

        for (size_t i = 0; i < jl_unbox_int64(jl_call1(fieldcount, (jl_value_t*) type)); ++i)
            _field_to_index.emplace(jl_symbol_name((jl_sym_t*) jl_svecref(field_names, i)), i);
    }

    template<typename State_t>
    Proxy<State_t>::Proxy(jl_value_t* value)
        : _value(value), _field_to_index(), _owner(nullptr), _field_i(-1)
    {
        if (_value == nullptr)
            return;

        State_t::create_reference(value);
        _value = value;
        setup_field_to_index();
    }

    template<typename State_t>
    Proxy<State_t>::Proxy(jl_value_t* value, jl_value_t* owner, size_t field_i)
        : _value(value), _field_to_index(), _owner(owner), _field_i(field_i)
    {
        if (_value == nullptr)
            return;

        State_t::create_reference(value);
        setup_field_to_index();
    }

    template<typename State_t>
    Proxy<State_t>::operator jl_value_t*()
    {
        return _value;
    }

    template<typename State_t>
    Proxy<State_t>::operator std::string()
    {
        static jl_function_t* to_string = jl_get_function(jl_base_module, "string");
        return std::string(jl_string_data(jl_call1(to_string, _value)));
    }

    template<typename State_t>
    Proxy<State_t>::~Proxy()
    {
        State_t::free_reference(_value);
        _value = nullptr;
    }

    template<typename State_t>
    auto Proxy<State_t>::get_field(const std::string& field_name)
    {
        auto it = _field_to_index.find(field_name);

        if (it == _field_to_index.end())
        {
            std::stringstream str;
            str << "field \"" << field_name << "\" does not exist for element of type " << jl_typeof_str(_value) << std::endl;
            throw std::out_of_range(str.str().c_str());
        }

        return Proxy<State_t>(jl_get_nth_field(_value, it->second), _value, it->second);
    }

    template<typename State_t>
    template<typename T>
    T Proxy<State_t>::get_field(const std::string& field_name) const
    {
        auto it = _field_to_index.find(field_name);

        if (it == _field_to_index.end())
        {
            std::stringstream str;
            str << "field \"" << field_name << "\" does not exist for element of type " << jl_typeof_str(_value) << std::endl;
            throw std::out_of_range(str.str().c_str());
        }

        return unbox<T>(jl_get_nth_field(_value, it->second));
    }

    template<typename State_t>
    template<Unboxable T>
    Proxy<State_t>::operator T()
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
        auto before = jl_gc_is_enabled();
        jl_gc_enable(false);

        if (_field_i == -1)
        {
            State::free_reference(_value);
            _value = box<T>(value);
            State::create_reference(_value);
        }
        else
        {
            State::free_reference(_value);
            jl_set_nth_field(_owner, size_t(_field_i), box<T>(value));
            _value = jl_get_nth_field(_owner, size_t(_field_i));
            State::create_reference(_value);
        }

        jl_gc_enable(before);

        return *this;
    }

    template<typename State_t>
    auto & Proxy<State_t>::operator=(jl_value_t* value)
    {
        auto before = jl_gc_is_enabled();
        jl_gc_enable(false);

        if (_field_i == -1)
        {
            State::free_reference(_value);
            _value = value;
            State::create_reference(_value);
        }
        else
        {
            State::free_reference(_value);
            jl_set_nth_field(_owner, size_t(_field_i), value);
            _value = jl_get_nth_field(_owner, size_t(_field_i));
            State::create_reference(_value);
        }

        jl_gc_enable(before);

        return *this;
    }

    template<typename State_t>
    bool Proxy<State_t>::is_const() const
    {
        return false;
    }
}