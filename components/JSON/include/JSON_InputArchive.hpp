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

namespace SerAr
{
    using namespace Archives;
    using JSONType = typename nlohmann::json;
    using JSONPointerType = typename nlohmann::json_pointer<JSONType>;

    namespace JSON::detail {

        template<typename Json, typename T>
        concept IsJSONLoadable = requires (Json& j, std::remove_cvref_t<T> value) {
            value = j.template get<std::remove_cvref_t<T>>();
        };
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
        template<typename T> requires (JSON::detail::IsJSONLoadable<JSONType, T> && !stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
        inline ThisClass& load(T& value)
        {
            value = json[json_pointer].get<std::remove_cvref_t<T>>();
            return *this;
        }
        template<typename T> requires (JSON::detail::IsJSONLoadable<JSONType, T>)
        inline ThisClass& load(NamedValue<T>& nval)
        {
            json_pointer.push_back(nval.name);
            nval.val = json[json_pointer].get<std::remove_cvref_t<T>>();
            json_pointer.pop_back();
            return *this;
        }
        template<typename T> requires (!JSON::detail::IsJSONLoadable<JSONType, T>)
        inline ThisClass& load(NamedValue<T>& nval)
        {
            json_pointer.push_back(nval.name);
            this->operator()(nval.val)
            json_pointer.pop_back();
            return *this;
        }
        JSONType json {};
    private:
        const Options options{};
        JSONPointerType json_pointer {"/"};
    };
}
