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
    using JSONType = typename nlohmann::json;

    namespace JSON::detail {

        template<typename Json, typename T>
        concept IsJSONStoreable = requires (Json& j, std::remove_cvref_t<T> value) {
            j["dummy"] = value;
        };
    }


    struct JSON_OutputArchive_Options {
        std::streamsize indent_spaces{ 4 };
    };

    class JSON_OutputArchive : public OutputArchive<JSON_OutputArchive>
    {
        friend class OutputArchive<JSON_OutputArchive>;
        using ThisClass = JSON_OutputArchive;

    public:
        using Options = JSON_OutputArchive_Options;

        JSON_OutputArchive(const Options& opt, const std::filesystem::path& path);
        ~JSON_OutputArchive() noexcept;
        template<typename T> requires (JSON::detail::IsJSONStoreable<JSONType, T>)
            inline ThisClass& save(const T& value)
        {
            auto& current_json = json_stack.top();
            current_json.push_back(value);
            return *this;
        }
        template<typename T> requires (JSON::detail::IsJSONStoreable<JSONType, T>)
            inline ThisClass& save(const NamedValue<T>& nval)
        {
            auto& current_json = json_stack.top();
            current_json[nval.name] = nval.val;
            return *this;
        }

        template<typename T> requires (!SerAr::IsTypeSaveable<T, ThisClass> && !JSON::detail::IsJSONStoreable<JSONType, T> && !stdext::is_container_v<std::remove_cvref_t<T>>)
            inline ThisClass& save(const T& value)
        {
            json_stack.push(JSONType{});
            this->operator()(value);                // Fill the JSON object
            const auto current_json = json_stack.top(); // Get filled JSON
            json_stack.pop();
            auto& parrent_json = json_stack.top();
            parrent_json.push_back = std::move(current_json); // Insert filled JSON into parrent. 
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
        template<typename T> requires(stdext::is_eigen_type_v<T>)
            inline ThisClass& save(const Eigen::EigenBase<T>& value)
        {
            using DataType = typename T::Scalar;
            const auto size = value.size();
            JSONType matrix{};
            if constexpr (EigenMatrix::IsVectorAtCompileTime) {
                std::vector<DataType> tmp(size);
                if constexpr (T::IsRowMajor)
                {
                    Eigen::Map< T, Eigen::Unaligned, Eigen::Stride<1, T::ColsAtCompileTime> >(tmp.data(), value.rows(), value.cols()) = value;
                }
                else
                {
                    Eigen::Map< T, Eigen::Unaligned>(dataposition, value.rows(), value.cols()) = value;
                }
                matrix.push_back(tmp);
            }
            else {
                
                const auto rows = value.rows();
                std::vector<DataType> tmp(rows);
                for (int i = 0; i < rows; ++i)
                {
                    JSONType row{};
                    if constexpr (T::IsRowMajor)
                    {
                        Eigen::Map< T, Eigen::Unaligned, Eigen::Stride<1, T::ColsAtCompileTime> >(tmp.data(), value.rows(), value.cols()) = value;
                    }
                    else
                    {
                        Eigen::Map< T, Eigen::Unaligned>(tmp.data(), value.rows(), value.cols()) = value;
                    }
                    row.push_back(tmp);
                }
                JSONType matrix{};

            }
            const auto child_json = json_stack.top(); // Get filled JSON
            json_stack.pop();
            parrent_json.push_back(std::move(child_json));
            return *this;
        }
#endif
    private:
        const Options options{};
        std::unique_ptr<std::ofstream> pstr {nullptr};
        std::stack<JSONType> json_stack;
    };

    //void throw_runtime_error(std::string_view,const std::source_location& loc = std::source_location::current())
    void throw_runtime_error(std::string_view);
}
