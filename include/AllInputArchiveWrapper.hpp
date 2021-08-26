#pragma once

#include "AllArchiveEnums.hpp"
#include "AllArchiveIncludes.hpp"
#include <SerAr/Core/LoadConstructor.h>

#include <filesystem>
#include <utility>

namespace SerAr {

    class AllFileInputArchiveWrapper {
        ::SerAr::file_input_archive_variants archive_enum_variant;
    public:
        AllFileInputArchiveWrapper(const std::filesystem::path &path);
        AllFileInputArchiveWrapper(ArchiveTypeEnum type, const std::filesystem::path &path);
    
        template<typename... Args>
        auto operator()(Args... args)
        {
            return std::visit([&args](auto& ar){ return ar.operator()(std::forward<Args>(args)...); },archive_enum_variant.variant);
        }

        template<typename T>
        auto construct()
        {
            return std::visit([](auto& archive) -> T { return Archives::LoadConstructor<T>::construct(archive); },
            archive_enum_variant.variant);
        }
    };

}
