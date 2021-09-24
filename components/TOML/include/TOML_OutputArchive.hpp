#pragma once

#include <filesystem>
#include <stack>
#include <concepts>
#include <type_traits>
#include <variant>
//#include <source_location>


#include <SerAr/Core/OutputArchive.h>
#include <SerAr/TOML/TOML_Common.hpp>

#include <iosfwd>
namespace SerAr
{
    using namespace Archives;

    struct TOML_OutputArchive_Options {
        std::ios_base::openmode     mode{std::ios::trunc};
    };

    class TOML_OutputArchive : public OutputArchive<TOML_OutputArchive>
    {
        friend class OutputArchive<TOML_OutputArchive>;
        using ThisClass = TOML_OutputArchive;

    public:
        using Options = TOML_OutputArchive_Options;

        TOML_OutputArchive(const std::filesystem::path& path, const Options& opt = TOML_OutputArchive_Options{});
        ~TOML_OutputArchive() noexcept;

        template<typename T> 
        requires (TOML::IsTOMLValue<std::remove_cvref_t<T>>)
        inline ThisClass& save(const NamedValue<T>& nval)
        {
            auto& current_table = value_stack.top();
            current_table[nval.name] = toml::value(nval.val);
            return *this;
        }
        template<typename T> 
        requires (TOML::IsTOMLValue<std::remove_cvref_t<T>>)
        inline ThisClass& save(const T& val)
        {
            auto& current_table = value_stack.top();
            if(current_table.is_uninitialized()) {
                current_table = std::move(toml::value(val));
            } else {
                current_table.push_back(val); //Otherwise assume array
            }
            return *this;
        }

        template<typename T> 
        requires ( !TOML::IsTOMLValue<std::remove_cvref_t<T>> 
                && !stdext::is_container_v<std::remove_cvref_t<T>>)
        inline ThisClass& save(const NamedValue<T>& nval)
        {
            auto& current_table = value_stack.top();
            value_stack.push(toml::value());
            this->operator()(nval.val);
            current_table[nval.name] = value_stack.top();
            value_stack.pop();
            return *this;
        }
        template<typename T> 
        requires (!TOML::IsTOMLValue<std::remove_cvref_t<T>> 
                && stdext::is_container_v<std::remove_cvref_t<T>>)
        inline ThisClass& save(const NamedValue<T>& nval)
        {
            auto& current_table = value_stack.top();
            toml::array val_array;
            for (const auto & element : nval.val) {
                value_stack.push(toml::value());
                this->operator()(element);
                val_array.push_back(value_stack.top());
                value_stack.pop();
            }
            current_table[nval.name] = std::move(val_array);
            return *this;
        }


        template<typename T> 
        requires (!TOML::IsTOMLValue<std::remove_cvref_t<T>> 
                && stdext::is_container_v<std::remove_cvref_t<T>>)
        inline ThisClass& save(const T& val)
        {
            auto& current_table = value_stack.top();
            if (current_table.is_uninitialized()) {
                current_table = std::move(toml::array());
            }
            for (const auto & element : val) {
                value_stack.push(toml::value());
                this->operator()(element);
                current_table.push_back(value_stack.top());
                value_stack.pop();
            }
            return *this;
        }

#ifdef EIGEN_CORE_H
        template<typename T> requires(stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
            inline ThisClass& save(const Eigen::MatrixBase<T>& value)
        {
            using DataType = typename T::Scalar;
            const auto size = value.size();
            auto& current_table = value_stack.top();
            if (current_table.is_uninitialized()) {
                current_table = std::move(toml::array{});
            }
            if constexpr (T::IsVectorAtCompileTime) {
                std::vector<DataType> tmp(static_cast<typename std::vector<DataType>::size_type>(size));
                Eigen::Map< T, Eigen::Unaligned>(tmp.data(), value.rows(), value.cols()) = value;
                current_table.push_back(tmp);
            }
            else {
                const auto cols = value.cols();
                std::vector<DataType> tmp(cols);
                for (int i = 0; i < value.rows(); ++i)
                {
                    Eigen::Matrix<DataType,1, T::ColsAtCompileTime> rowmat = value.row(i);
                    Eigen::Map< decltype(rowmat), Eigen::Unaligned>(tmp.data(), 1, value.cols()) = rowmat;
                    current_table.push_back(tmp);
                }
            }
            return *this;
        }
#endif

    private:
        std::unique_ptr<std::ofstream> fileptr{nullptr};
        const Options options{};
        std::stack<toml::value> value_stack;
    };

    #define TOML_ARCHIVE_SAVE(type) \
        extern template TOML_OutputArchive& TOML_OutputArchive::save< type &>(const NamedValue< type &> &); \
        extern template TOML_OutputArchive& TOML_OutputArchive::save< const type &>(const NamedValue< const type &> &); \
        extern template TOML_OutputArchive& TOML_OutputArchive::save< type >(const NamedValue< type > &); \
        extern template TOML_OutputArchive& TOML_OutputArchive::save< const type >(const NamedValue< const type > &); 
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
    #undef TOML_ARCHIVE_SAVE
}

