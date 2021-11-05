#pragma once

#include <MyCEL/types/metainfo.hpp>
#include <SerAr/Core/NamedValue.h>

namespace MyCEL {

    template<typename Archive>
    void serialize(build_metainfo& info,Archive& ar) {
        ar(SerAr::createNamedValue("date",info.date));
        ar(SerAr::createNamedValue("time",info.time));
    }

    template<typename Archive>
    void serialize(git_metainfo& info,Archive& ar) {
        ar(SerAr::createNamedValue("branch",info.branch));
        ar(SerAr::createNamedValue("sha",info.sha));
        ar(SerAr::createNamedValue("description",info.describe));
    }
}
