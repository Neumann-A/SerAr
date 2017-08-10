///---------------------------------------------------------------------------------------------------
// file:		HDF5_Archive.h
//
// summary: 	Declares the hdf 5 archive class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander
// date: 08.03.2017

#ifndef INC_HDF5_Archive_H
#define INC_HDF5_Archive_H
///---------------------------------------------------------------------------------------------------
#pragma once

#include <cassert>

#include <type_traits>

#include <experimental/filesystem>

#include <utility>
#include <map>
#include <iosfwd>
#include <string>
#include <string_view>
#include <regex>
#include <complex>
#include <exception>
#include <memory>
#include <stack>

//#ifdef EIGEN_CORE_H
//#include <Eigen/Core>
//#endif

//If you want dynamic lib for HDF5 define H5_BUILT_AS_DYNAMIC_LIB
#ifdef _MSC_VER
#ifdef _DEBUG
#ifdef H5_BUILT_AS_DYNAMIC_LIB
#pragma comment(lib,"hdf5_D.lib")
#pragma comment(lib,"hdf5_hl_D.lib")
//#pragma comment(lib,"hdf5_hl_cpp_D.lib")
//#pragma comment(lib,"hdf5_cpp_D.lib")
//#pragma comment(lib,"szip.lib")
//#pragma comment(lib,"zlib.lib")
#else
#pragma comment(lib,"libhdf5_D.lib")
#pragma comment(lib,"libhdf5_hl_D.lib")
//#pragma comment(lib,"libhdf5_hl_cpp_D.lib")
//#pragma comment(lib,"libhdf5_cpp_D.lib")
//#pragma comment(lib,"libszip.lib")
//#pragma comment(lib,"libzlib.lib")
#endif
#else
#ifdef H5_BUILT_AS_DYNAMIC_LIB
#pragma comment(lib,"hdf5.lib")
#pragma comment(lib,"hdf5_hl.lib")
//#pragma comment(lib,"hdf5_hl_cpp.lib")
//#pragma comment(lib,"hdf5_cpp.lib")
//#pragma comment(lib,"szip.lib")
//#pragma comment(lib,"zlib.lib")
#else
#pragma comment(lib,"libhdf5.lib")
#pragma comment(lib,"libhdf5_hl.lib")
//#pragma comment(lib,"libhdf5_hl_cpp.lib")
//#pragma comment(lib,"libhdf5_cpp.lib")
//#pragma comment(lib,"libszip.lib")
//#pragma comment(lib,"libzlib.lib")
#endif
#endif
#endif

#include <hdf5.h>
#include <hdf5_hl.h>

#include "basics/BasicMacros.h"
#include "basics/BasicIncludes.h"
#include "stdext/std_extensions.h"

#include "Archive/NamedValue.h"
#include "Archive/InputArchive.h"
#include "Archive/OutputArchive.h"

#include "HDF5_Wrappers.h"

namespace Archives
{
	class HDF5_OutputArchive;
	class HDF5_InputArchive;

	namespace HDF5_traits
	{
		//Member Function type for converting values to strings
		template<class Class, typename ...Args>
		using write_to_HDF5_t = decltype(std::declval<Class>().write(std::declval<std::decay_t<Args>>()...));

		template<typename Datatype>
		class has_write_to_HDF5 : public stdext::is_detected_exact<void, write_to_HDF5_t, HDF5_OutputArchive, Datatype> {};
		template<typename Type>
		static constexpr bool has_write_to_HDF5_v = has_write_to_HDF5<Type>::value;

		template<class Class, typename ...Args>
		using getData_from_HDF5_t = decltype(std::declval<Class>().getData(std::declval<std::decay_t<Args>>()...));
		template<typename Datatype>
		class has_getData_from_HDF5 : public stdext::is_detected_exact<void, getData_from_HDF5_t, HDF5_InputArchive, Datatype> {};
		template<typename Type>
		static constexpr bool has_getData_from_HDF5_v = has_getData_from_HDF5<Type>::value;
	}

	class HDF5_OutputArchive; // Forward declare archive for Options;

	class HDF5_OutputOptions : OutputArchive_Options<HDF5_OutputOptions, HDF5_OutputArchive>
	{
	public:
		HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode FileCreationMode{ HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode::CreateOrOverwrite };
	};

	class HDF5_OutputArchive : public OutputArchive<HDF5_OutputArchive>
	{
	public:
		HDF5_OutputArchive(const std::experimental::filesystem::path &path, const HDF5_OutputOptions& options) : OutputArchive(this), mFile(openOrCreateFile(path,options))
		{
			//std::cout << mFile.getLocation().getHDF5Path() << std::endl;
			//std::cout << mFile.getLocation().getHDF5Fullpath() << std::endl;
		}

		DISALLOW_COPY_AND_ASSIGN(HDF5_OutputArchive)

		template<typename T>
		inline void save(const Archives::NamedValue<T>& value)
		{
			setNextPath(value.getName());		//Set the Name of the next Field
			this->operator()(value.getValue()); //Write Data to the Field/struct
			clearNextPath();					//Remove the last Fieldname
		};

		template<typename T>
		inline std::enable_if_t< HDF5_traits::has_write_to_HDF5_v<T> > save(const T& value)
		{
			write(value);
		};

		template<typename T>
		inline std::enable_if_t<traits::uses_type_save_v<T, HDF5_OutputArchive> > prologue(const T& value)
		{		
			createOrOpenDataset(value);
		};
		template<typename T>
		inline std::enable_if_t<traits::uses_type_save_v<T, HDF5_OutputArchive> > epilogue(const T& value)
		{
			closeDataset(value);
		};

		//If the type does not have a write to HDF5 function here it means we need to create/open a Group!
		template<typename T>
		inline std::enable_if_t<!HDF5_traits::has_write_to_HDF5_v<T> || is_nested_NamedValue_v<T> > prologue(const T& value)
		{
			if constexpr(is_nested_NamedValue_v<T>)
				setNextPath(value.getName());
			
			createOrOpenGroup(value);
			
			if constexpr(is_nested_NamedValue_v<T>)
				clearNextPath();
		};

		template<typename T>
		inline std::enable_if_t<!HDF5_traits::has_write_to_HDF5_v<T> || is_nested_NamedValue_v<T> > epilogue(const T& value)
		{
			closeLastGroup(value);
		};

	private:
		std::string nextPath;
		using CurrentGroup = HDF5_Wrapper::HDF5_GroupWrapper;
		using File = HDF5_Wrapper::HDF5_FileWrapper;
		std::stack<CurrentGroup> mGroupStack;
		File mFile;

		static File openOrCreateFile(const std::experimental::filesystem::path &path, const HDF5_OutputOptions& options)
		{
			using namespace HDF5_Wrapper;
			HDF5_FileOptions opt{};
			opt.mode = options.FileCreationMode;
			
			File file{ path, opt };
			return file; 
			//return file; //this called a destructor!
		}

		bool isValidNextLocation(const std::string&) const
		{
			//TODO: Check the Location name for invalid characters
			return true;
		}
		void setNextPath(const std::string& str)
		{
			if (!isValidNextLocation(str))
				throw std::runtime_error{ "Invalid HDF5 path string!" };

			assert(nextPath.empty());
			
			nextPath = str;
		}
		void clearNextPath()
		{
			nextPath.clear();
		}

		template<typename T>
		void createOrOpenGroup(const T&)
		{
			using namespace HDF5_Wrapper;

			assert(!nextPath.empty());
	
			if (mGroupStack.empty())
			{
				HDF5_GroupOptions opts;
				opts.mode = HDF5_GeneralOptions::HDF5_Mode::OpenOrCreate;
				HDF5_GroupWrapper group{mFile,nextPath, opts};
				mGroupStack.push(std::move(group));
			}
			else
			{
				const auto& currentGroup{mGroupStack.top()};
				HDF5_GroupOptions opts;
				opts.mode = HDF5_GeneralOptions::HDF5_Mode::OpenOrCreate;
				HDF5_GroupWrapper group{ currentGroup,nextPath, opts };
				mGroupStack.push(std::move(group));
			}
		}

		template<typename T>
		void closeLastGroup(const T&)
		{
			assert(!mGroupStack.empty());
			mGroupStack.pop();
		}

		template<typename T>
		void createOrOpenDataset(const T& val)
		{

		}
		template<typename T>
		void closeLastDataset(const T& val)
		{

		}

		template <typename T>
		std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>> write(const T&)
		{

		}
	};


	class HDF5_InputArchive : public InputArchive<HDF5_InputArchive>
	{
		HDF5_InputArchive(const std::experimental::filesystem::path &) : InputArchive(this)
		{

		}
	};

}

#endif	// INC_HDF5_Archive_H

///---------------------------------------------------------------------------------------------------
