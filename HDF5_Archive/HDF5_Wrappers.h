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
#include <cstdint>

#include <type_traits>

#include <filesystem>
#include <utility>
#include <string_view>
#include <variant>

#include <iostream>

#include "basics/BasicMacros.h"

#include "stdext/std_extensions.h"

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

#include "HDF5_Type_Selector.h"

namespace HDF5_Wrapper
{
	////Remove this section when filesystme is moved out of experimental!
	//namespace std
	//{
	//	//silly hack to use the correct namespace for filesystem ;)
	//	using namespace ::std;
	//	namespace filesystem
	//	{
	//		using namespace ::std::experimental::filesystem;
	//	}
	//}

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
		enum class HDF5_Mode { CreateOrOverwrite, OpenOrCreate, Open, Create };
		HDF5_Mode mode{ HDF5_Mode::OpenOrCreate };

		};
	struct HDF5_FileOptions : HDF5_GeneralOptions
	{
		enum class HDF5_FileCreationFlags { Exclusive, Overwrite };
		enum class HDF5_FileAccess { ReadOnly, ReadWrite };
		HDF5_FileCreationFlags creation_property{ HDF5_FileCreationFlags::Exclusive };
		HDF5_FileAccess access_property{ HDF5_FileAccess::ReadWrite };
		hid_t creation_propertylist{ H5P_DEFAULT };
		hid_t access_propertylist{ H5P_DEFAULT };
		unsigned int getCreationFlags() const noexcept
		{
			switch (mode)
			{
			case HDF5_Mode::Create:
				return H5F_ACC_EXCL;
			case HDF5_Mode::CreateOrOverwrite:
				return H5F_ACC_TRUNC;
			default:
				std::runtime_error{ "Invalid file creation mode!" };
			};
			return 0;
		}
		unsigned int getAccessFlags() const noexcept
		{
			switch (access_property)
			{
			case HDF5_FileAccess::ReadOnly:
				return H5F_ACC_RDONLY;
			case HDF5_FileAccess::ReadWrite:
				return H5F_ACC_RDWR;
			default:
				std::runtime_error{ "Invalid access mode!" };

			};
			return 0;
		};
	};
	struct HDF5_GroupOptions : HDF5_GeneralOptions
	{
		hid_t creation_propertylist{ H5P_DEFAULT };
		hid_t access_propertylist{ H5P_DEFAULT };
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

		HDF5_Datatype default_memory_datatyp{ HDF5_Datatype::Native };
		HDF5_Datatype default_storage_datatyp{ HDF5_Datatype::Native };


		hid_t link_creation_propertylist{ H5P_DEFAULT };
		hid_t creation_propertylist{ H5P_DEFAULT };
		hid_t access_propertylist{ H5P_DEFAULT };
	};
	struct HDF5_DataspaceOptions 
	{
		H5S_class_t type{ H5S_SCALAR };
	};

	struct HDF_GeneralLocationOptions
	{
		hid_t link_access_properties{ H5P_DEFAULT };
	};


	class HDF5_LocationWrapper
	{
	private:
		hid_t mLocation;

		inline bool isValid() const noexcept { return (mLocation > 0); };
	public:
		DISALLOW_COPY_AND_ASSIGN(HDF5_LocationWrapper)
		ALLOW_DEFAULT_MOVE_AND_ASSIGN(HDF5_LocationWrapper)

		inline explicit HDF5_LocationWrapper(hid_t locID) : mLocation(std::move(locID))
		{
			if (!isValid()) {
				throw std::runtime_error{ "Invalid HDF5 location." };
			};
		};

		inline ~HDF5_LocationWrapper() noexcept = default;

		inline operator const hid_t&() const 
		{ 
			return mLocation; 
				
		}; //Implicit Conversion Operator! 

		inline std::string getHDF5Path() const noexcept
		{
			std::string res;
			
			auto size = H5Iget_name(mLocation, nullptr, 0);
			res.resize(size+1);

			H5Iget_name(mLocation, res.data(), size+1);
			
			return res;
		}

		inline std::string getHDF5Fullpath() const noexcept
		{
			std::string res;

			auto size = H5Fget_name(mLocation, nullptr, 0);
			res.resize(size + 1);

			H5Fget_name(mLocation, res.data(), size + 1);

			return res;
		}
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
		static_assert(std::is_same_v<T, std::decay_t<T>>);

		HDF5_LocationWrapper mLoc;
		bool wasMoved{ false };
	public:
		using Base = T;
		DISALLOW_COPY_AND_ASSIGN(HDF5_GeneralLocation)

	protected:
		explicit HDF5_GeneralLocation(HDF5_LocationWrapper &&locID) : mLoc(std::move(locID)) 
		{};
	public:	
		HDF5_GeneralLocation(HDF5_GeneralLocation&& rhs) : mLoc(std::move(rhs.mLoc))
		{
			rhs.wasMoved = true;
		}//Move Constructor
		HDF5_GeneralLocation& operator=(HDF5_GeneralLocation&& rhs)
		{
			this->mLoc = std::move(rhs.mLoc);
			rhs.wasMoved = true;
		}; //Move Assignment

		~HDF5_GeneralLocation() noexcept
		{		
			if (!wasMoved)
			try
			{
				
					HDF5_OpenCreateCloseWrapper<T>::close(mLoc);
			}
			catch (...)
			{
				std::cerr << "HDF5: Some error while destructing: " << typeid(*this).name() << "\n";
			}
		}

		const HDF5_LocationWrapper& getLocation() const noexcept { return mLoc; };

		template<typename U>
		U openOrCreateLocation(const std::experimental::filesystem::path& path,const typename HDF5_OptionsWrapper<U>::type& options)
		{			
			U newLocation{ HDF5_OpenCreateCloseWrapper<U>::openOrCreate(*this,path,options) };
			return newLocation;
		}
	};

	template<typename T>
	struct HDF5_OpenCreateCloseWrapper
	{
		static hid_t openOrCreate(const std::experimental::filesystem::path& path, const typename HDF5_OptionsWrapper<T>::type &options = typename HDF5_OptionsWrapper<T>::type{})
		{
			if constexpr (std::is_same_v<HDF5_FileWrapper, T>)
			{
				switch (options.mode)
				{
				case HDF5_GeneralOptions::HDF5_Mode::CreateOrOverwrite: case HDF5_GeneralOptions::HDF5_Mode::Create:
					return H5Fcreate(path.string().c_str(), options.getCreationFlags(), options.creation_propertylist, options.access_propertylist);
				case HDF5_GeneralOptions::HDF5_Mode::OpenOrCreate:
				{	const auto test = H5Fopen(path.string().c_str(), options.getAccessFlags(), options.access_propertylist);
					if (test < 0)
						return H5Fcreate(path.string().c_str(), options.getCreationFlags(), options.creation_propertylist, options.access_propertylist);
					return test;
				}
				case HDF5_GeneralOptions::HDF5_Mode::Open:
					return H5Fopen(path.string().c_str(), options.getAccessFlags(), options.access_propertylist);
				}
				assert(false); // Programming error;
			}
			else if constexpr (std::is_same_v<HDF5_DataspaceWrapper, T>)
			{
				switch (options.mode)
				{
				case HDF5_GeneralOptions::HDF5_Mode::Create:
					return H5Screate(options.type);
				}
				assert(false); // Programming error;
			}
			return -1;
		};

		template<typename U>
		static hid_t openOrCreate(const HDF5_GeneralLocation<U>& loc, const std::experimental::filesystem::path& path, const typename HDF5_OptionsWrapper<T>::type &options = typename HDF5_OptionsWrapper<T>::type{})
		{
			if constexpr (std::is_same_v<HDF5_GroupWrapper, T>)
			{
				switch (options.mode)
				{
				case HDF5_GeneralOptions::HDF5_Mode::OpenOrCreate:
				{
					const auto test = H5Gopen(loc.getLocation(), path.string().c_str(), options.access_propertylist);
					if (test < 0)
						return H5Gcreate(loc.getLocation(), path.string().c_str(), options.link_creation_propertylist, options.creation_propertylist, options.access_propertylist);
					return test;
				}
				case HDF5_GeneralOptions::HDF5_Mode::Open:
					return H5Gopen(loc.getLocation(), path.string().c_str(), options.access_propertylist);
				case HDF5_GeneralOptions::HDF5_Mode::Create:
					return H5Gcreate(loc.getLocation(), path.string().c_str(), options.link_creation_propertylist, options.creation_propertylist, options.access_propertylist);
				default:
					assert(false); 
					return (hid_t)(-1);
				}
			}
			else if constexpr (std::is_same_v<HDF5_DatatypeWrapper, T>)
			{
				static_assert(!std::is_same_v<HDF5_DatatypeWrapper, T>, "What are you doing? Dont define your own Datatypes! Use the predifined ones!");
				assert(false); // Dont use your own Datatypes! Use predefined ones!
				return (hid_t)(-1);
			}
			else if constexpr (std::is_same_v<HDF5_AttributeWrapper, T>)
			{

			}
			else if constexpr (std::is_same_v<HDF5_DatasetWrapper, T>)
			{

			}
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

	class HDF5_DatasetWrapper : public HDF5_GeneralLocation<HDF5_DatasetWrapper>
	{
	public:
	};

	class HDF5_DataspaceWrapper : public HDF5_GeneralLocation<HDF5_DataspaceWrapper>
	{
	public:
	};

	class HDF5_GroupWrapper : public HDF5_GeneralLocation<HDF5_GroupWrapper>
	{
		using ThisClass = HDF5_GroupWrapper;

		template<typename T>
		static HDF5_LocationWrapper openOrCreateFile(const HDF5_GeneralLocation<T> &loc ,const std::experimental::filesystem::path &path, const HDF5_GroupOptions& options = HDF5_GroupOptions{})
		{
			if (path.has_extension())
				throw ::std::runtime_error{ "Group path should not have an extension!" };

			if (path.has_filename())
				throw ::std::runtime_error{ "Group path should not have a filename!" };

			return HDF5_LocationWrapper(HDF5_OpenCreateCloseWrapper<ThisClass>::openOrCreate(loc, path, options));
		}
	public:
		template<typename T>
		HDF5_GroupWrapper(const HDF5_GeneralLocation<T> &loc, const std::experimental::filesystem::path &path, const HDF5_GroupOptions& options = HDF5_GroupOptions{})
			: HDF5_GeneralLocation<HDF5_GroupWrapper>(openOrCreateFile(loc,path,options)) {};
	};

	class HDF5_AttributeWrapper : public HDF5_GeneralLocation<HDF5_AttributeWrapper>
	{
	public:
	};

	class HDF5_DatatypeWrapper : public HDF5_GeneralLocation<HDF5_DatatypeWrapper>
	{
	public:
	};

	class HDF5_FileWrapper : public HDF5_GeneralLocation<HDF5_FileWrapper>
	{
		using ThisClass = HDF5_FileWrapper;
		//std::experimental::filesystem::path _filepath;
		HDF5_FileOptions moptions;
		
		static HDF5_LocationWrapper openOrCreateFile(const std::experimental::filesystem::path &path, const HDF5_FileOptions& options = HDF5_FileOptions{})
		{
			if (!path.has_filename())
				throw ::std::runtime_error{ "Cannot Open/Create HDF5 file. Missing filename!" };

			return HDF5_LocationWrapper(HDF5_OpenCreateCloseWrapper<ThisClass>::openOrCreate(path,options));
		}

	public:
		explicit HDF5_FileWrapper(const std::experimental::filesystem::path &path, const HDF5_FileOptions& options = HDF5_FileOptions{})
			: HDF5_GeneralLocation<HDF5_FileWrapper>(openOrCreateFile(path, options)), moptions(options)
		{};

		DISALLOW_COPY_AND_ASSIGN(HDF5_FileWrapper)
		ALLOW_DEFAULT_MOVE_AND_ASSIGN(HDF5_FileWrapper)
	};


}



#endif	// INC_HDF5_FileWrapper_H
// end of HDF5_Archive\HDF5_FileWrapper.h
///---------------------------------------------------------------------------------------------------
