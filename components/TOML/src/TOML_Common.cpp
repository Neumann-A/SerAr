#include "TOML_Common.hpp"

#include "TOML_OutputArchive.hpp"

#include <string>
#include <vector>

#include <filesystem>

namespace SerAr::TOML {
    void throw_runtime_error(std::string_view msg)
    {
        const auto s = fmt::format("Error(TOML_Archive): {}", msg);
        throw std::runtime_error{ s.c_str() };
    }

    static_assert(IsTOMLValue<int>);
    static_assert(IsTOMLValue<std::vector<int>>);
    static_assert(IsTOMLValue<std::vector<std::string>>);
    static_assert(IsTOMLValue<std::string>);

    static_assert(!IsTOMLValue<std::vector<std::vector<int>>>);
    static_assert(stdext::is_container_v<std::vector<std::vector<int>>>);

    namespace {
        struct test;
    }
    static_assert(!IsTOMLValue<test>);

    static_assert(UseArchiveMemberSave<const std::vector<int> &, Archives::OutputArchive<SerAr::TOML_OutputArchive>::ArchiveType>);
    static_assert(UseArchiveMemberSave<std::vector<int>, Archives::OutputArchive<SerAr::TOML_OutputArchive>::ArchiveType>);
    static_assert(UseArchiveMemberSave<std::vector<std::vector<int> > &, Archives::OutputArchive<SerAr::TOML_OutputArchive>::ArchiveType>);

    static_assert(!TOML::IsTOMLValue<std::filesystem::path>);
    static_assert(!TOML::IsTOMLNormalValue<std::vector<std::filesystem::path>>);
    static_assert(!TOML::IsTOMLContainerValue<std::vector<std::filesystem::path>>);
    static_assert(!TOML::IsTOMLValue<std::vector<std::filesystem::path>>);

}