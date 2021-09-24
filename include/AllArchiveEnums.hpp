#pragma once

#include <string_view>
#include <type_traits>

#include <MyCEL/basics/enumhelpers.h>
#include <MyCEL/types/static_map.hpp>

namespace SerAr {
    using namespace std::literals::string_view_literals;
    enum class ArhiveType { Input, Output};
    enum class ArchiveInputMode { Open };
    enum class ArchiveOutputMode { Overwrite = 1, CreateOrAppend= 2};
    enum class ArchiveTypeEnum { ConfigFile = 1, JSON, TOML, MATLAB, HDF5, ProgramOptionsWrapper, CerealWrapper};
    inline constexpr MyCEL::static_map<ArchiveTypeEnum, std::string_view, 7> ArchiveTypeEnumMap { { { 
                            { ArchiveTypeEnum::ConfigFile,               "ConfigFile"sv }
                            ,{ ArchiveTypeEnum::JSON,                    "JSON"sv }
                            ,{ ArchiveTypeEnum::TOML,                    "TOML"sv }
                            ,{ ArchiveTypeEnum::MATLAB,                  "MATLAB"sv }
                            ,{ ArchiveTypeEnum::HDF5,                    "HDF5"sv }
                            ,{ ArchiveTypeEnum::ProgramOptionsWrapper,   "ProgramOptions"sv }
                            ,{ ArchiveTypeEnum::CerealWrapper,           "Cereal"sv }
                            } } };

    inline constexpr auto AllArchiveTypeEnums{ ArchiveTypeEnumMap.get_key_array() };

    template<typename T>
    T from_string(const std::string&);
    template<>
    ArchiveTypeEnum from_string<ArchiveTypeEnum>(const std::string& FieldString);

    ArchiveTypeEnum from_string(std::string_view, ArchiveTypeEnum&);
    std::string to_string(const ArchiveTypeEnum& field);


    template<ArchiveTypeEnum value>
    struct get_archive_type {};

    template<ArchiveTypeEnum value>
    struct is_output_archive_available : std::false_type {};
    template<ArchiveTypeEnum value>
    inline constexpr bool is_output_archive_available_v = is_output_archive_available<value>::value;
    template<ArchiveTypeEnum value>
    struct is_input_archive_available : std::false_type {};
    template<ArchiveTypeEnum value>
    inline constexpr bool is_input_archive_available_v = is_input_archive_available<value>::value;

    template<ArchiveTypeEnum value>
    struct archive_details {};

    template<ArchiveTypeEnum value>
    struct input_archive_traits{
        static_assert(is_input_archive_available_v<value>);
    };
    template<ArchiveTypeEnum value>
    struct output_archive_traits {
        static_assert(is_output_archive_available_v<value>);
    };

    template<ArchiveTypeEnum value>
    struct archive_traits {
        using input = input_archive_traits<value>;
        using output = output_archive_traits<value>;
        using details = archive_details<value>;
    };


    template<typename Archive>
    struct archive_enum_value_from_type {
    };

    namespace traits {
        template<ArchiveTypeEnum value>
        struct is_file_archive : std::false_type {};
        template<ArchiveTypeEnum value>
        inline constexpr bool is_file_archive_v = is_file_archive<value>::value;
    }
}

