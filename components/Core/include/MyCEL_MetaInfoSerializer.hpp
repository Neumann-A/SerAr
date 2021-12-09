#pragma once

#include <string>
#include <MyCEL/types/metainfo.hpp>
#include <SerAr/Core/NamedValue.h>

namespace MyCEL {

    template<typename Archive>
    void serialize(build_metainfo& info,Archive& ar) {
        ar(Archives::createNamedValue("date",std::string(info.date)));
        ar(Archives::createNamedValue("time",std::string(info.time)));
    }

    template<typename Archive>
    void serialize(git_metainfo& info,Archive& ar) {
        ar(Archives::createNamedValue("branch",std::string(info.branch)));
        ar(Archives::createNamedValue("sha",std::string(info.sha)));
        ar(Archives::createNamedValue("description",std::string(info.describe)));
    }
}
