///---------------------------------------------------------------------------------------------------
// file:		HDF5_Archive\HDF5_FileWrapper.h
//
// summary: 	Declares the hdf 5 file wrapper class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander
// date: 07.07.2017

#ifndef INC_HDF5_FileWrapper_H
#define INC_HDF5_FileWrapper_H
///---------------------------------------------------------------------------------------------------
#pragma once

#include <filesystem>
#include <utility>

#ifdef _DEBUG
#ifdef H5_BUILT_AS_DYNAMIC_LIB
//#pragma comment(lib,"hdf5_hl_cpp_D.lib")
//#pragma comment(lib,"hdf5_cpp_D.lib")
//#pragma comment(lib,"hdf5_hl_D.lib")
#pragma comment(lib,"hdf5_D.lib")
//#pragma comment(lib,"szip.lib")
//#pragma comment(lib,"zlib.lib")
#else
//#pragma comment(lib,"libhdf5_hl_cpp_D.lib")
//#pragma comment(lib,"libhdf5_cpp_D.lib")
//#pragma comment(lib,"libhdf5_hl_D.lib")
#pragma comment(lib,"libhdf5_D.lib")
//#pragma comment(lib,"libszip.lib")
//#pragma comment(lib,"libzlib.lib")
#endif
#else
#ifdef H5_BUILT_AS_DYNAMIC_LIB
//#pragma comment(lib,"hdf5_hl_cpp.lib")
//#pragma comment(lib,"hdf5_cpp.lib")
//#pragma comment(lib,"hdf5_hl.lib")
#pragma comment(lib,"hdf5.lib")
//#pragma comment(lib,"szip.lib")
//#pragma comment(lib,"zlib.lib")
#else
//#pragma comment(lib,"libhdf5_hl_cpp.lib")
//#pragma comment(lib,"libhdf5_cpp.lib")
//#pragma comment(lib,"libhdf5_hl.lib")
#pragma comment(lib,"libhdf5.lib")
//#pragma comment(lib,"libszip.lib")
//#pragma comment(lib,"libzlib.lib")
#endif
#endif

#include <hdf5.h>
#include <hdf5_hl.h>

namespace HDF5_Wrapper
{
	struct HDF5_Options
	{
		enum class HDF5_Access {ReadOnly, ReadWrite};
		enum class HDF5_Creation {Exclusive, Overwrite};
		enum class HDF5_FileMode {CreateOrOverwrite, Open};

		HDF5_Access access_property { HDF5_Access::ReadWrite };
		HDF5_Creation creation_property{ HDF5_Creation::Exclusive };
		HDF5_FileMode mode{ HDF5_FileMode::CreateOrOverwrite };

		hid_t filecreation_propertylist{ H5P_DEFAULT};
		hid_t fileaccess_propertylist{ H5P_DEFAULT };

		hid_t getCreationFlags() const noexcept
		{
			hid_t res{ 0 };
			switch (creation_property)
			{
			case HDF5_Creation::Exclusive:
				res |= H5F_ACC_EXCL;
				break;
			case HDF5_Creation::Overwrite:
				res |= H5F_ACC_TRUNC;
				break;
			};
			return res;
		}
		hid_t getAccessFlags() const noexcept
		{
			hid_t res{ 0 };

			switch (access_property)
			{
			case HDF5_Access::ReadOnly:
				res |= H5F_ACC_RDONLY;
				break;
			case HDF5_Access::ReadWrite:
				res |= H5F_ACC_RDWR;
				break;
			};

			return res;
		};
	};

	class HDF5_LocationWrapper
	{
	private:
		const hid_t _locID;

		inline bool isValid() const noexcept { return (_locID < 0) ? false : true; };
	public:
		inline explicit HDF5_LocationWrapper(hid_t locID) : _locID(std::move(locID))
		{
			if (!isValid()) {
				throw std::runtime_error{ "Invalid HDF5 location." };
			};
		};

		inline const hid_t& getLoc() const noexcept { return _locID; };
	};

	class HDF5_DatasetWrapper
	{
		const HDF5_LocationWrapper _loc;
	public:
	};

	class HDF5_GroupWrapper
	{
		const HDF5_LocationWrapper _loc;
	public:
		HDF5_GroupWrapper();

	};


	class HDF5_FileWrapper
	{
	private:
		//std::experimental::filesystem::path _filepath;
		HDF5_Options _options;
		
		const HDF5_LocationWrapper _FileID;
		
		hid_t openOrCreateFile(const std::experimental::filesystem::path &path)
		{
			if (!path.has_filename())
				throw std::runtime_error{ "Cannot Open/Create HDF5 file. Missing filename!" };

			switch (_options.mode)
			{
			case HDF5_Options::HDF5_FileMode::CreateOrOverwrite:
				return H5Fcreate(path.string().c_str(),_options.getCreationFlags(),_options.filecreation_propertylist,_options.fileaccess_propertylist);
			case HDF5_Options::HDF5_FileMode::Open:
				return H5Fopen(path.string().c_str(), _options.getAccessFlags(), _options.fileaccess_propertylist);
			}
		}

	public:
		explicit HDF5_FileWrapper(const std::experimental::filesystem::path &path, HDF5_Options options) 
			: _options(std::move(options)), _FileID(openOrCreateFile(path))
		{

		};

		~HDF5_FileWrapper() noexcept
		{
			//We do not need to care if anything else of the HDF5 file is still open;
			//The HDF5 library takes care of this and handles it (basically still allowing access.) 
			H5Fclose(_FileID.getLoc());
		}

		
	};


}



#endif	// INC_HDF5_FileWrapper_H
// end of HDF5_Archive\HDF5_FileWrapper.h
///---------------------------------------------------------------------------------------------------
