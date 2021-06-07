#pragma once

#include "InputArchive.h"
#include "OutputArchive.h"

namespace Basic
{
    template<typename Archive>
    class TestIOArchive : public InputArchive<Archive>, public OutputArchive<Archive> 
    {
    public:

    private:
    protected:
    public:
    };

}