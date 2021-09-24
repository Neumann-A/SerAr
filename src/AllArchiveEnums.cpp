#include "AllArchiveEnums.hpp"

namespace SerAr {
    std::string to_string(const ArchiveTypeEnum& value)
    {
        return std::string{ArchiveTypeEnumMap[value]};
    }

    template<>
    ArchiveTypeEnum from_string<ArchiveTypeEnum>(const std::string& str)
    {
        return ArchiveTypeEnumMap[str];
    }

    ArchiveTypeEnum from_string(std::string_view str, ArchiveTypeEnum& value)
    {
        return (value = ArchiveTypeEnumMap[str]);
    }
}