#pragma once

#include <filesystem>
#include <stack>
#include <concepts>
#include <type_traits>

#include <MyCEL/stdext/is_container.h>
#include <MyCEL/stdext/is_eigen3_type.h>
#include <MyCEL/stdext/is_optional.hpp>

#include <SerAr/Core/InputArchive.h>

#include <SerAr/TOML/TOML_Common.hpp>

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
        template<typename T>
        std::enable_if_t<std::is_same<T, typename Archives::NamedValue<T>::internal_type>::value, std::map<std::string,std::string>> list(const Archives::NamedValue<T>& nval)
        {
            value_stack.push(value_stack.top().at(nval.name));
            const auto tmp {list(nval.val)};
            value_stack.pop();
            return tmp;
        }

        inline ThisClass& load(std::filesystem::path& path)
        {
            std::string tmp;
            this->operator()(tmp);
            path = std::filesystem::path(tmp);
            return *this;
        }

        template<typename T> 
        requires (TOML::IsTOMLValue<std::remove_cvref_t<T>>)
        inline ThisClass& load(NamedValue<T>& nval)
        {
            auto& current_value = value_stack.top();
            nval.val = toml::find<std::remove_cvref_t<T>>(current_value,nval.name);
            return *this;
        }
        template<typename T> 
        requires (TOML::IsTOMLValue<std::remove_cvref_t<T>>)
        inline ThisClass& load(T& val)
        {
            auto& current_value = value_stack.top();
            val = toml::get<std::remove_cvref_t<T>>(current_value);
            return *this;
        }

        template<typename T> 
        requires ( !TOML::IsTOMLValue<std::remove_cvref_t<T>> 
                && !stdext::is_container_v<std::remove_cvref_t<T>>)
        inline ThisClass& load(NamedValue<T>& nval)
        {
            auto& current_table = value_stack.top();
            if constexpr(stdext::IsOptional<std::remove_cvref_t<T>>)
            {
                if(!current_table.as_table().contains(nval.name)) {
                    nval.val = std::nullopt;
                    return *this;
                }
                nval.val = std::remove_cvref_t<typename std::remove_cvref_t<T>::value_type> {};
                value_stack.push(current_table.as_table().at(nval.name));
                this->operator()(*nval.val);
                value_stack.pop();
            }
            else {
                value_stack.push(current_table.as_table().at(nval.name));
                this->operator()(nval.val);
                value_stack.pop();
            }
            return *this;
        }
        template<typename T> 
        requires (!TOML::IsTOMLValue<std::remove_cvref_t<T>> 
                && stdext::is_container_v<std::remove_cvref_t<T>>)
        inline ThisClass& load(NamedValue<T>& nval)
        {
            auto& current_table = value_stack.top();
            auto named_array = current_table.as_table().at(nval.name).as_array();

            nval.val.resize(named_array.size());
            std::transform(named_array.begin(),named_array.end(), nval.val.begin(), 
                [&] (auto& toml_value) {
                    value_stack.push(toml_value);
                    typename std::remove_cvref_t<T>::value_type value;
                    this->operator()(value);
                    value_stack.pop();
                    return value;
                } );
            return *this;
        }

        template<typename T> 
        requires (!TOML::IsTOMLValue<std::remove_cvref_t<T>> 
                && stdext::is_container_v<std::remove_cvref_t<T>>)
        inline ThisClass& load(T& val)
        {
            auto& current_table = value_stack.top();
            const auto array = current_table.as_array();
            val.resize(array.size());
            std::transform(array.begin(),array.end(), val.begin(), 
                [&] (auto& toml_value) {
                    value_stack.push(toml_value);
                    typename std::remove_cvref_t<T>::value_type value;
                    this->operator()(value);
                    value_stack.pop();
                    return value;
                } );
            return *this;
        }

#ifdef EIGEN_CORE_H
        template<typename T> requires(stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
        inline ThisClass& load(Eigen::MatrixBase<T>& value)
        {
            using DataType = typename T::Scalar;
            std::vector<DataType> res;
            auto& current_table = value_stack.top().as_array();

            if constexpr (T::IsVectorAtCompileTime) {
                res = toml::get<decltype(res)>(value_stack.top());
                value = Eigen::Map< T, Eigen::Unaligned>(res.data(),static_cast<Eigen::Index>(res.size()));
            } else {
                const auto rows = current_table.size();
                for (std::size_t i = 0; i < rows; ++i)
                {
                    auto subarray = current_table.at(i);
                    std::vector<DataType> tmp = toml::get<decltype(tmp)>(subarray);;
                    res.insert(res.end(), tmp.begin(), tmp.end());
                }
                if constexpr (T::IsRowMajor)
                {
                    value = Eigen::Map< T, Eigen::Unaligned >(res.data(), rows, res.size() / rows);
                }
                else
                {
                    value = Eigen::Map< T, Eigen::Unaligned, Eigen::Stride<1, T::ColsAtCompileTime>>(res.data(), rows, res.size() / rows);
                }
            }
            return *this;
        }
#endif
    private:
        const Options options{};
        std::stack<toml::value> value_stack;
    };

    #define TOML_ARCHIVE_LOAD(type) \
        extern template TOML_InputArchive& TOML_InputArchive::load<type&>(NamedValue< type &> &); \
        extern template TOML_InputArchive& TOML_InputArchive::load<type>(NamedValue< type > &);
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
    TOML_ARCHIVE_LOAD(std::filesystem::path)
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
    TOML_ARCHIVE_LOAD(std::vector<std::filesystem::path>)
    TOML_ARCHIVE_LOAD(std::optional<bool>)
    TOML_ARCHIVE_LOAD(std::optional<short>)
    TOML_ARCHIVE_LOAD(std::optional<unsigned short>)
    TOML_ARCHIVE_LOAD(std::optional<int>)
    TOML_ARCHIVE_LOAD(std::optional<unsigned int>)
    TOML_ARCHIVE_LOAD(std::optional<long>)
    TOML_ARCHIVE_LOAD(std::optional<unsigned long>)
    TOML_ARCHIVE_LOAD(std::optional<long long>)
    TOML_ARCHIVE_LOAD(std::optional<unsigned long long>)
    TOML_ARCHIVE_LOAD(std::optional<double>)
    TOML_ARCHIVE_LOAD(std::optional<float>)
    TOML_ARCHIVE_LOAD(std::optional<std::string>)
    TOML_ARCHIVE_LOAD(std::optional<std::filesystem::path>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<short>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<unsigned short>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<int>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<unsigned int>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<long>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<unsigned long>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<long long>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<unsigned long long>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<double>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<float>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<std::string>>)
    TOML_ARCHIVE_LOAD(std::optional<std::vector<std::filesystem::path>>)
    #undef TOML_ARCHIVE_LOAD
}
