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
        : _value(value), _field_to_index(), _owner(nullptr), _field_i(-1), _symbol(nullptr)
    {
        THROW_IF_UNINITIALIZED;

        if (_value == nullptr)
            return;

        State_t::create_reference(_owner);
        State_t::create_reference(_value);
        State_t::create_reference((jl_value_t*) _symbol);
        _value = value;
        setup_field_to_index();
    }

    template<typename State_t>
    Proxy<State_t>::Proxy(jl_value_t* value, jl_value_t* owner, size_t field_i)
        : _value(value), _field_to_index(), _owner(owner), _field_i(field_i), _symbol(nullptr)
    {
        THROW_IF_UNINITIALIZED;

        if (_value == nullptr)
            return;

        State_t::create_reference(_owner);
        State_t::create_reference(_value);
        State_t::create_reference((jl_value_t*) _symbol);

        setup_field_to_index();
    }

    template<typename State_t>
    Proxy<State_t>::Proxy(jl_value_t* value, jl_value_t* owner, jl_sym_t* symbol)
        : _value(value), _field_to_index(), _owner(owner), _field_i(-1), _symbol(symbol)
    {
        THROW_IF_UNINITIALIZED;

        if (_value == nullptr)
            return;

        State_t::create_reference(_owner);
        State_t::create_reference(_value);
        State_t::create_reference((jl_value_t*) _symbol);
        setup_field_to_index();
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
        if (jl_isa(_value, (jl_value_t*) jl_module_type))
        {
            static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return jluna");
            static jl_function_t* dot = jl_get_function(jl_main_module, "dot");
            static jl_function_t* isdefined = jl_get_function(jl_base_module, "isdefined");
            static jl_function_t* tostring = jl_get_function(jl_base_module, "string");
            auto* as_symbol = jl_symbol(field_name.c_str());

            if (not jl_unbox_bool(jl_call2(isdefined, _value, (jl_value_t*) as_symbol)))
            {
                std::stringstream str;
                str << "member \"" << field_name << "\" is not defined in module " << jl_string_data(jl_call1(tostring, _value)) << std::endl;
                throw std::out_of_range(str.str().c_str());
            }

            return Proxy<State_t>(jl_call2(dot, _value, (jl_value_t*) jl_symbol(field_name.c_str())), _value, as_symbol);
        }
        else if (jl_is_structtype(jl_typeof(_value)))
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
        else
        {
            std::stringstream str;
            str << "value of type " << jl_typeof_str(_value) << " has no field or member \"" << field_name << "\"" << std::endl;
            throw std::out_of_range(str.str().c_str());
            return Proxy<State_t>(nullptr);
        }
    }

    template<typename State_t>
    template<typename T>
    T Proxy<State_t>::get_field(const std::string& field_name) const
    {
        if (jl_isa(_value, (jl_value_t*) jl_module_type))
        {
            static jl_module_t* jluna_module = (jl_module_t*) jl_eval_string("return jluna");
            static jl_function_t* dot = jl_get_function(jluna_module, "dot");
            static jl_function_t* isdefined = jl_get_function(jl_base_module, "isdefined");
            static jl_function_t* tostring = jl_get_function(jl_base_module, "string");

            if (not jl_unbox_bool(jl_call2(isdefined, _value, (jl_value_t*) jl_symbol(field_name.c_str()))))
            {
                std::stringstream str;
                str << "member \"" << field_name << "\" is not defined in module " << jl_string_data(jl_call1(tostring, _value)) << std::endl;
                throw std::out_of_range(str.str().c_str());
            }

            return unbox<T>(jl_call2(dot, _value, (jl_value_t*) jl_symbol(field_name.c_str())));
        }
        else if (jl_is_structtype(jl_typeof(_value)))
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
        else
        {
            std::stringstream str;
            str << "value of type " << jl_typeof_str(_value) << " has no field or member \"" << field_name << "\"" << std::endl;
            throw std::out_of_range(str.str().c_str());
            return T();
        }
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

        if (_owner != nullptr and _symbol != nullptr) // module
        {
            static jl_function_t* isconst = jl_get_function(jl_base_module, "isconst");

            if (jl_unbox_bool(jl_call2(isconst, _owner, (jl_value_t*) _symbol)))
                throw ImmutableVariableException(_value);

            static jl_function_t* assign = jl_get_function(jl_main_module, "assign");
            State::free_reference(_value);

            jl_value_t* arr[3] = {_owner, (jl_value_t*) _symbol, box<T>(value)};
            _value = jl_call(assign, &arr[0], 3);
            forward_last_exception();

            State::create_reference(_value);
        }
        else if (_owner != nullptr and _field_i >= 0) // struct field
        {
            static jl_function_t* ismutable = jl_get_function(jl_base_module, "ismutable");

            if (not jl_unbox_bool(jl_call1(ismutable, jl_get_nth_field(_owner, size_t(_field_i)))))
                throw ImmutableVariableException(_value);

            State::free_reference(_value);
            jl_set_nth_field(_owner, size_t(_field_i), box<T>(value));
            _value = jl_get_nth_field(_owner, size_t(_field_i));
            State::create_reference(_value);
        }
        else
        {
            State::free_reference(_value);
            _value = box<T>(value);
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

        if (_owner != nullptr and _symbol != nullptr) // module
        {
            static jl_function_t* isconst = jl_get_function(jl_base_module, "isconst");

            if (jl_unbox_bool(jl_call2(isconst, _owner, (jl_value_t*) _symbol)))
                throw ImmutableVariableException(_value);

            static jl_function_t* assign = jl_get_function(jl_main_module, "assign");
            State::free_reference(_value);
            
            jl_value_t* arr[3] = {_owner, (jl_value_t*) _symbol, value};
            _value = jl_call(assign, &arr[0], 3);
            forward_last_exception();
            
            State::create_reference(_value);
        }
        else if (_owner != nullptr and _field_i >= 0) // struct field
        {
            static jl_function_t* ismutable = jl_get_function(jl_base_module, "ismutable");

            if (not jl_unbox_bool(jl_call1(ismutable, jl_get_nth_field(_owner, size_t(_field_i)))))
                throw ImmutableVariableException(_value);

            State::free_reference(_value);
            jl_set_nth_field(_owner, size_t(_field_i), value);
            _value = jl_get_nth_field(_owner, size_t(_field_i));
            State::create_reference(_value);
        }
        else
        {
            State::free_reference(_value);
            _value = value;
            State::create_reference(_value);
        }

        jl_gc_enable(before);
        return *this;
    }

    template<typename State_t>
    bool Proxy<State_t>::is_mutable() const
    {
         if (_owner != nullptr and _symbol != nullptr) // module
        {
            static jl_function_t* isconst = jl_get_function(jl_base_module, "isconst");
            if (jl_unbox_bool(jl_call2(isconst, _owner, (jl_value_t*) _symbol)))
                return false;
            else
                return true;
        }
        else if (_owner != nullptr and _field_i >= 0) // struct field
        {
            static jl_function_t* ismutable = jl_get_function(jl_base_module, "ismutable");

            if (not jl_unbox_bool(jl_call1(ismutable, jl_get_nth_field(_owner, size_t(_field_i)))))
                return false;
            else
                return true;
        }
        else
        {
            return true;
        }
    }

    template<typename State_t>
    bool Proxy<State_t>::is_struct() const
    {
        return jl_is_structtype(jl_typeof(_value));
    }

    template<typename State_t>
    const std::unordered_map<std::string, size_t> & Proxy<State_t>::get_fieldnames() const
    {
        return _field_to_index;
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