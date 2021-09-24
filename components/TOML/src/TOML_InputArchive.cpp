#include "TOML_InputArchive.hpp"

#include <fmt/core.h>

namespace SerAr {
    using namespace ::Archives;
    using namespace TOML;



    static auto parse_toml(const std::filesystem::path& path) {
        if (!path.has_filename()) {
             const auto msg = fmt::format("Invalid filepath '{}'", path.string());
             throw_runtime_error(msg);
        }
        auto parse_result =  toml::parse(path.string()); 
        return parse_result;
    }

    TOML_InputArchive::TOML_InputArchive(const std::filesystem::path& path, const Options& opt /*, const std::source_location& loc = std::source_location::current()*/)
        : InputArchive(this)
        , options(opt)
        , value_stack({parse_toml(path)})
    {
    }

    auto TOML_InputArchive::list(const Archives::NamedValue<decltype(nullptr)>& value) -> std::map<std::string,std::string> {
        return list(value.getName());
    }

    auto TOML_InputArchive::list(const std::string& str) -> std::map<std::string,std::string> {
        auto& current_table = value_stack.top();
        auto tmp_table = current_table.as_table().at(str).as_table();
        std::map<std::string,std::string> ret;
        for(auto& [key, value] : tmp_table) {
            ret.emplace(key, toml::get<std::string>(value));
        }
        return ret;
    }

    #define TOML_ARCHIVE_LOAD(type) \
        template TOML_InputArchive& TOML_InputArchive::load<type&>(NamedValue< type &> &); \
        template TOML_InputArchive& TOML_InputArchive::load<type>(NamedValue< type > &);
    TOML_ARCHIVE_LOAD(bool)
    TOML_ARCHIVE_LOAD(short)
    TOML_ARCHIVE_LOAD(unsigned short)
    TOML_ARCHIVE_LOAD(int)
    TOML_ARCHIVE_LOAD(unsigned int)
    TOML_ARCHIVE_LOAD(long)
    TOML_ARCHIVE_LOAD(unsigned long)
    TOML_ARCHIVE_LOAD(long long)
    TOML_ARCHIVE_LOAD(unsigned long long)
    TOML_ARCHIVE_LOAD(double)
    TOML_ARCHIVE_LOAD(float)
    TOML_ARCHIVE_LOAD(std::string)
    TOML_ARCHIVE_LOAD(std::vector<short>)
    TOML_ARCHIVE_LOAD(std::vector<unsigned short>)
    TOML_ARCHIVE_LOAD(std::vector<int>)
    TOML_ARCHIVE_LOAD(std::vector<unsigned int>)
    TOML_ARCHIVE_LOAD(std::vector<long>)
    TOML_ARCHIVE_LOAD(std::vector<unsigned long>)
    TOML_ARCHIVE_LOAD(std::vector<long long>)
    TOML_ARCHIVE_LOAD(std::vector<unsigned long long>)
    TOML_ARCHIVE_LOAD(std::vector<double>)
    TOML_ARCHIVE_LOAD(std::vector<float>)
    TOML_ARCHIVE_LOAD(std::vector<std::string>)
    #undef TOML_ARCHIVE_LOAD

}
