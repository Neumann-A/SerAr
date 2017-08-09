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
#pragma comment(lib,"libhdf5_hl.lib")
#pragma comment(lib,"libhdf5.lib")
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

#include "HDF5_FileWrapper.h"

namespace Archives
{
	namespace HDF5_traits
	{
		//Member Function type for converting values to strings
		template<class Class, typename ...Args>
		using write_to_HDF5_t = decltype(std::declval<Class>().write(std::declval<std::decay_t<Args>>()...));

		template<typename Datatype>
		class has_write_to_HDF5 : public stdext::is_detected_exact<void, write_to_HDF5_t, Datatype> {};
		template<typename Type>
		static constexpr bool has_write_to_HDF5_v = has_write_to_HDF5<Type>::value;

		template<class Class, typename ...Args>
		using getData_from_HDF5_t = decltype(std::declval<Class>().getData(std::declval<std::decay_t<Args>>()...));
		template<typename Datatype>
		class has_getData_from_HDF5 : public stdext::is_detected_exact<void, getData_from_HDF5_t, Datatype> {};
		template<typename Type>
		static constexpr bool has_getData_from_HDF5_v = has_getData_from_HDF5<Type>::value;
	}


	namespace HDF5_detail
	{

		//Not specialized
		template<typename T, typename = void>
		struct DataSpaceTypeSelector
		{
			static constexpr inline H5S_class_t value() { return H5S_NO_CLASS; };
		};

		//Arithmetic Types
		template<typename T>
		struct DataSpaceTypeSelector<T, std::enable_if_t<std::is_arithmetic_v<T>>>
		{
			static constexpr inline H5S_class_t value() { return H5S_SCALAR; };
		};

		//Container Types
		template<typename T>
		struct DataSpaceTypeSelector<T, std::enable_if_t<stdext::is_container_v<T>>>
		{
			static constexpr inline H5S_class_t value() { return H5S_SIMPLE; };
		};

		//Strings
		template<typename T>
		struct DataSpaceTypeSelector<T, std::enable_if_t<stdext::is_string_v<T>>>
		{
			static constexpr inline H5S_class_t value() { return H5S_SCALAR; };
		};
	}

	class HDF5_OutputArchive; // Forward declare archive for Options;

	class HDF5_OutputOptions : OutputArchive_Options<HDF5_OutputOptions, HDF5_OutputArchive>
	{
	public:

	};

	class HDF5_OutputArchive : public OutputArchive<HDF5_OutputArchive>
	{
	private:
	public:
	};


	class HDF5_InputArchive : public InputArchive<HDF5_InputArchive>
	{

	};

}

#endif	// INC_HDF5_Archive_H

///---------------------------------------------------------------------------------------------------
