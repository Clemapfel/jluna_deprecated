// 
// Copyright 2022 Clemens Cords
// Created on 03.01.22 by clem (mail@clemens-cords.com)
//

#include <type_proxy.hpp>

namespace jluna
{
    /*
    /// @brief proxy for julia-side singleton instances of Base.Type{T}
    class Type : public Proxy<State>
    {
        public:
            /// @brief ctor from already existing type
            Type(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>&, jl_sym_t*);

            /// @brief implicitly decay to julia c-type
            operator jl_datatype_t*();

            /// @brief cast to string
            explicit operator std::string() const;

            /// @brief compare
            /// @param other
            /// @returns true if in julia (==)(*this, other) would return true
            bool operator==(const Type&) const;

            /// @brief compare
            /// @param other
            /// @returns true if in julia (!=)(*this, other) would return true
            bool operator!=(const Type&) const;

            // not yet implemented

            /// @brief is mutable
            /// @returns true if Base.ismutable would return true
            bool is_mutable_type() const;

            /// @brief is structtype
            /// @returns true if Base.issstructype would return true
            bool is_structtype() const;

            /// @brief <: operator
            /// @param type
            /// @returns true if type <: this
            bool is_subtype(const Type& type) const;


        private:
            using Proxy<State>::_content;
    };

    /// @brief get type of proxy as jluna::Type
    /// @param proxy
    /// @returns Type
    extern Type get_typeof(Proxy<State>&);

    inline jl_value_t* box(Type type)
    {
        return (jl_value_t*) type.operator _jl_datatype_t *();
    }

    Type::Type(jl_value_t* value, std::shared_ptr<typename Proxy<State>::ProxyValue>& owner, jl_sym_t* symbol)
        : Proxy<State>(value, owner, symbol)
    {
        THROW_IF_UNINITIALIZED;
        assert(jl_isa(value, (jl_value_t*) jl_type_type) || jl_isa(value, (jl_value_t*) jl_type_type) && "value is not a type or datatype");
    }

    Type::operator jl_datatype_t*()
    {
        return (jl_datatype_t*) _content->value();
    }

    Type::operator std::string() const
    {
        static jl_function_t* to_string = jl_get_function(jl_base_module, "string");
        return std::string(jl_string_data(jl_call1(to_string, _content->value())));
    }

    bool Type::operator==(const Type& other) const
    {
        static jl_function_t* equals = jl_get_function(jl_base_module, "==");
        return jl_unbox_bool(jl_call2(equals, this->_content->value(), other._content->value()));
    }

    bool Type::operator!=(const Type& other) const
    {
        static jl_function_t* not_equals = jl_get_function(jl_base_module, "!=");
        return jl_unbox_bool(jl_call2(not_equals, this->_content->value(), other._content->value()));
    }
     */
}