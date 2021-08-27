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
#include <SerAr/Core/OutputArchive.h>

#include <nlohmann/json.hpp>
namespace SerAr
{
    using namespace Archives;
    using JSONType = nlohmann::json;

    namespace JSON::detail {

        template<typename Json, typename T>
        concept IsJSONStoreable = !stdext::is_eigen_type_v<std::remove_cvref_t<T>> && requires (Json& j, std::remove_cvref_t<T> value) {
            j["dummy"] = value;
        };
    }

    struct JSON_OutputArchive_Options {
        int indent_spaces{ 4 };
        std::ios_base::openmode     mode{std::ios::trunc};
    };

    class JSON_OutputArchive : public OutputArchive<JSON_OutputArchive>
    {
        friend class OutputArchive<JSON_OutputArchive>;
        using ThisClass = JSON_OutputArchive;

    public:
        using Options = JSON_OutputArchive_Options;

        JSON_OutputArchive(const std::filesystem::path& path, const Options& opt = JSON_OutputArchive_Options{});
        ~JSON_OutputArchive() noexcept;
        template<typename T> requires (JSON::detail::IsJSONStoreable<JSONType, T> && !stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
            inline ThisClass& save(const T& value)
        {
            auto& current_json = json_stack.top();
            current_json = value;
            return *this;
        }
        template<typename T> requires (JSON::detail::IsJSONStoreable<JSONType, T>)
        inline ThisClass& save(const NamedValue<T>& nval)
        {
            auto& current_json = json_stack.top();
            current_json[nval.name] = nval.val;
            return *this;
        }

        template<typename T> requires (!SerAr::IsTypeSaveable<T, ThisClass> && !stdext::is_eigen_type_v<std::remove_cvref_t<T>> && !JSON::detail::IsJSONStoreable<JSONType, T> && !stdext::is_container_v<std::remove_cvref_t<T>>)
            inline ThisClass& save(const T& value)
        {
            json_stack.push(JSONType{});
            this->operator()(value);                // Fill the JSON object
            const auto current_json = json_stack.top(); // Get filled JSON
            json_stack.pop();
            auto& parrent_json = json_stack.top();
            parrent_json.push_back(std::move(current_json)); // Insert filled JSON into parrent. 
            return *this;
        }

        template<typename T> requires (!JSON::detail::IsJSONStoreable<JSONType, T> && !stdext::is_container_v<std::remove_cvref_t<T>>)
            inline ThisClass& save(const NamedValue<T>& nvalue)
        {
            json_stack.push(JSONType{});
            this->operator()(nvalue.val);                // Fill the JSON object
            const auto current_json = json_stack.top(); // Get filled JSON
            json_stack.pop();
            auto& parrent_json = json_stack.top();
            parrent_json[nvalue.name] = std::move(current_json); // Insert filled JSON into parrent. 
            return *this;
        }
        template<typename T> requires (!JSON::detail::IsJSONStoreable<JSONType, T>
            && stdext::is_container_v<std::remove_cvref_t<T>>)
            inline ThisClass& save(const T& value)
        {
            JSONType current_json{};
            for (const auto& element : value) {
                json_stack.push(JSONType{});
                this->operator()(element);                // Fill the JSON object
                const auto child_json = json_stack.top(); // Get filled JSON
                json_stack.pop();
                current_json.push_back(std::move(child_json));
            }
            auto& parrent_json = json_stack.top();
            parrent_json.push_back(std::move(current_json));
            return *this;
        }
        template<typename T> requires (!JSON::detail::IsJSONStoreable<JSONType, T>
            && stdext::is_container_v<std::remove_cvref_t<T>>)
            inline ThisClass& save(const NamedValue<T>& value)
        {
            auto& parrent_json = json_stack.top();
            for (const auto& element : value.val) {
                json_stack.push(JSONType{});
                this->operator()(element);                // Fill the JSON object
                const auto child_json = json_stack.top(); // Get filled JSON
                json_stack.pop();
                parrent_json[value.name].push_back(std::move(child_json));
            }
            return *this;
        }
#ifdef EIGEN_CORE_H
        template<typename T> requires(stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
            inline ThisClass& save(const Eigen::MatrixBase<T>& value)
        {
            using DataType = typename T::Scalar;
            const auto size = value.size();
            auto& parrent_json = json_stack.top();

            if constexpr (T::IsVectorAtCompileTime) {
                std::vector<DataType> tmp(static_cast<typename std::vector<DataType>::size_type>(size));
                Eigen::Map< T, Eigen::Unaligned>(tmp.data(), value.rows(), value.cols()) = value;
                parrent_json.push_back(tmp);
            }
            else {
                //JSONType matrix{};
                const auto cols = value.cols();
                std::vector<DataType> tmp(cols);
                for (int i = 0; i < value.rows(); ++i)
                {
                    Eigen::Matrix<DataType,1, T::ColsAtCompileTime> rowmat = value.row(i);
                    Eigen::Map< decltype(rowmat), Eigen::Unaligned>(tmp.data(), 1, value.cols()) = rowmat;
                    //matrix.push_back(tmp);
                    parrent_json.push_back(std::move(tmp));
                }
                //parrent_json.push_back(std::move(matrix));
            }
            return *this;
        }
#endif
    private:
        const Options options{};
        std::unique_ptr<std::ofstream> pstr {nullptr};
        std::stack<JSONType> json_stack{};
    };

    #define JSON_ARCHIVE_SAVE(type) extern template JSON_OutputArchive& JSON_OutputArchive::save< type &>(const NamedValue< type &> &);
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
