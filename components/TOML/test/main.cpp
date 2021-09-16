#include <filesystem>

#include <array>
#include <vector>

#include <Eigen/Core>

#include <SerAr/Core/NamedValue.h>
#include <SerAr/TOML/TOML_OutputArchive.hpp>
#include <SerAr/TOML/TOML_InputArchive.hpp>


struct othertest {
    double mydouble {5.35116151};
    std::vector<std::string> myvector{"a","b","c"};
};

struct test
{
    int myint = 3;
    std::array<double, 3> myarray {3.0,5.0,6.0};
    std::string mystring {"testing"};
    othertest mynested;
    std::vector<othertest> myvecnested{othertest{}, othertest{}};
};
static_assert(!SerAr::TOML::IsTOMLValue<std::vector<othertest>>);
static_assert(!SerAr::TOML::IsTOMLValue<std::vector<othertest>> 
                && stdext::is_container_v<std::vector<othertest>>);
static_assert(!SerAr::TOML::IsTOMLValue<Eigen::Matrix<double, 3, 2, 0>> );
static_assert(!stdext::is_container_v<Eigen::Matrix<double, 3, 2, 0>> );

template<SerAr::IsArchive Archive>
void serialize(othertest& val, Archive& ar) {
    ar(Archives::createNamedValue("mydouble",val.mydouble));
    ar(Archives::createNamedValue("myvector",val.myvector));
}
template<SerAr::IsArchive Archive>
void serialize(test& val, Archive& ar) {
    ar(Archives::createNamedValue("myint",val.myint));
    ar(Archives::createNamedValue("myarray",val.myarray));
    ar(Archives::createNamedValue("mystring",val.mystring));
    ar(Archives::createNamedValue("mynested",val.mynested));
    ar(Archives::createNamedValue("myvecnested", val.myvecnested));
}

int main()
{
    using Archive = Archives::TOML_OutputArchive;
    using ArchiveRead = Archives::TOML_InputArchive;
    std::filesystem::path   path{"test.toml"};
    {
        Archive ar {path, {}};
        test mytest;
        ar(Archives::createNamedValue("mytest",mytest));
    }
    {
        ArchiveRead ar {path, {}};
        test mytest{.myint = 0, .myarray = {0, 0, 0}, .mystring = "somethingdifferent", .mynested = {0.0, {"0"}}
        };
        ar(Archives::createNamedValue("mytest",mytest));
    }
    path = "test3.toml";
    {
        Archive ar{ path, {} };
        std::vector tmp{ std::vector{ 1,2,3 }, std::vector{ 4,5,6 }, std::vector{ 7,8,9 } };
        ar(Archives::createNamedValue("vecvec", tmp));
    }
    {
        ArchiveRead ar{ path, {} };
        std::vector tmp{ std::vector{ 0,0,0 }, std::vector{ 0,0,0 }, std::vector{ 0,0,0 } };
        ar(Archives::createNamedValue("vecvec", tmp));
    }
    //path = "test4.toml"; // Unsupported by TOML
    //{
    //    Archive ar{ path, {} };
    //    std::vector tmp{ std::vector{ 1,2,3 }, std::vector{ 4,5,6 }, std::vector{ 7,8,9 } };
    //    ar(tmp);
    //}
    //{
    //    ArchiveRead ar{ path, {} };
    //    std::vector tmp { std::vector{ 0,0,0 }, std::vector{ 0,0,0 }, std::vector{ 0,0,0 } };
    //    ar(tmp);
    //}
    path = "test5.toml";
    {
        Archive ar{ path, {} };
        Eigen::Matrix<double, 3, 2> m{ {1,2},{3,4},{5,6} };
        ar(Archives::createNamedValue("matrix", m));
    }
    {
        ArchiveRead ar{ path, {} };
        Eigen::Matrix<double, 3, 2> m { {0,0},{0,0},{0,0} };
        ar(Archives::createNamedValue("matrix", m));
    }
    //path = "test6.toml"; // Unsupported by TOML
    //{
    //    Archive ar{ path, {} };
    //    Eigen::Matrix<double, 3, 2> m{ {1,2},{3,4},{5,6} };
    //    ar(m);
    //}
    //{
    //    ArchiveRead ar{ path, {} };
    //    Eigen::Matrix<double, 3, 2> m { {0,0},{0,0},{0,0} };
    //    ar(m);
    //}
}
