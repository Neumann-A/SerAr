#include <SerAr/SerAr.hpp>
#include <iostream>
#include <array>
#include <algorithm>
#include <ranges>


#include <MyCEL/basics/enumhelpers.h>
enum class testenum {value1, value2, value3};

int main()
{
    using namespace Archives;
    using namespace SerAr;
    // constexpr auto bool_array = bool_array_available_archives<AllArchiveTypeEnums,is_output_archive_available>();
    // constexpr auto size = count_available_archives<AllArchiveTypeEnums,is_output_archive_available>();
    //static_assert(count_available_if<AllArchiveTypeEnums,is_output_archive_available>()==3);
    //static_assert(!(count_available_if<AllArchiveTypeEnums,is_output_archive_available>()==4));

    for( std::size_t i = 0; i < AllArchiveTypeEnums.size(); ++i)
    {
        std::cout << ArchiveTypeEnumMap[AllArchiveTypeEnums[i]] << std::endl;
    }

    std::cout << "Available output:" << std::endl;
    for(const auto& elem : all_available_output_archives)
    {
        std::cout << ArchiveTypeEnumMap[elem] << std::endl;
    }
    std::cout << "Available input:" << std::endl;
    for(const auto& elem : all_available_input_archives)
    {
        std::cout << ArchiveTypeEnumMap[elem] << std::endl;
    }
    return 0;
}
