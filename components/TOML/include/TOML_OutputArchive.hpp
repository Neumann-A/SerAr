#pragma once

#include <filesystem>
#include <stack>
#include <concepts>
#include <type_traits>
//#include <source_location>
#include <MyCEL/stdext/is_container.h>
#include <MyCEL/stdext/is_eigen3_type.h>

#include <SerAr/Core/NamedValue.h>
#include <SerAr/Core/NamedValueWithDesc.h>
#include <SerAr/Core/OutputArchive.h>

#include <toml++/toml.h>

namespace SerAr
{
    using namespace Archives;

    struct TOML_OutputArchive_Options {
    };

    class TOML_OutputArchive : public OutputArchive<TOML_OutputArchive>
    {
        friend class OutputArchive<TOML_OutputArchive>;
        using ThisClass = TOML_OutputArchive;

    public:
        using Options = TOML_OutputArchive_Options;

        TOML_OutputArchive(const std::filesystem::path& path, const Options& opt = TOML_OutputArchive_Options{});
        ~TOML_OutputArchive() noexcept;
 #ifdef EIGEN_CORE_H

#endif
    private:
        const Options options{};
    };
}
