#pragma once

#include <string>
#include <MyCEL/types/metainfo.hpp>
#include <SerAr/Core/NamedValue.h>

namespace MyCEL {

    template<typename Archive>
    void serialize(build_metainfo& info,Archive& ar) {
        ar(SerAr::createNamedValue("date",std::string(info.date)));
        ar(SerAr::createNamedValue("time",std::string(info.time)));
    }

    template<typename Archive>
    void serialize(git_metainfo& info,Archive& ar) {
        ar(SerAr::createNamedValue("branch",std::string(info.branch)));
        ar(SerAr::createNamedValue("sha",std::string(info.sha)));
        ar(SerAr::createNamedValue("description",std::string(info.describe)));
    }
}
