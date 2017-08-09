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

#include <cassert>
#include <filesystem>
#include <utility>
#include <string_view>

#include <iostream>

#include "basics/BasicMacros.h"

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
	namespace std
	{
		//silly hack to use the correct namespace for filesystem ;)
		using namespace ::std;
		namespace filesystem
		{
			using namespace ::std::experimental::filesystem;
		}
	}

	//Forward Declarations
	class HDF5_FileWrapper;
	class HDF5_GroupWrapper;
	class HDF5_DatasetWrapper;
	class HDF5_DataspaceWrapper;
	class HDF5_AttributeWrapper;
	class HDF5_DatatypeWrapper;
	class HDF5_LocationWrapper;
	template<typename T>
	class HDF5_GeneralLocation;
	template<typename T>
	struct HDF5_OpenCreateCloseWrapper;


	struct HDF5_DummyOptions{};
	struct HDF5_GeneralOptions
	{
		enum class HDF5_Access { ReadOnly, ReadWrite };
		enum class HDF5_Creation { Exclusive, Overwrite };
		enum class HDF5_Mode { CreateOrOverwrite, CreateOrOpen, Open, Create };
		enum class HDF5_Datatype { Native, STD_LE, STD_BE, IEEE_LE, IEEE_BE };

		HDF5_Mode mode{ HDF5_Mode::CreateOrOpen };
		HDF5_Creation creation_property{ HDF5_Creation::Exclusive };
		HDF5_Access access_property{ HDF5_Access::ReadWrite };
		HDF5_Datatype default_memory_datatyp{ HDF5_Datatype::Native };
		HDF5_Datatype default_storage_datatyp{ HDF5_Datatype::Native };

		hid_t creation_propertylist{ H5P_DEFAULT };
		hid_t access_propertylist{ H5P_DEFAULT };

	};
	struct HDF5_FileOptions : HDF5_GeneralOptions
	{
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
	struct HDF5_GroupOptions : HDF5_GeneralOptions  
	{
		hid_t link_creation_propertylist{ H5P_DEFAULT };
		hid_t getLinkCreationFlags() const noexcept
		{
			return link_creation_propertylist;
		}
		hid_t getCreationFlags() const noexcept
		{
			return creation_propertylist;
		}
		hid_t getAccessFlags() const noexcept
		{
			return access_propertylist;
		};
	};
	struct HDF5_DatasetOptions
	{
		hid_t datatype_id;
		hid_t dataspace_id;

		hid_t link_creation_propertylist{ H5P_DEFAULT };
		hid_t creation_propertylist{ H5P_DEFAULT };
		hid_t access_propertylist{ H5P_DEFAULT };
	};
	struct HDF5_DataspaceOptions 
	{
		H5S_class_t type{ H5S_SCALAR };
		//A scalar dataspace, H5S_SCALAR, has a single element, though that element may be of a complex datatype, such as a compound or array datatype.
		//A simple dataspace, H5S_SIMPLE, consists of a regular array of elements.
		//A null dataspace, H5S_NULL, has no data elements.
	};

	struct HDF_GeneralLocationOptions
	{
		hid_t link_access_properties{ H5P_DEFAULT };
	};


	class HDF5_LocationWrapper
	{
	private:
		const hid_t _locID;

		inline bool isValid() const noexcept { return (_locID < 0) ? false : true; };
	public:
		DISALLOW_COPY_AND_ASSIGN(HDF5_LocationWrapper)
		inline explicit HDF5_LocationWrapper(hid_t locID) : _locID(std::move(locID))
		{
			if (!isValid()) {
				throw std::runtime_error{ "Invalid HDF5 location." };
			};
		};

		inline const hid_t& getLoc() const noexcept { return _locID; };

		inline operator hid_t() const { return _locID; }; //Implicit Conversion Operator! 
	};

	template<typename T>
	struct HDF5_OptionsWrapper;
	template<>
	struct HDF5_OptionsWrapper<HDF5_FileWrapper> { using type = HDF5_FileOptions; };
	template<>
	struct HDF5_OptionsWrapper<HDF5_GroupWrapper> { using type = HDF5_GroupOptions; };
	template<>
	struct HDF5_OptionsWrapper<HDF5_DatasetWrapper> { using type = HDF5_DatasetOptions; };
	template<>
	struct HDF5_OptionsWrapper<HDF5_DataspaceWrapper> { using type = HDF5_DataspaceOptions; };
	template<>
	struct HDF5_OptionsWrapper<HDF5_AttributeWrapper> { using type = HDF5_DummyOptions; };
	template<>
	struct HDF5_OptionsWrapper<HDF5_DatatypeWrapper> { using type = HDF5_DummyOptions; };

	template<typename T>
	class HDF5_GeneralLocation
	{
	private:
		HDF5_LocationWrapper mLoc;
	public:
		using Base = T;
		HDF5_GeneralLocation() : mLoc(0) {};
		explicit HDF5_GeneralLocation(hid_t locID) : mLoc(std::move(locID)) {};
		explicit HDF5_GeneralLocation(HDF5_LocationWrapper locID) : mLoc(std::move(locID)) {};

		~HDF5_GeneralLocation() noexcept
		{		
			try
			{
				HDF5_OpenCreateCloseWrapper<T>::close(mLoc);
			}
			catch (...)
			{
				std::cerr << "HDF5: Some error while destructing: " << typeid(*this).name() << "\n";
			}
		}

		auto& getLocation() const noexcept { return mLoc; };

		template<typename U>
		U openOrCreateLocation(const std::filesystem::path& path,const typename HDF5_OptionsWrapper<U>::type& options)
		{			
			U newLocation{ HDF5_OpenCreateCloseWrapper<U>::openOrCreate(*this,path,options) };
			return newLocation;
		}
	};

	template<typename T>
	struct HDF5_OpenCreateCloseWrapper
	{
		static hid_t openOrCreate(const std::filesystem::path& path, const typename HDF5_OptionsWrapper<T>::type &options = typename HDF5_OptionsWrapper<T>::type{})
		{
			if constexpr (std::is_same_v<HDF5_FileWrapper, T>)
			{
				switch (options.mode)
				{
				case HDF5_GeneralOptions::HDF5_Mode::CreateOrOverwrite: case HDF5_GeneralOptions::HDF5_Mode::Create:
					return H5Fcreate(path.string().c_str(), options.getCreationFlags(), options.creation_propertylist, options.getAccessFlags());
				case HDF5_GeneralOptions::HDF5_Mode::CreateOrOpen:
				{
					const auto test = H5Fopen(path.string().c_str(), options.getAccessFlags(), options.access_propertylist);
					if (test < 0)
						return H5Fcreate(path.string().c_str(), options.getCreationFlags(), options.creation_propertylist, options.getAccessFlags());
					return test;
				}
				case HDF5_GeneralOptions::HDF5_Mode::Open:
					return H5Fopen(path.string().c_str(), options.getAccessFlags(), options.access_propertylist);
				}
				assert(false); // Programming error;
				return (hid_t)(-1);
			}
			else if constexpr (std::is_same_v<HDF5_DataspaceWrapper, T>)
			{
				switch (options.mode)
				{
				case HDF5_GeneralOptions::HDF5_Mode::Create:
					return H5Screate(options.type);
				}
				assert(false); // Programming error;
				return (hid_t)(-1);
			}
			assert(false); // Programming error;
			return (hid_t)(-1);
		};

		template<typename U>
		static hid_t openOrCreate(const HDF5_GeneralLocation<U>& loc, const std::filesystem::path& path, const typename HDF5_OptionsWrapper<T>::type &options = typename HDF5_OptionsWrapper<T>::type{})
		{
			if constexpr (std::is_same_v<HDF5_GroupWrapper, T>)
			{
				switch (options.mode)
				{
				case HDF5_GeneralOptions::HDF5_Mode::CreateOrOpen:
					const auto test = H5Gopen(loc.getLocation(), path.string().c_str(), options.access_propertylist);
					if (test < 0)
						return H5Gcreate(loc.getLocation, path.string().c_str(), options.link_creation_propertylist, options.creation_propertylist, options.access_propertylist);
					return test;
				case HDF5_GeneralOptions::HDF5_Mode::Open:
					return H5Gopen(loc.getLocation(), path.string().c_str(), options.access_propertylist);
				case HDF5_GeneralOptions::HDF5_Mode::Create:
					return H5Gcreate(loc.getLocation, path.string().c_str(), options.link_creation_propertylist, options.creation_propertylist, options.access_propertylist);
				default:
					assert(false); 
					return (hid_t)(-1);
				}
			}
			else if constexpr (std::is_same_v<HDF5_DatatypeWrapper, T>)
			{
				static_assert(!std::is_same_v<HDF5_DatatypeWrapper, T>, "hat are you doing? Dont define your own Datatypes!");
				assert(false); // Dont use your own Datatypes! Use predefined ones!
				return (hid_t)(-1);
			}
			else if constexpr (std::is_same_v<HDF5_AttributeWrapper, T>)
			{

			}
			else if constexpr (std::is_same_v<HDF5_DatasetWrapper, T>)
			{

			}
			assert(false); // Programming error;
			return (hid_t)(-1);
		}

		static herr_t close(HDF5_LocationWrapper& mLoc)
		{
			if constexpr (std::is_same_v<HDF5_FileWrapper, T>)
			{
				return H5Fclose(mLoc);
			}
			else if constexpr (std::is_same_v<HDF5_GroupWrapper, T>)
			{
				return H5Gclose(mLoc);
			}
			else if constexpr (std::is_same_v<HDF5_DatasetWrapper, T>)
			{
				return H5Dclose(mLoc);
			}
			else if constexpr (std::is_same_v<HDF5_DataspaceWrapper, T>)
			{
				return H5Sclose(mLoc);
			}
			else if constexpr (std::is_same_v<HDF5_AttributeWrapper, T>)
			{
				return H5Aclose(mLoc);
			}
			else if constexpr (std::is_same_v<HDF5_DatatypeWrapper, T>)
			{
				return H5Tclose(mLoc);
			}
		}
	};

	class HDF5_DatasetWrapper : HDF5_GeneralLocation<HDF5_DatasetWrapper>
	{
	public:
	};

	class HDF5_DataspaceWrapper : HDF5_GeneralLocation<HDF5_DataspaceWrapper>
	{
	public:
	};

	class HDF5_GroupWrapper : HDF5_GeneralLocation<HDF5_GroupWrapper>
	{
	public:
		HDF5_GroupWrapper(hid_t loc) : HDF5_GeneralLocation<HDF5_GroupWrapper>(std::move(loc)) {};
	};

	class HDF5_AttributeWrapper : HDF5_GeneralLocation<HDF5_AttributeWrapper>
	{
	public:
	};

	class HDF5_DatatypeWrapper : HDF5_GeneralLocation<HDF5_DatatypeWrapper>
	{
	public:
	};

	class HDF5_FileWrapper :HDF5_GeneralLocation<HDF5_FileWrapper>
	{
		using ThisClass = HDF5_FileWrapper;
	private:
		//std::experimental::filesystem::path _filepath;
		HDF5_FileOptions moptions;
		
		hid_t openOrCreateFile(const std::filesystem::path &path, const HDF5_FileOptions& options)
		{
			if (!path.has_filename())
				throw ::std::runtime_error{ "Cannot Open/Create HDF5 file. Missing filename!" };

			return HDF5_OpenCreateCloseWrapper<ThisClass>::openOrCreate(path,options);
		}

	public:
		explicit HDF5_FileWrapper(const std::filesystem::path &path, const HDF5_FileOptions& options)
			: HDF5_GeneralLocation<HDF5_FileWrapper>(openOrCreateFile(path, options)), moptions(options)
		{};

		~HDF5_FileWrapper() noexcept
		{ };


	};


}



#endif	// INC_HDF5_FileWrapper_H
// end of HDF5_Archive\HDF5_FileWrapper.h
///---------------------------------------------------------------------------------------------------
