#include "AllInputArchiveWrapper.hpp"

#include <stdexcept>
#include <fmt/format.h>

namespace SerAr {

    static file_input_archive_variants getInputArchiveTypeByPath(const std::filesystem::path &path) {
        if(!std::filesystem::exists(filepath)) {
            const auto error = fmt::format("Given path '{}' does not exist!", path.string() );
            throw std::runtime_error{error.c_str()};
        }
        if(!path.has_extension()) {
            const auto error = fmt::format("Given path '{}' has no extension. Cannot deduce archive type!", path.string() );
            throw std::runtime_error{error.c_str()};
        }
        const auto archive_enum =  SerAr::getArchiveEnumByExtension(path.extension().string());
        if(!archive_enum) {
            const auto error = fmt::format("No archive known to support the file extension: '{}'!", path.extension().string() );
            throw std::runtime_error{error.c_str()};
        }
        return input_archive_from_enum(*archive_enum, path);
    }

    AllFileInputArchiveWrapper::AllFileInputArchiveWrapper(const std::filesystem::path &path) 
        : archive_enum_variant(getInputArchiveTypeByPath(path)) {}
    AllFileInputArchiveWrapper::AllFileInputArchiveWrapper(ArchiveTypeEnum type, const std::filesystem::path &path) 
        : archive_enum_variant(input_archive_from_enum(type, path)) {}

}
