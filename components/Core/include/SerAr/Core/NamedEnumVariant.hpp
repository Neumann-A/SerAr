#pragma once

#include <MyCEL/basics/enumhelpers.h>
#include <SerAr/Core/NameValue.h>
#include <cassert>
//         using anisotropy_variant = typename MyCEL::apply_nttp_t<IAnisotropyValues,anisotropy_variant_helper_t>;
        // template<IAnisotropy value>
        // struct anisotropy_switch_case
        // {
        //     template<typename Archive>
        //     void operator()(anisotropy_variant& aniso, Archive &ar)
        //     {
        //         using anisotropy_param_type = typename anisotropy_enum_property_mapping<value>::type;
        //         if(!std::holds_alternative<anisotropy_param_type>(aniso) )
        //         {
        //             aniso = anisotropy_param_type{};
        //         }
        //         ar(Archives::createNamedValue(::Properties::Anisotropy::General<prec>::getSectionName(), std::get<anisotropy_param_type>(aniso)));
        //     }
        // };
        // struct anisotropy_default_switch_case
        // {
        //     template<typename Archive>
        //     void operator()(anisotropy_variant& /* aniso */, Archive &/* ar */)
        //     {
        //         throw std::out_of_range{"Type of anisotropy unknown!"};
        //     }
        // };
        // template<typename Archive>
        // void serialize(Archive &ar)
        // {
        //     std::string str{ "undefined" };
        //     if(static_cast<std::underlying_type_t<Properties::IAnisotropy>>(TypeOfAnisotropy) != 0)
        //         str= to_string(TypeOfAnisotropy);

        //     ar(Archives::createNamedValue(std::string{ "Type_of_anisotropy" }, str));
        //     TypeOfAnisotropy = from_string<decltype(TypeOfAnisotropy)>(str);
        //     MyCEL::enum_switch::run<decltype(TypeOfAnisotropy),anisotropy_default_switch_case,anisotropy_switch_case>(TypeOfAnisotropy,AnisotropyProperties,ar);
        // }

namespace SerAr {

    template <typename EnumType, typename Variant, template <EnumType> class EnumTypeMapper = default_type_mapper>
    struct NamedEnumVariant :  {
        template<typename T>
        using NV = Archives::NamedValue<T>;

        NV<EnumType> nv_enum; 
        NV<Variant>  nv_variant; 

        NamedEnumVariant(NV<EnumType>&& enum, NV<Variant>&& variant) 
        : NamedEnum(enum), NamedVariant(variant) {} = default;

    };

    template<typename EnumType, template<EnumType> class VariantMapper, typename Variant>
    struct default_variant_switch_functors
    {
        template<EnumType Value>
        using mapped_type = VariantMapper<Value>::type;

        template<EnumType Value>
        static constexpr std::string_view to_string_view() {
            return to_string_view(Value);
        }

        template<EnumType Value, typename Archive>
        struct case_functor {
            void operator()(Archives::NamedValue<Variant>& in, Archive& ar)
            {
                using type = typename VariantMapper<value>::type;
                if(!std::holds_alternative<type>(val) )
                {
                    val = type{};
                }
                ar(Archives::createNamedValue(in.name, std::get<type>(in.val)));
            }
        };

        struct default_functor {
            template<typename Archive>
            void operator()(Archives::NamedValue<Variant>& in, Archive& ar)
            {
                throw std::out_of_range{"Invalid enum value. No variant type for enum value known!"};
            }
        };
    };

    template<typename EnumType, template<EnumType> class VariantMapper, typename Variant, template<EnumType,typename> class case_functor = default_variant_switch_functors<EnumType, VariantMapper ,Variant>::case_functor, typename default_functor = default_variant_switch_functors<EnumType, VariantMapper ,Variant>::default_functor>
    struct default_archive_type_mapper
    {
        template<EnumType value>
        struct archive_variant_switch_functor
        {
            template<typename Archive>
            decltype(auto) operator()(Variant& val, Archive& ar)
            {
                return case_functor<value,Archive>(val,ar);
            }
        }
    }

    template <typename EnumType, typename Variant, typename EnumTypeMapper = default_archive_type_mapper, typename Archive>
    void save(NamedEnumVariant<EnumType,Variant,EnumTypeMapper>& in, Archive& ar)
    {
        ar(in.nv_enum);
        std::visit([in.nv_variant.name](auto&& arg) { ar(Archives::NamedValue(in.nv_variant.name,arg))}, in.nv_variant.val)
    }
    template <typename EnumType, typename Variant, typename EnumTypeMapper = default_archive_type_mapper, typename Archive>
    void load(NamedEnumVariant<EnumType,Variant,EnumTypeMapper>& in, Archive& ar)
    {
        ar(in.nv_enum);
        MyCEL::enum_switch::run<EnumType,default_functor,EnumTypeMapper<EnumType>::switch_enum_type>(in.nv_enum.val,in.nv_variant,ar);
    }
}