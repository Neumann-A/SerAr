#include "TOML_InputArchive.hpp"

#include <fmt/core.h>

namespace SerAr {
    using namespace ::Archives;

    static void throw_runtime_error(std::string_view msg)
    {
        const auto s = fmt::format("Error(TOML_Archive): {}", msg);
        throw std::runtime_error{ s.c_str() };
    }

    static auto parse_toml(const std::filesystem::path& path) {
        if (!path.has_filename()) {
            const auto msg = fmt::format("Invalid filepath '{}'", path.string());
            throw_runtime_error(msg);
        }
        auto parse_result =  toml::parse_file(path); 
        if (!result) {
            const auto msg = fmt::format("Parsing of toml failed: '{}'", result.error());
            throw_runtime_error(msg);
        }
        return parse_result;
    }

    TOML_InputArchive::TOML_InputArchive(const std::filesystem::path& path, const Options& opt /*, const std::source_location& loc = std::source_location::current()*/)
        : InputArchive(this), options(opt), input_table(parse_toml(path))  {}

    auto TOML_InputArchive::list(const Archives::NamedValue<decltype(nullptr)>& value) -> std::map<std::string,std::string> {
        return list(value.getName());
    }

    auto TOML_InputArchive::list(const std::string& str) -> std::map<std::string,std::string> {
        std::map<std::string,std::string> ret;
        json_pointer.push_back(str);
        auto json_objects = json[json_pointer];
        for(auto& [key, value] : json_objects.items() )
        {
            ret.emplace(key, value);
        }
        json_pointer.pop_back();
        return ret;
    }
}
