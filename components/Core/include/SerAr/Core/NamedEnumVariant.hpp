#pragma once
#include <type_traits>
#include <utility>
#include <string>
#include <fmt/core.h>

#include <MyCEL/basics/enumhelpers.h>
#include <SerAr/Core/NamedValue.h>
#include <SerAr/Core/ArchiveHelper.h>

namespace SerAr {

    namespace concepts {
        template<typename T>
        concept EnumVariant = requires(T&& t) {
            typename std::remove_cvref_t<T>::enum_type;
            typename std::remove_cvref_t<T>::enum_variant_type;
            t.value;
            t.variant;
        };
    }

    template<concepts::EnumVariant T>
    struct NamedEnumVariant {
        using type = T;
        //If T is an Array: keep the type
        //If T is a l value: store a reference (T&)
        //If T is a r value: copy the value (T) (if used as intended this is an unlikly case but sometimes happens)
        using internal_type = typename std::conditional<std::is_array<typename std::remove_reference<T>::type>::value,
                                                typename std::remove_cv<T>::type,
                                                typename std::conditional<std::is_lvalue_reference<T>::value,
                                                                        T&,	typename std::decay<T>::type>::type>::type;

        using underlying_enum_type = typename std::remove_cvref_t<T>::enum_type;
        using underlying_enum_variant_type = typename std::remove_cvref_t<T>::enum_variant_type;
        template<underlying_enum_type Value>
        using underlying_enum_variant_mapping_type = typename std::remove_cvref_t<T>::template enum_variant_mapping_t<Value>;

        const std::string enum_name;
        const std::string type_name;
        internal_type value;

        //Disallow assignment of NamedValue; 
        //could run in problems with r value typed NamedValue
        NamedEnumVariant & operator=(const NamedEnumVariant &) = delete;
        BASIC_ALWAYS_INLINE explicit NamedEnumVariant(std::string enumname, std::string nametype, T&& val) 
            : enum_name(std::move(enumname)), type_name(std::move(nametype)), value(std::forward<T>(val)) {}

        template<std::remove_cvref_t<underlying_enum_type> EValue, typename = void>
        struct enum_variant_switch_case_functor {
            template<typename Archive>
            void operator()(const std::string& name, std::remove_cvref_t<underlying_enum_variant_type>&, Archive &)
            {
                if constexpr(!SerAr::IsOutputArchive<Archive>) {
                    std::string error {fmt::format("Missing mapping for enum named: '{}' with value:{}",name, to_string(EValue))};
                    throw std::out_of_range{error.c_str()};
                }
            }
            template<typename Archive>
            void operator()(std::remove_cvref_t<underlying_enum_variant_type>&, Archive &)
            {
                if constexpr(!SerAr::IsOutputArchive<Archive>) {
                    std::string error {fmt::format("Missing mapping for enum value:{}",to_string(EValue))};
                    throw std::out_of_range{error.c_str()};
                }
            }
        };
        template <std::remove_cvref_t<underlying_enum_type> EValue>
        struct enum_variant_switch_case_functor<EValue, std::void_t<typename underlying_enum_variant_mapping_type<EValue>::type>> {
            template<typename Archive>
            void operator()(const std::string& name, std::remove_cvref_t<underlying_enum_variant_type>& variant, Archive &ar)
            {
                using variant_type = typename underlying_enum_variant_mapping_type<EValue>::type;
                if(!std::holds_alternative<variant_type>(variant) )
                {
                    variant = variant_type{};
                }
                ar(Archives::createNamedValue(name, std::get<variant_type>(variant)));
            }
            template<typename Archive>
            void operator()(std::remove_cvref_t<underlying_enum_variant_type>& variant, Archive &ar)
            {
                using variant_type = typename underlying_enum_variant_mapping_type<EValue>::type;
                if(!std::holds_alternative<variant_type>(variant) )
                {
                    variant = variant_type{};
                }
                ar(std::get<variant_type>(variant));
            }
        };

        template <std::remove_cvref_t<underlying_enum_type> EValue>
        struct enum_switch_case_functor  {
            template<typename Archive>
            void operator()(const std::string& name, std::remove_cvref_t<underlying_enum_variant_type>& variant, Archive &ar)
            {
                return enum_variant_switch_case_functor<EValue>{}(name,variant,ar);
            }
            template<typename Archive>
            void operator()(std::remove_cvref_t<underlying_enum_variant_type>& variant, Archive &ar)
            {
                return enum_variant_switch_case_functor<EValue>{}(variant,ar);
            }
        };

        // template<typename Archive>
        // Archive& serialize(Archive& ar)
        // {
        //     auto& enum_value = value.value;
        //     auto& enum_variant = value.variant;
        //     std::string enum_value_str {to_string(enum_value)};
        //     ar(Archives::createNamedValue(enum_name,enum_value_str));
        //     enum_value = from_string<underlying_enum_type>(enum_value_str,enum_value);
        //     ::MyCEL::enum_switch::run<underlying_enum_type, enum_switch_case_functor>(enum_value,type_name,enum_variant,ar);
        //     return ar;
        // }
        template<typename Archive>
        void save(Archive& ar)
        {
            auto& enum_value = value.value;
            auto& enum_variant = value.variant;
            if constexpr (!std::is_reference_v<underlying_enum_type>) {
                ar(Archives::createNamedValue(enum_name,to_string(enum_value)));
            }
            if(type_name.empty()) {
                std::visit([&](auto&& arg) { ar(arg); }, enum_variant);
            }
            else {
                std::visit([&](auto&& arg) { ar(Archives::NamedValue(type_name,arg)); }, enum_variant);
            }
        }
        template<typename Archive>
        void load(Archive& ar)
        {
            using MyCEL::enum_switch;
            auto& enum_value = value.value;
            auto& enum_variant = value.variant;
            std::string enum_str;
            ar(Archives::createNamedValue(enum_name,enum_str));
            enum_value = from_string(enum_str,enum_value);
            if(type_name.empty()) {
                enum_switch::run<std::remove_cvref_t<underlying_enum_type>, enum_switch_case_functor>(enum_value,enum_variant,ar);
            } else {
                enum_switch::run<std::remove_cvref_t<underlying_enum_type>, enum_switch_case_functor>(enum_value,type_name,enum_variant,ar);
            }
        }
    };
    template<concepts::EnumVariant T>
    NamedEnumVariant(std::string, std::string, T&& value) -> NamedEnumVariant<T>;

    template<typename T>
    inline NamedEnumVariant<T> createNamedEnumVariant(std::string ename,std::string tname, T&& value)
    {
        return NamedEnumVariant<T>{std::move(ename),std::move(tname), std::forward<T>(value)};
    }
}
