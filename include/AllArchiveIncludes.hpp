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
#include <stdexcept>
#include <string_view>
#include <MyCEL/basics/enumhelpers.h>
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
        using archive_type = ::Archives::ConfigFile_InputArchive;
        using option_type = ::Archives::ConfigFile_Options;
    };
    template<>
    struct output_archive_traits<ArchiveTypeEnum::ConfigFile> {
        using archive_type = ::Archives::ConfigFile_OutputArchive;
        using option_type = ::Archives::ConfigFile_Options;
        static constexpr auto append_option = option_type{};
        static constexpr auto overwrite_option = option_type{};
    };

    template<>
    struct archive_traits<ArchiveTypeEnum::ConfigFile> {
        static constexpr const ArchiveTypeEnum enum_value = ArchiveTypeEnum::ConfigFile;
        using output = output_archive_traits<enum_value>;
        using input= input_archive_traits<enum_value>;

    };

    template<>
    struct archive_details<ArchiveTypeEnum::ConfigFile> {
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
    template<>
    struct is_input_archive_available<ArchiveTypeEnum::MATLAB> : std::true_type {};

    template<>
    struct input_archive_traits<ArchiveTypeEnum::MATLAB> {
        using archive_type = ::Archives::MatlabInputArchive;
        using option_type = ::Archives::MatlabOptions;
    };
    template<>
    struct output_archive_traits<ArchiveTypeEnum::MATLAB> {
        using archive_type = ::Archives::MatlabOutputArchive;
        using option_type = ::Archives::MatlabOptions;
        static constexpr auto append_option = option_type::update;
        static constexpr auto overwrite_option = option_type::write_v73;
    };

    template<>
    struct archive_details<ArchiveTypeEnum::MATLAB> {
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

    };

    template<>
    struct archive_details<ArchiveTypeEnum::HDF5> {
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
    struct archive_details<ArchiveTypeEnum::JSON> {
        static constexpr std::string_view defaut_file_extension {".json"};
        static constexpr std::array<std::string_view,1> native_file_extensions { {".json"} };
    };

    template<>
    struct archive_enum_value_from_type<SerAr::JSON_InputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::JSON;
    };
    template<>
    struct archive_enum_value_from_type<SerAr::JSON_OutputArchive> {
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

#ifdef SERAR_HAS_TOML
#include <SerAr/TOML/TOML_Archives.hpp>
// TOML defines
namespace SerAr {
    template<>
    struct is_output_archive_available<ArchiveTypeEnum::TOML> : std::true_type {};
    template<>
    struct is_input_archive_available<ArchiveTypeEnum::TOML> : std::true_type {};

    template<>
    struct input_archive_traits<ArchiveTypeEnum::TOML> {
        using archive_type = SerAr::TOML_InputArchive;
        using option_type = SerAr::TOML_InputArchive_Options;
    };
    template<>
    struct output_archive_traits<ArchiveTypeEnum::TOML> {
        using archive_type = SerAr::TOML_OutputArchive;
        using option_type = SerAr::TOML_OutputArchive_Options;
        static constexpr auto append_option = option_type{.mode=std::ios::app};
        static constexpr auto overwrite_option = option_type{.mode=std::ios::trunc};
    };

    template<>
    struct archive_details<ArchiveTypeEnum::TOML> {
        static constexpr std::string_view defaut_file_extension {".toml"};
        static constexpr std::array<std::string_view,1> native_file_extensions { {".toml"} };
    };

    template<>
    struct archive_enum_value_from_type<SerAr::TOML_InputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::TOML;
    };
    template<>
    struct archive_enum_value_from_type<SerAr::TOML_OutputArchive> {
        static constexpr const ArchiveTypeEnum value = ArchiveTypeEnum::TOML;
    };

    namespace traits {
        template<>
        struct is_file_archive<ArchiveTypeEnum::TOML> : std::true_type {};
    }
}
#else
namespace SerAr {
    class TOML_OutputArchive;
    class TOML_InputArchive;
}
#endif
//AllArchiveTypeEnums
namespace SerAr {

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
    inline constexpr auto available_archives_from_array = get_input_available_if<Input,available_if>();
    template<const auto &Input>
    inline constexpr auto available_output_archives_from_array = get_input_available_if<Input,is_output_archive_available>();
    template<const auto &Input>
    inline constexpr auto available_input_archives_from_array = get_input_available_if<Input,is_input_archive_available>();
    template<const auto &Input>
    inline constexpr auto available_file_archives_from_array = get_input_available_if<Input,traits::is_file_archive>();
    inline constexpr auto all_file_archives = available_file_archives_from_array<AllArchiveTypeEnums>;

    template<ArchiveTypeEnum value>
    struct input_archive_enum_type_mapping {
        using type = typename archive_traits<value>::input::archive_type;
    };
    template<ArchiveTypeEnum value>
    using file_archive_input_enum_mapping = input_archive_enum_type_mapping<value>;
    template<ArchiveTypeEnum value>
    struct output_archive_enum_type_mapping {
        using type = typename archive_traits<value>::output::archive_type;
    };
    template<ArchiveTypeEnum value>
    using file_archive_output_enum_mapping = output_archive_enum_type_mapping<value>;

    using file_input_archive_variants = typename ::MyCEL::enum_variant<ArchiveTypeEnum, file_archive_input_enum_mapping, all_file_archives>;
    using file_output_archive_variants = typename ::MyCEL::enum_variant<ArchiveTypeEnum, file_archive_output_enum_mapping, all_file_archives>;

    template< ArchiveTypeEnum value>
    struct archive_default_extension_switch_case {
        inline std::optional<ArchiveTypeEnum> operator()(std::string_view sv) const 
        {
            if(archive_details<value>::defaut_file_extension.compare(sv)==0) {
                return value;
            }
            return std::nullopt;
        }
    };
    template< ArchiveTypeEnum value>
    struct archive_native_extensions_switch_case {
        inline constexpr std::optional<ArchiveTypeEnum> operator()(std::string_view sv) const
        {
            for(const auto& ext: archive_details<value>::native_file_extensions) {
                if(ext.compare(sv)==0)
                    return value;
            }
            return std::nullopt;
        }
    };

    struct archive_extension_switch_case_default {
        inline constexpr std::optional<ArchiveTypeEnum> operator()(std::string_view) const
        {
            return std::nullopt;
        }
    };

    template< ArchiveTypeEnum value>
    struct get_archive_extension_switch_case {
        inline std::string_view operator()() {
            return archive_details<value>::defaut_file_extension;
        }
    };
    struct get_archive_extension_switch_case_default {
        inline std::string_view operator()() {
            throw std::runtime_error{"Invalid value for ArchiveTypeEnum!"};
        }
    };

    template<ArchiveTypeEnum... values>
    using archive_enum_tuple = ::MyCEL::enum_tuple<ArchiveTypeEnum,values...>;

    static constexpr std::string_view getArchiveDefaultExtension(ArchiveTypeEnum value) {
        using EnumTuple = typename MyCEL::apply_nttp_t<all_file_archives,archive_enum_tuple>;
        return MyCEL::enum_switch::switch_impl<EnumTuple::count, EnumTuple, 
                get_archive_extension_switch_case_default, get_archive_extension_switch_case>(
                value);
    }

    inline static constexpr std::optional<ArchiveTypeEnum> getArchiveEnumByExtension(std::string_view sv) {
        //TODO: Make this code better since currently it is ineffective
        using EnumTuple = typename MyCEL::apply_nttp_t<all_file_archives,archive_enum_tuple>;
        for(const auto& archive_enum : all_file_archives) {

            const auto res = MyCEL::enum_switch::switch_impl<EnumTuple::count, EnumTuple, 
                archive_extension_switch_case_default, archive_default_extension_switch_case>(
                archive_enum, sv);
            if(res) {
                return res;
            }
        }
        for(const auto& archive_enum : all_file_archives) {
            const auto res = MyCEL::enum_switch::switch_impl<EnumTuple::count, EnumTuple, 
                archive_extension_switch_case_default, archive_native_extensions_switch_case>(
                archive_enum, sv);
            if(res) {
                return res;
            }
        }
        return std::nullopt;
    }

    template< ArchiveTypeEnum value>
    struct input_archive_from_enum_case {
        template<typename... Args>
        inline file_input_archive_variants operator()(Args ... args) const 
        {
            using Type = typename input_archive_traits<value>::archive_type;
            return file_input_archive_variants{value,file_input_archive_variants::enum_variant_type( std::in_place_type_t<Type>(),std::forward<Args>(args)...)};
        }
    };
    template< ArchiveTypeEnum value>
    struct output_archive_from_enum_case {
        template<typename... Args>
        inline file_output_archive_variants operator()(ArchiveOutputMode mode, Args ... args) const 
        {
            using Type = typename output_archive_traits<value>::archive_type;
            switch(mode)
            {
                case ArchiveOutputMode::Overwrite:
                    return file_output_archive_variants{value, 
                        file_output_archive_variants::enum_variant_type( std::in_place_type_t<Type>(),
                        std::forward<Args>(args)...,
                        output_archive_traits<value>::overwrite_option
                        )};
                case ArchiveOutputMode::CreateOrAppend:
                    return file_output_archive_variants{value, 
                        file_output_archive_variants::enum_variant_type( std::in_place_type_t<Type>(),
                        std::forward<Args>(args)...,
                        output_archive_traits<value>::append_option
                        )};
            }
            return file_output_archive_variants{value, file_output_archive_variants::enum_variant_type( std::in_place_type_t<Type>(),std::forward<Args>(args)...)};
        }
    };
    struct input_archive_from_enum_default {
        template<typename... Args>
        inline file_input_archive_variants operator()(Args...) const 
        {
            throw std::out_of_range{"Unknown value for archive type!"};
        }
    };
    struct output_archive_from_enum_default {
        template<typename... Args>
        inline file_output_archive_variants operator()(ArchiveOutputMode , Args...) const
        {
            throw std::out_of_range{"Unknown value for archive type!"};
        }
    };

    template<typename... Args>
    inline file_input_archive_variants input_archive_from_enum(ArchiveTypeEnum value, Args ... args)
    {
        using EnumTuple = typename MyCEL::apply_nttp_t<all_file_archives,archive_enum_tuple>;
        return MyCEL::enum_switch::switch_impl<EnumTuple::count, EnumTuple, input_archive_from_enum_default, input_archive_from_enum_case>(
                value, std::forward<Args>(args)...);
    }
    template<typename... Args>
    inline file_output_archive_variants output_archive_from_enum(ArchiveOutputMode mode, ArchiveTypeEnum value, Args ... args)
    {
        using EnumTuple = typename MyCEL::apply_nttp_t<all_file_archives,archive_enum_tuple>;
        static_assert(EnumTuple::count!=0);
        return MyCEL::enum_switch::switch_impl<EnumTuple::count, EnumTuple, output_archive_from_enum_default, output_archive_from_enum_case>(
                value, mode, std::forward<Args>(args)...);
    }
}

#endif	// INC_AllArchiveIncludes_H
// end of Archive\AllArchiveIncludes.h
///---------------------------------------------------------------------------------------------------

