#include <filesystem>

#include <array>
#include <vector>

#include <Eigen/Core>

#include <SerAr/Core/NamedValue.h>
#include <SerAr/JSON/JSON_OutputArchive.hpp>
#include <SerAr/JSON/JSON_InputArchive.hpp>


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

int main()
{
    using Archive = Archives::JSON_OutputArchive;
    using ArchiveRead = Archives::JSON_InputArchive;
    std::filesystem::path   path{"test.json"};
    {
        Archive ar {{},path};
        test mytest;
        ar(Archives::createNamedValue("mytest",mytest));
    }
    {
        ArchiveRead ar {{},path};
        test mytest {.myint=0};
        ar(Archives::createNamedValue("mytest",mytest));
    }
    path = { "test2.json" };
    {
        Archive ar{ {},path };
        test mytest;
        ar(Archives::createNamedValue("mytest", std::vector{3,2,1}));
    }
    path = { "test3.json" };
    {
        Archive ar{ {},path };
        test mytest;
        ar(std::vector{3,2,1});
    }
    path = { "test4.json" };
    {
        Archive ar{ {},path };
        test mytest;
        std::vector{ std::vector{ 1,2,3 }, std::vector{ 4,5,6 }, std::vector{ 7,8,9 } };
        ar(std::vector{ std::vector{ 1,2,3 }, std::vector{ 4,5,6 }, std::vector{ 7,8,9 } });
    }
    path = { "test5.json" };
    {
        Archive ar{ {},path };
        test mytest;
        Eigen::Matrix<double,3,2> m;
        m << 1,2,3,4,5,6;
        ar(m);
    }
    static_assert(Archives::traits::has_type_save_v<othertest, Archives::JSON_OutputArchive>);
    static_assert(SerAr::IsTypeSaveable<othertest, Archives::JSON_OutputArchive>);
    static_assert(!SerAr::UseArchiveMemberSave<othertest, Archive>);
    static_assert(SerAr::UseTypeFunctionSerialize<othertest, Archive>);
    // static_assert(Archives::JSON::detail::IsPlainTypeJSONStoreable<Archives::JSONType,int>);
    // static_assert(stdext::is_container_v<std::remove_cvref_t<std::vector<othertest>&>>);
    // //static_assert(!stdext::is_container_v<std::vector<othertest>>);
    // static_assert(!Archives::JSON::detail::IsPlainTypeJSONStoreable<Archives::JSONType, std::vector<othertest>>);
    // static_assert(!Archives::JSON::detail::IsContainerJSONStoreable<Archives::JSONType, std::vector<othertest>>);
    // static_assert(!Archives::JSON::detail::IsPlainTypeJSONStoreable<Archives::JSONType, std::vector<othertest>> && stdext::is_container_v<std::vector<othertest>>);
    // static_assert(!Archives::JSON::detail::IsContainerJSONStoreable<Archives::JSONType, std::vector<othertest>> && stdext::is_container_v<std::vector<othertest>>);
}
