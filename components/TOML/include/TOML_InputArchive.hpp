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
#include <SerAr/Core/InputArchive.h>

#include <toml++/toml.h>

namespace SerAr
{
    using namespace Archives;

    struct TOML_InputArchive_Options {
    };

    class TOML_InputArchive : public InputArchive<TOML_InputArchive>
    {
        friend class InputArchive<TOML_InputArchive>;
        using ThisClass = TOML_InputArchive;

    public:
        using Options = TOML_InputArchive_Options;

        TOML_InputArchive(const std::filesystem::path& path, const Options& opt = Options{});
        auto list(const std::string& str) -> std::map<std::string,std::string>;
        auto list(const Archives::NamedValue<decltype(nullptr)>& value) -> std::map<std::string,std::string>;

#endif
    private:
        const Options options{};
        toml::table input_table;
    };

}
