#include "Matlab_Common.hpp"

namespace SerAr {

    const char* MatlabHelper::getMatlabMode(const MatlabOptions& option) noexcept {
        const char* mode{ [&]()
        {switch (option)
        {
        case MatlabOptions::read:
            return "r";
        case MatlabOptions::update:
            return "u";
        case MatlabOptions::write:
            return "w";
        case MatlabOptions::write_v4:
            return "w4";
        case MatlabOptions::write_v6:
            return "w6";
        case MatlabOptions::write_local:
            return "wL";
        case MatlabOptions::write_v7:
            return "w7";
        case MatlabOptions::write_compressed:
            return "wz";
        case MatlabOptions::write_v73:
            return "w7.3";
        default:
            assert(false); //Invalid Enum!
            return "";
        }
        }() };
        return mode;
    }
}
