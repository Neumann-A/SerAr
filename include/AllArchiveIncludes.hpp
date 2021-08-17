///---------------------------------------------------------------------------------------------------
// file:		Archive\AllArchiveIncludes.h
//
// summary: 	Declares all archive includes 
//
// Copyright (c) 2019 Alexander Neumann.
//
// author: Neumann
// date: 10.05.2019
#pragma once
#ifndef INC_AllArchiveIncludes_H
#define INC_AllArchiveIncludes_H
///---------------------------------------------------------------------------------------------------

#include "AllArchiveEnums.hpp"
#include <filesystem>
#include <optional>
#include <MyCEL/basics/templatehelpers.h>

#ifdef SERAR_HAS_CONFIGFILE
#include <SerAr/ConfigFile/ConfigFile_Archive.h>
namespace SerAr {

    template<>
    struct is_output_archive_available<ArchiveTypeEnum::ConfigFile> : std::true_type {};
    template<>
    struct is_input_archive_available<ArchiveTypeEnum::ConfigFile> : std::true_type {};

    template<>
    struct input_archive_traits<ArchiveTypeEnum::ConfigFile> {
        using archive_type = Archives::ConfigFile_InputArchive;
        using option_type = Archives::ConfigFile_Options;
    };
    template<>
    struct output_archive_traits<ArchiveTypeEnum::ConfigFile> {
        using archive_type = Archives::ConfigFile_OutputArchive;
        using option_type = Archives::ConfigFile_Options;
        static constexpr auto append_option = option_type{};
        static constexpr auto overwrite_option = option_type{};
    };

    template<>
    struct archive_traits<ArchiveTypeEnum::ConfigFile> {
        static constexpr const ArchiveTypeEnum enum_value = ArchiveTypeEnum::ConfigFile;
        using output = output_archive_traits<enum_value>;
        using input= input_archive_traits<enum_value>;
        static constexpr std::string_view defaut_file_extension {".ini"};
        static constexpr std::array<std::string_view,2> native_file_extensions { {{".ini"}, {".txt"}}};
    };
    template<>
    struct archive_enum_value_from_type<Archives::ConfigFile_InputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::ConfigFile;
    };
    template<>
    struct archive_enum_value_from_type<Archives::ConfigFile_OutputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::ConfigFile;
    };

    namespace traits {
        template<>
        struct is_file_archive<ArchiveTypeEnum::ConfigFile> : std::true_type {};
    }
}
#endif

#ifdef SERAR_HAS_MATLAB
#include <SerAr/MATLAB/Matlab_Archive.h>
namespace SerAr {
    template<>
    struct is_output_archive_available<ArchiveTypeEnum::MATLAB> : std::true_type {};
    //template<>
    //struct is_input_archive_available<ArchiveTypeEnum::MATLAB> : std::true_type {};

    // template<>
    // struct input_archive_traits<ArchiveTypeEnum::MATLAB> {
    //     using archive_type = Archives::HDF5_InputArchive;
    //     using option_type = Archives::HDF5_InputOptions;
    // };
    template<>
    struct output_archive_traits<ArchiveTypeEnum::MATLAB> {
        using archive_type = Archives::MatlabOutputArchive;
        using option_type = Archives::MatlabOptions;
        static constexpr auto append_option = option_type::update;
        static constexpr auto overwrite_option = option_type::write_v73;
    };

    template<>
    struct archive_traits<ArchiveTypeEnum::MATLAB> {
        static constexpr const ArchiveTypeEnum enum_value = ArchiveTypeEnum::MATLAB;
        using output = output_archive_traits<enum_value>;
        using input= void;
        static constexpr std::string_view defaut_file_extension {".mat"};
        static constexpr std::array<std::string_view,2> native_file_extensions { {".mat", ".m"} };
    };
    template<>
    struct archive_enum_value_from_type<Archives::MatlabInputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::MATLAB;
    };
    template<>
    struct archive_enum_value_from_type<Archives::MatlabOutputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::MATLAB;
    };

    namespace traits {
        template<>
        struct is_file_archive<ArchiveTypeEnum::MATLAB> : std::true_type {};
    }
}
#else
namespace Archives {
    class MatlabOptions;
    class MatlabOutputArchive;
    class MatlabInputArchive;
}
#endif

// HDF5
#ifdef SERAR_HAS_HDF5
#include <SerAr/HDF5/HDF5_Archive.h>
namespace SerAr {
    template<>
    struct is_output_archive_available<ArchiveTypeEnum::HDF5> : std::true_type {};
    template<>
    struct is_input_archive_available<ArchiveTypeEnum::HDF5> : std::true_type {};

    template<>
    struct input_archive_traits<ArchiveTypeEnum::HDF5> {
        using archive_type = Archives::HDF5_InputArchive;
        using option_type = Archives::HDF5_InputOptions;
    };
    template<>
    struct output_archive_traits<ArchiveTypeEnum::HDF5> {
        using archive_type = Archives::HDF5_OutputArchive;
        using option_type = Archives::HDF5_OutputOptions;
        static constexpr auto append_option = option_type{HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode::OpenOrCreate};
        static constexpr auto overwrite_option = option_type{HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode::CreateOrOverwrite};
    };

    template<>
    struct archive_traits<ArchiveTypeEnum::HDF5> {
        static constexpr const ArchiveTypeEnum enum_value = ArchiveTypeEnum::HDF5;
        using output = output_archive_traits<enum_value>;
        using input= input_archive_traits<enum_value>;
        static constexpr std::string_view defaut_file_extension {".hdf5"};
        static constexpr std::array<std::string_view,2> native_file_extensions { ".hdf5", ".h5"};
        static constexpr std::array<std::string_view,1> derived_file_extensions { ".mdf"};
    };
    template<>
    struct archive_enum_value_from_type<Archives::HDF5_InputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::HDF5;
    };
    template<>
    struct archive_enum_value_from_type<Archives::HDF5_OutputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::HDF5;
    };

    namespace traits {
        template<>
        struct is_file_archive<ArchiveTypeEnum::HDF5> : std::true_type {};
    }
}
#else
namespace Archives {
    class HDF5_OutputArchive;
    class HDF5_InputArchive;
}
#endif

#ifdef SERAR_HAS_JSON
#include <SerAr/JSON/JSON_Archives.hpp>
// JSON defines
namespace SerAr {
    template<>
    struct is_output_archive_available<ArchiveTypeEnum::JSON> : std::true_type {};
    template<>
    struct is_input_archive_available<ArchiveTypeEnum::JSON> : std::true_type {};

    template<>
    struct input_archive_traits<ArchiveTypeEnum::JSON> {
        using archive_type = SerAr::JSON_InputArchive;
        using option_type = SerAr::JSON_InputArchive_Options;
    };
    template<>
    struct output_archive_traits<ArchiveTypeEnum::JSON> {
        using archive_type = SerAr::JSON_OutputArchive;
        using option_type = SerAr::JSON_OutputArchive_Options;
        static constexpr auto append_option = option_type{.mode=std::ios::app};
        static constexpr auto overwrite_option = option_type{.mode=std::ios::trunc};
    };

    template<>
    struct archive_traits<ArchiveTypeEnum::JSON> {
        static constexpr const ArchiveTypeEnum enum_value = ArchiveTypeEnum::JSON;
        using output = output_archive_traits<enum_value>;
        using input= input_archive_traits<enum_value>;
        static constexpr std::string_view defaut_file_extension {".json"};
        static constexpr std::array<std::string_view,1> native_file_extensions { {".json"} };
    };
    template<>
    struct archive_enum_value_from_type<Archives::JSON_InputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::JSON;
    };
    template<>
    struct archive_enum_value_from_type<Archives::JSON_OutputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::JSON;
    };

    namespace traits {
        template<>
        struct is_file_archive<ArchiveTypeEnum::JSON> : std::true_type {};
    }
}
#else
namespace SerAr {
    class JSON_OutputArchive;
    class JSON_InputArchive;
}
#endif

namespace SerAr {
    // static constexpr ArchiveTypeEnum get_archive_enum_from_file_extension(std::string_view extension) {

    // }


    template<std::size_t count = 0, const auto &Input, template <std::remove_cvref_t<decltype(::std::get<0>(Input))>> typename available_if, std::size_t... Is>
    constexpr auto count_available_if_impl(std::index_sequence<Is...>) {
        return (count + ... + available_if<::std::get<Is>(Input)>());
    }
    template<const auto &Input, template <std::remove_cvref_t<decltype(::std::get<0>(Input))>> typename available_if>
    constexpr auto count_available_if() {
        return (count_available_if_impl<0, Input, available_if>(std::make_index_sequence<Input.size()>()));
    }

    template<const auto &Input, template <std::remove_cvref_t<decltype(::std::get<0>(Input))>> typename available_if, std::size_t... Is>
    constexpr auto create_bool_array_available_if_impl(std::index_sequence<Is...>) {
        using array_type = std::remove_cvref_t<decltype(::std::get<0>(Input))>;
        return MyCEL::static_map<array_type, bool,sizeof...(Is)> { 
            { std::pair<array_type,bool>{const_cast<array_type&>(::std::get<Is>(Input)), available_if<::std::get<Is>(Input)>()} ... } 
            };
    }
    template<const auto &Input, template <std::remove_cvref_t<decltype(::std::get<0>(Input))>> typename available_if>
    constexpr auto create_bool_array_available_if() {
        return create_bool_array_available_if_impl<Input, available_if>(std::make_index_sequence<Input.size()>());
    }

    template<const auto &Input, template <std::remove_cvref_t<decltype(::std::get<0>(Input))>> typename available_if>
    constexpr auto get_input_available_if() {
        using type = std::remove_cvref_t<decltype(::std::get<0>(Input))>;
        constexpr auto available_array = create_bool_array_available_if<Input,available_if>();
        constexpr auto size = count_available_if<Input,available_if>();
        std::array<type,size> result;
        std::copy_if(Input.cbegin(),Input.cend(),result.begin(),[available_array](auto elem) {return available_array[elem];});
        return result;
    }

    template<const auto &Input, template <std::remove_cvref_t<decltype(::std::get<0>(Input))>> typename available_if>
    constexpr const auto available_archives_from_array = get_input_available_if<Input,available_if>();

    template<const auto &Input>
    static constexpr const auto available_output_archives_from_array = get_input_available_if<Input,is_output_archive_available>();
    template<const auto &Input>
    static constexpr const auto available_input_archives_from_array = get_input_available_if<Input,is_input_archive_available>();

    //static constexpr const auto all_available_output_archives = available_output_archives_from_array<AllArchiveTypeEnums>;
    //static constexpr const auto all_available_input_archives = available_input_archives_from_array<AllArchiveTypeEnums>;

    //template<ArchiveTypeEnum value>
    //struct output_archives_enum_mapping { using type = typename output_archive_traits<value>::archive_type; };
    //template <ArchiveTypeEnum... Values>
    //using output_archives_variant_helper_t = typename MyCEL::enum_variant_creator_t<ArchiveTypeEnum, output_archives_enum_mapping, Values...>;
    //template<const auto &Input>
    //using output_archives_variant = typename MyCEL::apply_nttp_t<Input,output_archives_variant_helper_t>;
    //using all_output_archives_variant = typename MyCEL::apply_nttp_t<all_available_output_archives,output_archives_variant_helper_t>;

    //template<ArchiveTypeEnum value>
    //struct input_archives_enum_mapping { using type = typename input_archive_traits<value>::archive_type; };
    //template <ArchiveTypeEnum... Values>
    //using input_archives_variant_helper_t = typename MyCEL::enum_variant_creator_t<ArchiveTypeEnum, input_archives_enum_mapping, Values...>;
    //template<const auto &Input>
    //using input_archives_variant = typename MyCEL::apply_nttp_t<Input,input_archives_variant_helper_t>;
    //using all_input_archives_variant = typename MyCEL::apply_nttp_t<all_available_input_archives,input_archives_variant_helper_t>;


    //    static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::JSON;

    // template<typename input_variant, ArchiveTypeEnum value>
    // struct input_archive_switch_case
    // {
    //     template<typename Archive>
    //     void operator()(input_variant& ar, )
    //     {
    //         using anisotropy_param_type = typename anisotropy_enum_property_mapping<value>::type;
    //         if(!std::holds_alternative<anisotropy_param_type>(aniso) )
    //         {
    //             aniso = anisotropy_param_type{};
    //         }
    //     }
    // };
    // template<const auto &Input, template <std::remove_cvref_t<decltype(::std::get<0>(Input))>> typename available_if, std::size_t... Is>
    // constexpr auto create_bool_array_available_if_impl(std::index_sequence<Is...>) {
    //     using array_type = std::remove_cvref_t<decltype(::std::get<0>(Input))>;
    //     return MyCEL::static_map<array_type, bool,sizeof...(Is)> { 
    //         { std::pair<array_type,bool>{const_cast<array_type&>(::std::get<Is>(Input)), available_if<::std::get<Is>(Input)>()} ... } 
    //         };
    // }
    //template<const auto &Input, template <std::remove_cvref_t<decltype(::std::get<0>(Input))>> typename traits, std::size_t... Is>
    //constexpr auto get_archive_by_extension_impl(std::index_sequence<Is...>) {
    //    using array_type = std::remove_cvref_t<decltype(::std::get<0>(Input))>;
    //    return MyCEL::static_map<array_type, bool,sizeof...(Is)> { 
    //        { std::pair<array_type,bool>{const_cast<array_type&>(::std::get<Is>(Input)), available_if<::std::get<Is>(Input)>()} ... } 
    //        };
    //}

    // TODO
    // template<const auto &Input, template <std::remove_cvref_t<decltype(::std::get<0>(Input))>> typename traits> 
    // constexpr std::optional<> get_archive_by_extension(const std::filesystem::path& path)
    // {
    //     auto 
        
    // }
    // template<const auto &Input> 
    // constexpr auto open_input_archive(const std::filesystem::path& path)
    // {
    //     using input_variant = input_archives_variant<Input>;


    // };


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
        //MyCEL::enum_switch::run<decltype(TypeOfAnisotropy),anisotropy_default_switch_case,anisotropy_switch_case>(TypeOfAnisotropy,AnisotropyProperties,ar);
}

#endif	// INC_AllArchiveIncludes_H
// end of Archive\AllArchiveIncludes.h
///---------------------------------------------------------------------------------------------------

