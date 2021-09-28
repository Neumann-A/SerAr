#include "TOML_OutputArchive.hpp"

#include <fstream>
#include <iostream>

#include <fmt/core.h>

namespace SerAr {
    using namespace ::Archives;

    static void throw_runtime_error(std::string_view msg)
    {
        const auto s = fmt::format("Error(TOML_Archive): {}",msg);
        throw std::runtime_error{ s.c_str() };
    }

    TOML_OutputArchive::TOML_OutputArchive( const std::filesystem::path& path, const Options &opt /*, const std::source_location& loc = std::source_location::current()*/)
        : OutputArchive(this), options(opt)
    {

        if (!path.has_filename())
        {
            const auto msg = fmt::format("Invalid filepath '{}'",path.string());
            //throw_runtime_error(msg,loc);
            throw_runtime_error(msg);
        }

        fileptr = std::make_unique<std::ofstream>();
        fileptr->open(path.string().c_str(), options.mode);
        if (!fileptr->is_open())
        {
            const auto msg= fmt::format("Unable to open file at '{}'",path.string());
            throw_runtime_error(msg);
        }
        assert(fileptr != nullptr);
        value_stack.push(toml::value{});
    }

    TOML_OutputArchive::~TOML_OutputArchive() noexcept {
        assert(value_stack.size()==1);
        if(value_stack.empty())
            return;
        //std::cout << std::scientific << std::setprecision(7) << std::setw(80) << value_stack.top()  << std::endl;
        *fileptr  << std::scientific << std::setprecision(7) << std::setw(80) << value_stack.top() << std::flush;
        value_stack.pop();
        fileptr->close();
    }

    // void throw_runtime_error(std::string_view msg,const std::source_location& loc = std::source_location::current())
    // {
    //     const auto s = fmt::format("Error({}): {}",loc,msg);
    //     throw std::runtime_error{ s.c_str() };
    // }
    
    #define TOML_ARCHIVE_SAVE(type) \
        template TOML_OutputArchive& TOML_OutputArchive::save< type &>(const NamedValue< type &> &); \
        template TOML_OutputArchive& TOML_OutputArchive::save< const type &>(const NamedValue< const type &> &); \
        template TOML_OutputArchive& TOML_OutputArchive::save< type >(const NamedValue< type > &); \
        template TOML_OutputArchive& TOML_OutputArchive::save< const type >(const NamedValue< const type > &); 
    TOML_ARCHIVE_SAVE(bool)
    TOML_ARCHIVE_SAVE(short)
    TOML_ARCHIVE_SAVE(unsigned short)
    TOML_ARCHIVE_SAVE(int)
    TOML_ARCHIVE_SAVE(unsigned int)
    TOML_ARCHIVE_SAVE(long)
    TOML_ARCHIVE_SAVE(unsigned long)
    TOML_ARCHIVE_SAVE(long long)
    TOML_ARCHIVE_SAVE(unsigned long long)
    TOML_ARCHIVE_SAVE(double)
    TOML_ARCHIVE_SAVE(float)
    TOML_ARCHIVE_SAVE(std::string)
    TOML_ARCHIVE_SAVE(std::filesystem::path)
    TOML_ARCHIVE_SAVE(std::vector<short>)
    TOML_ARCHIVE_SAVE(std::vector<unsigned short>)
    TOML_ARCHIVE_SAVE(std::vector<int>)
    TOML_ARCHIVE_SAVE(std::vector<unsigned int>)
    TOML_ARCHIVE_SAVE(std::vector<long>)
    TOML_ARCHIVE_SAVE(std::vector<unsigned long>)
    TOML_ARCHIVE_SAVE(std::vector<long long>)
    TOML_ARCHIVE_SAVE(std::vector<unsigned long long>)
    TOML_ARCHIVE_SAVE(std::vector<double>)
    TOML_ARCHIVE_SAVE(std::vector<float>)
    TOML_ARCHIVE_SAVE(std::vector<std::string>)
    TOML_ARCHIVE_SAVE(std::vector<std::filesystem::path>)
    TOML_ARCHIVE_SAVE(std::optional<bool>)
    TOML_ARCHIVE_SAVE(std::optional<short>)
    TOML_ARCHIVE_SAVE(std::optional<unsigned short>)
    TOML_ARCHIVE_SAVE(std::optional<int>)
    TOML_ARCHIVE_SAVE(std::optional<unsigned int>)
    TOML_ARCHIVE_SAVE(std::optional<long>)
    TOML_ARCHIVE_SAVE(std::optional<unsigned long>)
    TOML_ARCHIVE_SAVE(std::optional<long long>)
    TOML_ARCHIVE_SAVE(std::optional<unsigned long long>)
    TOML_ARCHIVE_SAVE(std::optional<double>)
    TOML_ARCHIVE_SAVE(std::optional<float>)
    TOML_ARCHIVE_SAVE(std::optional<std::string>)
    TOML_ARCHIVE_SAVE(std::optional<std::filesystem::path>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<short>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<unsigned short>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<int>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<unsigned int>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<long>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<unsigned long>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<long long>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<unsigned long long>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<double>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<float>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<std::string>>)
    TOML_ARCHIVE_SAVE(std::optional<std::vector<std::filesystem::path>>)
    #undef TOML_ARCHIVE_SAVE

    static_assert(!TOML::IsTOMLValue<std::filesystem::path>);
    static_assert(!TOML::IsTOMLNormalValue<std::vector<std::filesystem::path>>);
    static_assert(!TOML::IsTOMLContainerValue<std::vector<std::filesystem::path>>);
    static_assert(!TOML::IsTOMLValue<std::vector<std::filesystem::path>>);
}
