///-------------------------------------------------------------------------------------------------
// file:	HDF5_Archive.cpp
//
// summary:	Implements the hdf 5 archive class
///-------------------------------------------------------------------------------------------------

#include "HDF5_Archive.h"
#include "HDF5_InputArchive.hpp"
#include "HDF5_OutputArchive.hpp"

namespace Archives {
    
    #define HDF5_ARCHIVE_LOAD(type) template void HDF5_InputArchive::load< type &>(NamedValue< type &> &);
    HDF5_ARCHIVE_LOAD(bool)
    HDF5_ARCHIVE_LOAD(short)
    HDF5_ARCHIVE_LOAD(unsigned short)
    HDF5_ARCHIVE_LOAD(int)
    HDF5_ARCHIVE_LOAD(unsigned int)
    HDF5_ARCHIVE_LOAD(long)
    HDF5_ARCHIVE_LOAD(unsigned long)
    HDF5_ARCHIVE_LOAD(long long)
    HDF5_ARCHIVE_LOAD(unsigned long long)
    HDF5_ARCHIVE_LOAD(double)
    HDF5_ARCHIVE_LOAD(float)
    HDF5_ARCHIVE_LOAD(std::string)
    HDF5_ARCHIVE_LOAD(std::vector<short>)
    HDF5_ARCHIVE_LOAD(std::vector<unsigned short>)
    HDF5_ARCHIVE_LOAD(std::vector<int>)
    HDF5_ARCHIVE_LOAD(std::vector<unsigned int>)
    HDF5_ARCHIVE_LOAD(std::vector<long>)
    HDF5_ARCHIVE_LOAD(std::vector<unsigned long>)
    HDF5_ARCHIVE_LOAD(std::vector<long long>)
    HDF5_ARCHIVE_LOAD(std::vector<unsigned long long>)
    HDF5_ARCHIVE_LOAD(std::vector<double>)
    HDF5_ARCHIVE_LOAD(std::vector<float>)
    HDF5_ARCHIVE_LOAD(std::vector<std::string>)
    #undef HDF5_ARCHIVE_LOAD

    #define HDF5_ARCHIVE_SAVE(type) template void HDF5_OutputArchive::save< type & >(const NamedValue< type &> &);
    HDF5_ARCHIVE_SAVE(bool)
    HDF5_ARCHIVE_SAVE(short)
    HDF5_ARCHIVE_SAVE(unsigned short)
    HDF5_ARCHIVE_SAVE(int)
    HDF5_ARCHIVE_SAVE(unsigned int)
    HDF5_ARCHIVE_SAVE(long)
    HDF5_ARCHIVE_SAVE(unsigned long)
    HDF5_ARCHIVE_SAVE(long long)
    HDF5_ARCHIVE_SAVE(unsigned long long)
    HDF5_ARCHIVE_SAVE(double)
    HDF5_ARCHIVE_SAVE(float)
    HDF5_ARCHIVE_SAVE(std::string)
    HDF5_ARCHIVE_SAVE(std::vector<short>)
    HDF5_ARCHIVE_SAVE(std::vector<unsigned short>)
    HDF5_ARCHIVE_SAVE(std::vector<int>)
    HDF5_ARCHIVE_SAVE(std::vector<unsigned int>)
    HDF5_ARCHIVE_SAVE(std::vector<long>)
    HDF5_ARCHIVE_SAVE(std::vector<unsigned long>)
    HDF5_ARCHIVE_SAVE(std::vector<long long>)
    HDF5_ARCHIVE_SAVE(std::vector<unsigned long long>)
    HDF5_ARCHIVE_SAVE(std::vector<double>)
    HDF5_ARCHIVE_SAVE(std::vector<float>)
    HDF5_ARCHIVE_SAVE(std::vector<std::string>)
    #undef HDF5_ARCHIVE_SAVE
}
