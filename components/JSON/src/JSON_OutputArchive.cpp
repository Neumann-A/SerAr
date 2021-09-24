#include "JSON_OutputArchive.hpp"

#include <fstream>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

namespace SerAr {
    using namespace ::Archives;

    static void throw_runtime_error(std::string_view msg)
    {
        const auto s = fmt::format("Error(JSON_Archive): {}",msg);
        throw std::runtime_error{ s.c_str() };
    }

    JSON_OutputArchive::JSON_OutputArchive( const std::filesystem::path& path, const Options &opt /*, const std::source_location& loc = std::source_location::current()*/)
        : OutputArchive(this), options(opt)
    {   
        json_stack.push(JSONType{});
        if (!path.has_filename())
        {
            const auto msg= fmt::format("Invalid filepath '{}'",path.string());
            //throw_runtime_error(msg,loc);
            throw_runtime_error(msg);
        }
        pstr = std::make_unique<std::ofstream>();
        assert(pstr != nullptr);
        pstr->open(path.string().c_str(), options.mode);
        if (!pstr->is_open())
        {
            const auto msg= fmt::format("Unable to open: '{}'",path.string());
            //throw_runtime_error(msg,loc);
            throw_runtime_error(msg);
        }
    }

    JSON_OutputArchive::~JSON_OutputArchive() noexcept {
        assert(json_stack.size()==1);
        if(json_stack.empty())
            return;
        
        *pstr << std::setw(options.indent_spaces) << json_stack.top();
        json_stack.pop();
        *pstr << std::flush;
        pstr->close();
    }

    // void throw_runtime_error(std::string_view msg,const std::source_location& loc = std::source_location::current())
    // {
    //     const auto s = fmt::format("Error({}): {}",loc,msg);
    //     throw std::runtime_error{ s.c_str() };
    // }
    

    #define JSON_ARCHIVE_SAVE(type) template JSON_OutputArchive& JSON_OutputArchive::save< type &>(const NamedValue< type &> &);
    JSON_ARCHIVE_SAVE(bool)
    JSON_ARCHIVE_SAVE(short)
    JSON_ARCHIVE_SAVE(unsigned short)
    JSON_ARCHIVE_SAVE(int)
    JSON_ARCHIVE_SAVE(unsigned int)
    JSON_ARCHIVE_SAVE(long)
    JSON_ARCHIVE_SAVE(unsigned long)
    JSON_ARCHIVE_SAVE(long long)
    JSON_ARCHIVE_SAVE(unsigned long long)
    JSON_ARCHIVE_SAVE(double)
    JSON_ARCHIVE_SAVE(float)
    JSON_ARCHIVE_SAVE(std::string)
    JSON_ARCHIVE_SAVE(std::vector<short>)
    JSON_ARCHIVE_SAVE(std::vector<unsigned short>)
    JSON_ARCHIVE_SAVE(std::vector<int>)
    JSON_ARCHIVE_SAVE(std::vector<unsigned int>)
    JSON_ARCHIVE_SAVE(std::vector<long>)
    JSON_ARCHIVE_SAVE(std::vector<unsigned long>)
    JSON_ARCHIVE_SAVE(std::vector<long long>)
    JSON_ARCHIVE_SAVE(std::vector<unsigned long long>)
    JSON_ARCHIVE_SAVE(std::vector<double>)
    JSON_ARCHIVE_SAVE(std::vector<float>)
    JSON_ARCHIVE_SAVE(std::vector<std::string>)
    #undef JSON_ARCHIVE_SAVE
}
