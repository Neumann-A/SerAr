#pragma once

#include <array>
#include <vector>

#include <Eigen/Core>

#include <SerAr/Core/NamedValue.h>

struct othertest {
    double mydouble {5.35116151};
    std::vector<std::string> myvector{"a","b","c"};
};

struct test
{
    int myint = 3;
    std::array<double, 3> myarray {3,5,6};
    std::string mystring {"testing"};
    std::vector<othertest> myvecnested{ othertest{},othertest{} };
    othertest mynested;
};

template<SerAr::IsArchive Archive>
void serialize(othertest& val, Archive& ar) {
    ar(Archives::createNamedValue("mydouble",val.mydouble));
    ar(Archives::createNamedValue("myvector",val.myvector));
};

template<SerAr::IsArchive Archive>
void serialize(test& val, Archive& ar) {
    ar(Archives::createNamedValue("myint",val.myint));
    ar(Archives::createNamedValue("myarray",val.myarray));
    ar(Archives::createNamedValue("mystring",val.mystring));
    ar(Archives::createNamedValue("mynested",val.mynested));
    ar(Archives::createNamedValue("myvecnested", val.myvecnested));
};
