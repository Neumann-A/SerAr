#pragma once

#include <string_view>

#include <MyCEL/stdext/is_string.h>
#include <MyCEL/stdext/is_container.h>
#include <MyCEL/stdext/is_eigen3_type.h>

#include <SerAr/Core/NamedValue.h>
#include <SerAr/Core/NamedValueWithDesc.h>

#include <fmt/format.h>

#include <toml.hpp>


namespace SerAr::TOML {
    void throw_runtime_error(std::string_view msg);


    template<typename T>
    concept IsTOMLNormalValue = 
        (!stdext::is_container_v<T> 
        || stdext::is_string_v<T> )
        && requires (T &&value) {
            { toml::value(std::forward<T>(value)) };
        };
    template<typename T>
    concept IsTOMLContainerValue = 
        stdext::is_container_v<T> 
        && (!stdext::is_container_v<std::remove_cvref_t<typename T::value_type>> 
            || stdext::is_string_v<std::remove_cvref_t<typename T::value_type>>)
        && !stdext::is_string_v<T>
        && requires (T &&value) {
            { toml::value(std::forward<T>(value)) };
            { toml::value(*value.begin()) };
        };

    template<typename T>
    concept IsTOMLValue = (IsTOMLNormalValue<T> || IsTOMLContainerValue<T>) && !stdext::is_eigen_type_v<std::remove_cvref_t<T>> ;


}