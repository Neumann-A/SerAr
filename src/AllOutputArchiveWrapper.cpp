#include "AllOutputArchiveWrapper.hpp"

#include <stdexcept>
#include <fmt/format.h>

namespace SerAr {

    file_output_archive_variants getOutputArchiveTypeByPath(const std::filesystem::path &path, ArchiveOutputMode mode) {
        if(!path.has_extension()) {
            const auto error = fmt::format("Given path '{}' has no extension. Cannot deduce archive type!", path.string() );
            throw std::runtime_error{error.c_str()};
        }
        const auto archive_enum =  SerAr::getArchiveEnumByExtension(path.extension().string());
        if(!archive_enum) {
            const auto error = fmt::format("No archive known to support the file extension: '{}'!", path.extension().string() );
            throw std::runtime_error{error.c_str()};
        }
        return output_archive_from_enum(mode, *archive_enum, path);
    }

    AllFileOutputArchiveWrapper::AllFileOutputArchiveWrapper(const std::filesystem::path &path, ArchiveOutputMode mode) 
        : archive_enum_variant(getOutputArchiveTypeByPath(path,mode)) {}
    AllFileOutputArchiveWrapper::AllFileOutputArchiveWrapper(ArchiveTypeEnum type, const std::filesystem::path &path, ArchiveOutputMode mode) 
        : archive_enum_variant(output_archive_from_enum(mode, type, path)) {};

}
