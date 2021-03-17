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
#include <SerAr/Core/InputArchive.h>

#include <nlohmann/json.hpp>

#ifdef __llvm__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsigned-enum-bitfield"
#endif
#include <fmt/format.h>
#ifdef __llvm__
#pragma clang diagnostic pop
#endif


namespace SerAr
{
    using namespace Archives;
    using JSONType = typename nlohmann::json;
    using JSONPointerType = typename nlohmann::json_pointer<JSONType>;

    namespace JSON::detail {

        template<typename Json, typename T>
        concept IsJSONLoadable = !stdext::is_eigen_type_v<std::remove_cvref_t<T>> &&  requires (Json& j, std::remove_cvref_t<T> value) {
            value = j.template get<std::remove_cvref_t<T>>();
        };

        template<typename Json, typename T>
        concept InputNamedValueJSONLoadable = IsJSONLoadable<Json, T> && IsNamedValue<T>;

        template<typename Json, typename T>
        concept InputNamedValueJSONNotLoadable = !IsJSONLoadable<Json, T> && IsNamedValue<T>;
    }


    struct JSON_InputArchive_Options {
    };

    class JSON_InputArchive : public InputArchive<JSON_InputArchive>
    {
        friend class InputArchive<JSON_InputArchive>;
        using ThisClass = JSON_InputArchive;

    public:
        using Options = JSON_InputArchive_Options;

        JSON_InputArchive(const Options& opt, const std::filesystem::path& path);
        template<typename T> requires (JSON::detail::IsJSONLoadable<JSONType, T>)
        inline ThisClass& load(T& value)
        {
            value = json[json_pointer].get<std::remove_cvref_t<T>>();
            return *this;
        }
        template<typename T> 
        requires (JSON::detail::InputNamedValueJSONLoadable < JSONType, T >)
        inline ThisClass& load(T&& nval)
        {
            using Type = std::remove_cvref_t<typename std::remove_cvref_t<T>::type>;
            json_pointer.push_back(nval.name);
            nval.val = json[json_pointer].get<Type>();
            json_pointer.pop_back();
            return *this;
        }
        template<typename T> 
        requires (JSON::detail::InputNamedValueJSONNotLoadable<JSONType, T>)
        inline ThisClass& load(T&& nval)
        {
            json_pointer.push_back(nval.name);
            this->operator()(nval.val);
            json_pointer.pop_back();
            return *this;
        }
        template<typename T> 
        requires (!JSON::detail::IsJSONLoadable<JSONType, T>
                  && stdext::is_container_v<std::remove_cvref_t<T>>
                  && std::default_initializable<typename std::remove_cvref_t<T>::value_type>)
        inline ThisClass& load(T&& value)
        {
            using Container = std::remove_cvref_t<T>;
            Container ret;
            typename Container::value_type tmp{};
            if (!json[json_pointer].is_array()) {
                const auto msg = fmt::format("Error: JSON member at '{}' is not an array!", json_pointer.to_string());
                throw std::runtime_error{ msg };
            }
            for (std::size_t i = 0; i < json[json_pointer].size(); i++) {
                json_pointer.push_back(fmt::format_int(i).c_str());
                this->operator()(tmp);
                ret.push_back(std::move(tmp));
                json_pointer.pop_back();
            }
            value = std::move(ret);
            return *this;
        }
#ifdef EIGEN_CORE_H
        template<typename T> requires(stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
        inline ThisClass& load(Eigen::MatrixBase<T>& value)
        {
            using DataType = typename T::Scalar;
            std::vector<DataType> res;

            if constexpr (T::IsVectorAtCompileTime) {               
                res = json[json_pointer].get<decltype(res)>();
                value = Eigen::Map< T, Eigen::Unaligned>(res.data(),res.size());
            }
            else {
                const auto rows = json[json_pointer].size();
                for (std::size_t i = 0; i < rows; ++i)
                {
                    json_pointer.push_back(fmt::format_int(i).c_str());
                    auto tmp = json[json_pointer].get<decltype(res)>();
                    json_pointer.pop_back();
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
        JSONType json {};
    private:
        const Options options{};
        JSONPointerType json_pointer {};
    };
}
