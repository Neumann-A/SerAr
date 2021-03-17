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

    JSON_InputArchive::JSON_InputArchive(const Options& opt, const std::filesystem::path& path /*, const std::source_location& loc = std::source_location::current()*/)
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
}
