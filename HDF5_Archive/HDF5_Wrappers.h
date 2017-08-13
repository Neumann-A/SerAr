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
#include <limits>
#include <iostream>

#include "basics/BasicMacros.h"

#include "stdext/std_extensions.h"

#define H5_BUILT_AS_DYNAMIC_LIB

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
	using hdf5path = std::experimental::filesystem::path;
	using filepath = std::experimental::filesystem::path;
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
	class HDF5_GeneralType;
	template<typename T>
	struct HDF5_OpenCreateCloseWrapper;

	struct HDF5_DummyOptions {};
	struct HDF5_GeneralOptions;
	struct HDF5_FileOptions;
	struct HDF5_GroupOptions;
	struct HDF5_DatasetOptions;
	//Extra Options without linkage
	struct HDF5_DataspaceOptions;
	struct HDF5_DatatypeOptions;


	template<typename T>
	struct HDF5_OptionsSelector { static_assert(std::is_same_v<void, T>, "Options not defined!"); };
	template<>
	struct HDF5_OptionsSelector<HDF5_FileWrapper> { using type = HDF5_FileOptions; };
	template<>
	struct HDF5_OptionsSelector<HDF5_GroupWrapper> { using type = HDF5_GroupOptions; };
	template<>
	struct HDF5_OptionsSelector<HDF5_DatasetWrapper> { using type = HDF5_DatasetOptions; };
	template<>
	struct HDF5_OptionsSelector<HDF5_DataspaceWrapper> { using type = HDF5_DataspaceOptions; };
	template<>
	struct HDF5_OptionsSelector<HDF5_AttributeWrapper> { using type = HDF5_DummyOptions; };
	template<>
	struct HDF5_OptionsSelector<HDF5_DatatypeWrapper> { using type = HDF5_DatatypeOptions; };

	template<typename T>
	using HDF5_Options_t = typename HDF5_OptionsSelector<T>::type;

	//End of Forward declarations
	/*********************************************************************************************************************************************/
	
	struct HDF5_GeneralOptions
	{
		enum class HDF5_Mode { CreateOrOverwrite, OpenOrCreate, Open, Create };
		HDF5_Mode mode{ HDF5_Mode::OpenOrCreate };

		hid_t creation_propertylist{ H5P_DEFAULT };
		hid_t access_propertylist{ H5P_DEFAULT };

	};

		
	template<typename T>
	struct HDF5_OpenCreateCloseWrapper
	{
		static hid_t openOrCreate(const hdf5path& path, const HDF5_Options_t<T> &options = HDF5_Options_t<T>{}) noexcept
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
		static hid_t openOrCreate(const HDF5_GeneralType<U>& loc, const hdf5path& path, const HDF5_Options_t<T> &options = HDF5_Options_t<T>{})
		{
			if constexpr (std::is_same_v<HDF5_GroupWrapper, T>)
			{
				switch (options.mode)
				{
				case HDF5_GeneralOptions::HDF5_Mode::OpenOrCreate:
				{
					if (loc.checkLinkExists<T>(path, options)) { //Link does exist
						H5O_info_t oinfo;
						const auto herr = H5Oget_info_by_name(loc.getLocation(), path.string().c_str(), &oinfo, options.access_propertylist);

						assert(herr >= 0); // we checked that the link exists so the above should never fail!

						if (oinfo.type == H5O_TYPE_GROUP) {
							return H5Gopen(loc.getLocation(), path.string().c_str(), options.access_propertylist);
						}
						else {
							std::runtime_error{ "Given path is neither empty nor points to a HDF5 group!" };
						}

					}
					else { // does not exist
						return H5Gcreate(loc.getLocation(), path.string().c_str(), options.link_creation_propertylist, options.creation_propertylist, options.access_propertylist);
					}
					return -1;
				}
				case HDF5_GeneralOptions::HDF5_Mode::Open:
					return H5Gopen(loc.getLocation(), path.string().c_str(), options.access_propertylist);
				case HDF5_GeneralOptions::HDF5_Mode::Create:
					return H5Gcreate(loc.getLocation(), path.string().c_str(), options.link_creation_propertylist, options.creation_propertylist, options.access_propertylist);
				default:
					assert(false);
					return -1;
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
				return (hid_t)(-1);
			}
			else if constexpr (std::is_same_v<HDF5_DatasetWrapper, T>)
			{
				switch (options.mode)
				{
				case HDF5_GeneralOptions::HDF5_Mode::Open:
					return H5Dopen(loc.getLocation(), path.string().c_str(), options.access_propertylist);
				case HDF5_GeneralOptions::HDF5_Mode::OpenOrCreate:
				{
					if (loc.checkLinkExists<T>(path, options)) { //Link does exist
						H5O_info_t oinfo;
						const auto herr = H5Oget_info_by_name(loc.getLocation(), path.string().c_str(), &oinfo, options.access_propertylist);

						assert(herr >= 0); // we checked that the link exists so the above should never fail!

						if (oinfo.type == H5O_TYPE_DATASET) {
							return H5Dopen(loc.getLocation(), path.string().c_str(), options.access_propertylist);
						}
						else {
							std::runtime_error{ "Given path is neither empty nor points to a HDF5 Dataset!" };
						}

					}
					else { // does not exist
						return H5Dcreate(loc.getLocation(), path.string().c_str(), options.datatype, options.dataspace, options.link_creation_propertylist options.creation_propertylist, options.access_propertylist);
					}
					return -1;
				}
				case HDF5_GeneralOptions::HDF5_Mode::Create:
				{
					if (!loc.checkLinkExists<T>(path, options)) { //Link does exist
						return H5Dcreate(loc.getLocation(), path.string().c_str(), options.datatype, options.dataspace, options.link_creation_propertylist options.creation_propertylist, options.access_propertylist);
					}
					else {
						std::runtime_error{ "Given path already exists! Cannot create Dataset!" };
					}
				}
				default:
					assert(false);
					return -1;
				}
			}
			else
			{
				return (hid_t)(-1);
			}
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
				if(!isTypeImmutable(mLoc))
					return H5Tclose(mLoc);
				return 0;
			}
		}
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
	class HDF5_GeneralType 
	{
		static_assert(std::is_same_v<T, std::decay_t<T>>);

		HDF5_LocationWrapper mLoc;
		bool wasMoved{ false };
	protected:
		HDF5_Options_t<T> mOptions;
	public:
		using Base = T;
		DISALLOW_COPY_AND_ASSIGN(HDF5_GeneralType)
			
	protected:
		explicit HDF5_GeneralType(HDF5_LocationWrapper &&locID) : mLoc(std::move(locID)) 
		{};


	public:	
		template<typename U>
		explicit HDF5_GeneralType(const HDF5_GeneralType<U>& loc, const hdf5path& path, HDF5_Options_t<T> options)
			: mLoc(HDF5_OpenCreateCloseWrapper<T>::openOrCreate(loc, path, options)), mOptions(std::move(options)) {};
		
		// Special Case for HDF5_FileWrapper
		explicit HDF5_GeneralType(const filepath& path, const HDF5_Options_t<T>& options)
			: mLoc(HDF5_OpenCreateCloseWrapper<Base>::openOrCreate(path, options)), mOptions(options) {
			static_assert(std::is_same_v<T, HDF5_FileWrapper>);
		};

		HDF5_GeneralType(HDF5_GeneralType&& rhs) : mLoc(std::move(rhs.mLoc)), mOptions(std::move(rhs.mOptions))
		{
			rhs.wasMoved = true;
		}//Move Constructor
		HDF5_GeneralType(const HDF5_GeneralType&& rhs) : mLoc(std::move(rhs.mLoc)), mOptions(std::move(rhs.mOptions))
		{
			rhs.wasMoved = true;
		}//Move Constructor
		HDF5_GeneralType& operator=(HDF5_GeneralType&& rhs) 
		{
			this->mLoc = std::move(const_cast<HDF5_LocationWrapper>(rhs.mLoc));
			this->mOptions = std::move(const_cast<HDF5_LocationWrapper>(rhs.mOptions));
			rhs.wasMoved = true;
		}; //Move Assignment
		HDF5_GeneralType& operator=(const HDF5_GeneralType&& rhs)
		{
			this->mLoc = std::move(const_cast<HDF5_LocationWrapper>(rhs.mLoc));
			this->mOptions = std::move(const_cast<HDF5_LocationWrapper>(rhs.mOptions));
			rhs.wasMoved = true;
		}; //Move Assignment	


		~HDF5_GeneralType() noexcept
		{		
			if (!wasMoved)
				HDF5_OpenCreateCloseWrapper<Base>::close(mLoc);
		}

		const HDF5_LocationWrapper& getLocation() const noexcept { return mLoc; };

		inline operator const hid_t&() const
		{
			return mLoc;
		}; //Implicit Conversion Operator! 

		template<typename U>
		bool checkLinkExists(const hdf5path& path, const HDF5_Options_t<U>& options) const noexcept
		{
			//TODO: recursive check if path is complete path to an object! Meaning Path = <Linkname> is ok but  Path =<somepath>/<linkname> must be checked recursivly. 
			const auto herr = H5Lexists(this->mLoc, path.string().c_str(), options.access_propertylist);
			return herr > 0;
		}
	};

	struct HDF5_FileOptions : HDF5_GeneralOptions
	{
		enum class HDF5_FileCreationFlags { Exclusive, Overwrite };
		enum class HDF5_FileAccess { ReadOnly, ReadWrite };
		HDF5_FileCreationFlags creation_property{ HDF5_FileCreationFlags::Exclusive };
		HDF5_FileAccess access_property{ HDF5_FileAccess::ReadWrite };

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
	class HDF5_FileWrapper : public HDF5_GeneralType<HDF5_FileWrapper>
	{
		using ThisClass = HDF5_FileWrapper;
	public:
		using HDF5_GeneralType<ThisClass>::HDF5_GeneralType;
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
	class HDF5_GroupWrapper : public HDF5_GeneralType<HDF5_GroupWrapper>
	{
		using ThisClass = HDF5_GroupWrapper;

		template<typename T>
		static HDF5_LocationWrapper openOrCreateFile(const HDF5_GeneralType<T> &loc, const hdf5path &path, const HDF5_GroupOptions& options = HDF5_GroupOptions{})
		{
			if (path.has_extension())
				throw ::std::runtime_error{ "Group path should not have an extension!" };

			return HDF5_LocationWrapper(HDF5_OpenCreateCloseWrapper<ThisClass>::openOrCreate(loc, path, options));
		}
	public:
		template<typename T, typename _ = std::enable_if_t<std::is_same_v<HDF5_GroupWrapper, T> || std::is_same_v<HDF5_FileWrapper, T>>>
		HDF5_GroupWrapper(const HDF5_GeneralType<T> &loc, const hdf5path &path, const HDF5_GroupOptions& options = HDF5_GroupOptions{})
			: HDF5_GeneralType<HDF5_GroupWrapper>(openOrCreateFile(loc, path, options)) {};
	};

	struct HDF5_DatatypeOptions
	{
		HDF5_Datatype default_memory_datatyp{ HDF5_Datatype::Native };
		HDF5_Datatype default_storage_datatyp{ HDF5_Datatype::Native };
	};
	class HDF5_DatatypeWrapper : public HDF5_GeneralType<HDF5_DatatypeWrapper>
	{
		using ThisClass = HDF5_DatatypeWrapper;
	public:
		template<typename T>
		HDF5_DatatypeWrapper(const T& val, const HDF5_DatatypeOptions &options) : 
			HDF5_GeneralType<HDF5_DatatypeWrapper>(HDF5_LocationWrapper(DatatypeRuntimeSelector::getType(options.default_storage_datatyp, val) ) ) {};
	};

	struct HDF5_DataspaceOptions
	{
		std::vector<hsize_t> dims;
		std::vector<hsize_t> maxdims;

		std::int32_t getRank() const
		{
			assert(dims.size() == maxdims.size());
			const auto ndims = dims.size();

			if (std::numeric_limits<std::int32_t>::max() < ndims)
				throw std::runtime_error{ "Too many dimension. Cannot determine rank due to overflow!" };
			return static_cast<std::int32_t>(ndims);
		}

		void makeUnlimited()
		{
			maxdims = std::vector<hsize_t>(dims.size(), H5S_UNLIMITED);
		}

		bool isUnlimited() const noexcept
		{
			const auto found = std::find(maxdims.begin(), maxdims.end(), H5S_UNLIMITED);
			return (found != maxdims.end());
		}
	};
	class HDF5_DataspaceWrapper : public HDF5_GeneralType<HDF5_DataspaceWrapper>
	{
		using ThisClass = HDF5_DatasetWrapper;

		static HDF5_LocationWrapper createDataspace(const H5S_class_t& type, const HDF5_DataspaceOptions& opts)
		{
			switch (type)
			{
			case H5S_SCALAR: case H5S_NO_CLASS:
				return HDF5_LocationWrapper(H5Screate(type));
			case H5S_SIMPLE:
				return HDF5_LocationWrapper(H5Screate_simple(opts.getRank(), opts.dims.data(), opts.maxdims.data()));
			default:
				assert(false);
				return HDF5_LocationWrapper(-1);
			}

		};
	public:
		HDF5_DataspaceWrapper(const H5S_class_t& type, const HDF5_DataspaceOptions& opts) : HDF5_GeneralType<HDF5_DataspaceWrapper>(createDataspace(type, opts))
		{}
	};

	struct HDF5_MemoryOptions
	{
		HDF5_DatatypeWrapper   datatype;
		HDF5_DataspaceWrapper  dataspace;
	};

	struct HDF5_DatasetOptions : HDF5_GeneralOptions
	{
		HDF5_DatatypeWrapper   datatype;
		HDF5_DataspaceWrapper  dataspace;

		hid_t link_creation_propertylist{ H5P_DEFAULT };
		hid_t transfer_propertylist{ H5P_DEFAULT };

		HDF5_DatasetOptions(HDF5_DatatypeWrapper dtype, HDF5_DataspaceWrapper dspace, hid_t lcpl = H5P_DEFAULT)
			: datatype(std::move(dtype)), dataspace(std::move(dspace)), link_creation_propertylist(std::move(lcpl)) {};

		DISALLOW_COPY_AND_ASSIGN(HDF5_DatasetOptions)
		ALLOW_DEFAULT_MOVE_AND_ASSIGN(HDF5_DatasetOptions)
	};
	class HDF5_DatasetWrapper : public HDF5_GeneralType<HDF5_DatasetWrapper>
	{
		using ThisClass = HDF5_DatasetWrapper;
	public:
		using HDF5_GeneralType<ThisClass>::HDF5_GeneralType;

		template<typename T, typename _ = void>
		auto writeData(const T& val, const HDF5_MemoryOptions& memopts)
		{
			return H5Dwrite(*this, memopts.datatype, memopts.dataspace, mOptions.dataspace, mOptions.transfer_propertylist, &val);
		}
		template<typename T, typename _ = void >
		auto writeData(const T* val, const HDF5_MemoryOptions& memopts)
		{
			return H5Dwrite(*this, memopts.datatype, memopts.dataspace, mOptions.dataspace, mOptions.transfer_propertylist, val);
		}

		template<typename T>
		auto readData(T& val, const HDF5_MemoryOptions& memopts)
		{
			return H5Dread(*this, memopts.datatype, memopts.dataspace, mOptions.dataspace, mOptions.transfer_propertylist, &val);
		}
		template<typename T>
		auto readData(T* val, const HDF5_MemoryOptions& memopts)
		{
			return H5Dread(*this, memopts.datatype, memopts.dataspace, mOptions.dataspace, mOptions.transfer_propertylist, val);
		}


	};



	class HDF5_AttributeWrapper : public HDF5_GeneralType<HDF5_AttributeWrapper>
	{
	public:
	};
}



#endif	// INC_HDF5_FileWrapper_H
// end of HDF5_Archive\HDF5_FileWrapper.h
///---------------------------------------------------------------------------------------------------
