#include "JSON_OutputArchive.hpp"

#include <fstream>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

namespace SerAr {
    using namespace ::Archives;
    // #ifdef EIGEN_CORE_H

    JSON_OutputArchive::JSON_OutputArchive(const Options &opt, const std::filesystem::path& path /*, const std::source_location& loc = std::source_location::current()*/)
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
        pstr->open(path.string().c_str(), std::fstream::trunc);
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
    }

    // void throw_runtime_error(std::string_view msg,const std::source_location& loc = std::source_location::current())
    // {
    //     const auto s = fmt::format("Error({}): {}",loc,msg);
    //     throw std::runtime_error{ s.c_str() };
    // }
    
    void throw_runtime_error(std::string_view msg)
    {
        const auto s = fmt::format("Error(JSON_Archive): {}",msg);
        throw std::runtime_error{ s.c_str() };
    }
}
