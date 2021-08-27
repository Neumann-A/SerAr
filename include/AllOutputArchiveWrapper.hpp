#pragma once

#include "AllArchiveEnums.hpp"
#include "AllArchiveIncludes.hpp"

#include <filesystem>
#include <utility>

namespace SerAr {

    class AllFileOutputArchiveWrapper {
        ::SerAr::file_output_archive_variants archive_enum_variant;
    public:
        AllFileOutputArchiveWrapper(const std::filesystem::path &path, ArchiveOutputMode mode = ArchiveOutputMode::CreateOrAppend);
        AllFileOutputArchiveWrapper(ArchiveTypeEnum type, const std::filesystem::path &path, ArchiveOutputMode mode = ArchiveOutputMode::CreateOrAppend);
    
        template<typename... Args>
        AllFileOutputArchiveWrapper& operator()(Args... args)
        {
            std::visit([&](auto&& ar){ ar(std::forward<Args>(args)...); },archive_enum_variant.variant);
            return *this;
        }
    };

}
