#include "JSON_InputArchive.hpp"

#include <fstream>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

namespace SerAr {
    using namespace ::Archives;

    static void throw_runtime_error(std::string_view msg)
    {
        const auto s = fmt::format("Error(JSON_Archive): {}", msg);
        throw std::runtime_error{ s.c_str() };
    }

    JSON_InputArchive::JSON_InputArchive(const std::filesystem::path& path, const Options& opt /*, const std::source_location& loc = std::source_location::current()*/)
        : InputArchive(this), options(opt)
    {
        if (!path.has_filename())
        {
            const auto msg = fmt::format("Invalid filepath '{}'", path.string());
            //throw_runtime_error(msg,loc);
            throw_runtime_error(msg);
        }
        std::unique_ptr<std::ifstream> pstr = std::make_unique<std::ifstream>();

        assert(pstr != nullptr);
        pstr->open(path.string().c_str());
        if (!pstr->is_open())
        {
            const auto msg = fmt::format("Unable to open: '{}'", path.string());
            //throw_runtime_error(msg,loc);
            throw_runtime_error(msg);
        }
        *pstr >> json;
    }

    auto JSON_InputArchive::list(const Archives::NamedValue<decltype(nullptr)>& value) -> std::map<std::string,std::string> {
        return list(value.getName());
    }

    auto JSON_InputArchive::list(const std::string& str) -> std::map<std::string,std::string> {
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

    #define JSON_ARCHIVE_LOAD(type) template JSON_InputArchive& JSON_InputArchive::load< NamedValue<type &> &>(NamedValue< type &> &);
    JSON_ARCHIVE_LOAD(bool)
    JSON_ARCHIVE_LOAD(short)
    JSON_ARCHIVE_LOAD(unsigned short)
    JSON_ARCHIVE_LOAD(int)
    JSON_ARCHIVE_LOAD(unsigned int)
    JSON_ARCHIVE_LOAD(long)
    JSON_ARCHIVE_LOAD(unsigned long)
    JSON_ARCHIVE_LOAD(long long)
    JSON_ARCHIVE_LOAD(unsigned long long)
    JSON_ARCHIVE_LOAD(double)
    JSON_ARCHIVE_LOAD(float)
    JSON_ARCHIVE_LOAD(std::string)
    JSON_ARCHIVE_LOAD(std::vector<short>)
    JSON_ARCHIVE_LOAD(std::vector<unsigned short>)
    JSON_ARCHIVE_LOAD(std::vector<int>)
    JSON_ARCHIVE_LOAD(std::vector<unsigned int>)
    JSON_ARCHIVE_LOAD(std::vector<long>)
    JSON_ARCHIVE_LOAD(std::vector<unsigned long>)
    JSON_ARCHIVE_LOAD(std::vector<long long>)
    JSON_ARCHIVE_LOAD(std::vector<unsigned long long>)
    JSON_ARCHIVE_LOAD(std::vector<double>)
    JSON_ARCHIVE_LOAD(std::vector<float>)
    JSON_ARCHIVE_LOAD(std::vector<std::string>)
    #undef JSON_ARCHIVE_LOAD
}
