///---------------------------------------------------------------------------------------------------
// file:		Matlab_Archive.h
//
// summary: 	Declares the matlab archive class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander
// date: 08.03.2017

#ifndef INC_Matlab_Archive_H
#define INC_Matlab_Archive_H
///---------------------------------------------------------------------------------------------------
#pragma once

#include <experimental/filesystem>
#include <type_traits>
#include <utility>
#include <map>
#include <iosfwd>
#include <string>
#include <regex>
#include <complex>
#include <exception>
#include <cassert>
#include <cstdint>

//#ifdef EIGEN_CORE_H
//#include <Eigen/Core>
//#endif

#include "stdext/std_extensions.h"
#include "basics/BasicMacros.h"
#include "basics/BasicIncludes.h"

//#include "ArchiveHelper.h"
#include "Archive/NamedValue.h"
#include "Archive/InputArchive.h"
#include "Archive/OutputArchive.h"

//#include <engine.h> //To connect to matlab engine (we dont work directly with matlab so does not matter)
//MATLAB includes

#ifdef _MSC_VER
#pragma comment (lib, "libmx")
#pragma comment (lib, "libmat")
#endif

//Not used!
//#pragma comment (lib, "libeng")
//#pragma comment (lib, "libmex")

#include <mat.h>


//Needs the following PATH = C:\Program Files\Matlab\R2015b\bin\win64; %PATH%

namespace Archives
{
	namespace traits
	{
		//Check if Type has the function create_MATLAB within the Archive
		template<class Class, typename ...Args>
		using create_MATLAB_t = decltype(std::declval<Class>().createMATLABArray(std::declval<std::decay_t<Args>>()...));
		template<typename MATClass, typename Type>
		class has_create_MATLAB : public stdext::is_detected_exact<mxArray&, create_MATLAB_t, MATClass, Type> {};
		template<typename MATClass, typename Type>
		static constexpr bool has_create_MATLAB_v = has_create_MATLAB<MATClass, Type>::value;

		//Check if the type has a function to load it from an mxArray
		template<class Class, typename Type>
		using getvalue_MATLAB_t = decltype(std::declval<Class>().getValue(std::declval<Type&>(), std::declval<mxArray const * const>()));
		//using getvalue_MATLAB_t = decltype(std::declval<Class>().getValue(std::declval<std::add_lvalue_reference_t<std::decay_t<Args>>>()...));
		template<typename MATClass, typename Type>
		class has_getvalue_MATLAB : public stdext::is_detected<getvalue_MATLAB_t, MATClass, Type> {};

		//template<typename MATClass, typename Type>
		//class has_loadType_MATLAB : public stdext::is_detected<loadtype_MATLAB_t, MATClass, Type> {};
		template<typename MATClass, typename Type>
		static constexpr bool has_getvalue_MATLAB_v = has_getvalue_MATLAB<MATClass, Type>::value;
	}

	namespace MATLAB
	{
		typedef std::integral_constant<mxClassID, mxUNKNOWN_CLASS>	MATLAB_UnknownClass;
		typedef std::integral_constant<mxClassID, mxCELL_CLASS>		MATLAB_CellClass;
		typedef std::integral_constant<mxClassID, mxSTRUCT_CLASS>	MATLAB_StructClass;
		typedef std::integral_constant<mxClassID, mxLOGICAL_CLASS>	MATLAB_LogicalClass;
		typedef std::integral_constant<mxClassID, mxCHAR_CLASS>		MATLAB_CharClass;
		typedef std::integral_constant<mxClassID, mxVOID_CLASS>		MATLAB_VoidClass;
		typedef std::integral_constant<mxClassID, mxDOUBLE_CLASS>	MATLAB_DoubleClass;
		typedef std::integral_constant<mxClassID, mxSINGLE_CLASS>	MATLAB_SingleClass;
		typedef std::integral_constant<mxClassID, mxINT8_CLASS>		MATLAB_Int8Class;
		typedef std::integral_constant<mxClassID, mxUINT8_CLASS>	MATLAB_UInt8Class;
		typedef std::integral_constant<mxClassID, mxINT16_CLASS>	MATLAB_Int16Class;
		typedef std::integral_constant<mxClassID, mxUINT16_CLASS>	MATLAB_UInt16Class;
		typedef std::integral_constant<mxClassID, mxINT32_CLASS>	MATLAB_Int32Class;
		typedef std::integral_constant<mxClassID, mxUINT32_CLASS>	MATLAB_UInt32Class;
		typedef std::integral_constant<mxClassID, mxINT64_CLASS>	MATLAB_Int64Class;
		typedef std::integral_constant<mxClassID, mxUINT64_CLASS>	MATLAB_UInt64Class;
		typedef std::integral_constant<mxClassID, mxFUNCTION_CLASS>	MATLAB_FunctionClass;
		typedef std::integral_constant<mxClassID, mxOPAQUE_CLASS>	MATLAB_OpaqueClass;
		typedef std::integral_constant<mxClassID, mxOBJECT_CLASS>	MATLAB_ObjectClass;
		typedef std::integral_constant<mxClassID, mxINDEX_CLASS>	MATLAB_IndexClass;

		template<typename T>
		struct MATLABClassFinder : MATLAB_UnknownClass 
		{
			// Normally this static assert should not be necessary but sometimes we seem to need it!
			// (Due to template specilization and selection rules)
			static_assert(std::is_same<T,void>::value,"MATLABClassFinder: Unknown type!");
		};

		//Partial specialization for all relevant types
		template<typename CharT, typename TraitsT, typename AllocatorT>
		struct MATLABClassFinder<std::basic_string<CharT, TraitsT,AllocatorT>> : MATLAB_CharClass {};
		template<typename ...Args>
		struct MATLABClassFinder<std::tuple<Args...>> : MATLAB_CellClass {};
		template<>
		struct MATLABClassFinder<const char*> : MATLAB_CharClass {};
		template<>
		struct MATLABClassFinder<bool> : MATLAB_LogicalClass {};
		template<>
		struct MATLABClassFinder<std::uint8_t> : MATLAB_UInt8Class {};
		template<>
		struct MATLABClassFinder<std::int8_t> : MATLAB_Int8Class {};
		template<>
		struct MATLABClassFinder<std::uint16_t> : MATLAB_UInt16Class {};
		template<>
		struct MATLABClassFinder<std::int16_t> : MATLAB_Int16Class {};
		template<>
		struct MATLABClassFinder<std::uint32_t> : MATLAB_UInt32Class {};
		//template<>
		//struct MATLABClassFinder<unsigned long> : MATLAB_UInt32Class {};
		template<>
		struct MATLABClassFinder<std::int32_t> : MATLAB_Int32Class {};
		// template<>
		// struct MATLABClassFinder<long> : MATLAB_Int32Class {};
		template<>
		struct MATLABClassFinder<std::uint64_t> : MATLAB_UInt64Class {};
		// template<>
		// struct MATLABClassFinder<unsigned long long> : MATLAB_UInt64Class {};
		// template<>
		// struct MATLABClassFinder<std::size_t> : MATLAB_UInt64Class {};
		template<>
		struct MATLABClassFinder<std::int64_t> : MATLAB_Int64Class {};
		template<>
		struct MATLABClassFinder<float> : MATLAB_SingleClass {};
		template<>
		struct MATLABClassFinder<double> : MATLAB_DoubleClass {};
		//NOTE: Narrowing conversion to MATLAB. There seems to be no long double conversion in MATLAB!
		template<>
		struct MATLABClassFinder<long double> : MATLAB_DoubleClass {}; 
#ifdef EIGEN_CORE_H
		template<typename T>
		struct MATLABClassFinder<Eigen::EigenBase<T>> : MATLABClassFinder<typename T::Scalar> {};
		template<typename T>
		struct MATLABClassFinder<Eigen::TensorBase<T>> : MATLABClassFinder<typename T::Scalar> {};
#endif
	}

	//Enum to represent the different Matlab file modes!
	//read: Opens file for reading only; determines the current version of the MAT-file by inspecting the files and preserves the current version.
	//update: Opens file for update, both reading and writing. If the file does not exist, does not create a file (equivalent to the r+ mode of fopen). Determines the current version of the MAT-file by inspecting the files and preserves the current version.
	//write: Opens file for writing only; deletes previous contents, if any.
	//write_v4: Creates a MAT-file compatible with MATLAB� Versions 4 software and earlier.
	//write_v6: Creates a MAT-file compatible with MATLAB Version 5 (R8) software or earlier. Equivalent to wL mode.
	//write_local :Opens file for writing character data using the default character set for your system. Use MATLAB Version 6 or 6.5 software to read the resulting MAT-file.
	//			     	  If you do not use the wL mode switch, MATLAB writes character data to the MAT-file using Unicode� character encoding by default.
	//					  Equivalent to w6 mode.
	//write_v7: Creates a MAT-file compatible with MATLAB Version 7.0 (R14) software or earlier. Equivalent to wz mode.
	//write_compressed: Opens file for writing compressed data. By default, the MATLAB save function compresses workspace variables as they are saved to a MAT-file. To use the same compression ratio when creating a MAT-file with the matOpen function, use the wz option.
	//						  Equivalent to w7 mode.
	//write_v73: Creates a MAT-file in an HDF5-based format that can store objects that occupy more than 2 GB.
	enum class MatlabOptions {read, update, write, write_v4, write_v6, write_local, write_v7, write_compressed, write_v73};
	
	class MatlabHelper
	{
	public:
		static inline const char* getMatlabMode(const MatlabOptions& option) noexcept
		{
			const char* mode{ [&]()
			{switch (option)
			{
			case MatlabOptions::read:
				return "r";
			case MatlabOptions::update:
				return "u";
			case MatlabOptions::write:
				return "w";
			case MatlabOptions::write_v4:
				return "w4";
			case MatlabOptions::write_v6:
				return "w6";
			case MatlabOptions::write_local:
				return "wL";
			case MatlabOptions::write_v7:
				return "w7";
			case MatlabOptions::write_compressed:
				return "wz";
			case MatlabOptions::write_v73:
				return "w7.3";
			default:
				assert(false); //Invalid Enum!
				return "";
			}
			}() };
			return mode;
		}
	};



	///-------------------------------------------------------------------------------------------------
	/// <summary>	A matlab output archive. </summary>
	///
	/// <remarks>	Neumann, 17.02.2017. </remarks>
	///-------------------------------------------------------------------------------------------------
	class MatlabOutputArchive : public OutputArchive<MatlabOutputArchive>
	{
		//TODO: Make the Code for the MatlabOutputArchive cleaner. The Code for the MatlabInputArchive seems much cleaner than this one!

		friend class OutputArchive<MatlabOutputArchive>;
		//needed so that the detector idom works with clang-cl (for some unknown reason!)
		template <class Default, class AlwaysVoid, template<class...> class Op, class... Args> friend struct stdext::DETECTOR;

	public:
		MatlabOutputArchive(const std::experimental::filesystem::path &fpath, const MatlabOptions &options = MatlabOptions::update);
		~MatlabOutputArchive();
		DISALLOW_COPY_AND_ASSIGN(MatlabOutputArchive)
		template<typename T>
		inline void save(const Archives::NamedValue<T>& value)
		{
			setNextFieldname(value.getName());  //Set the Name of the next Field
			this->operator()(value.getValue()); //Write Data to the Field/struct
			clearNextFieldname();				//Remove the last Fieldname
		};

		
		template<typename T>
		inline std::enable_if_t<traits::has_create_MATLAB_v<MatlabOutputArchive,  std::remove_reference_t<T>>> save(const T& value)
		{	//SFINE checks wether T can be saved by this MATLAB Archive!
			using Type = std::remove_reference_t<T>;
			//TODO: Build Fieldname if none has been set. Important! Otherwise matlab will throw an runtime exception!
			if (nextFieldname.empty()) //We need to create a fieldname 
				nextFieldname = typeid(T).name(); //TODO: Sanitize Fieldnames(remove spaces, points etc)! Currently wont work correctly without a (allowed) fieldname
			
			static_assert(!std::is_same< MATLAB::MATLABClassFinder<Type>, MATLAB::MATLAB_UnknownClass>::value,"T is not known within MATLAB. Will be unable to create mxArrray*!");

			auto& arrdata = createMATLABArray<Type>(value);
			
			Fields.push(std::make_tuple(std::move(nextFieldname), &arrdata));

			finishMATLABArray();
		}


	//private:
	public:
		const std::experimental::filesystem::path m_filepath;
		MatlabOptions m_options;
		MATFile &m_MatlabFile;
		
		using Field = std::tuple<std::string, mxArray*>;	
		std::stack<Field> Fields;	//Using a stack to hold all Fields; Since the implementation is recursive in save().
	
		std::string nextFieldname;	//Storage for next fieldname which has not been pushed on the Fields stack yet since the mxArray* was not yet created 

		MATFile& getMatlabFile(const std::experimental::filesystem::path &fpath, const MatlabOptions &options = MatlabOptions::update) const
		{	
			assert(options != MatlabOptions::read);//, "Cannot have a MatlabOutputArchive with read-only access!");

			if(!fpath.has_filename())
				throw std::runtime_error{ std::string{"Could not open file due to missing filename: "} + fpath.string() };

			MATFile *pMAT = matOpen( fpath.string().c_str(), MatlabHelper::getMatlabMode(options));

			if (pMAT == nullptr)
				throw std::runtime_error{ std::string{ "Could not open file: " } +fpath.string() };

			return *pMAT;
		};
		
		template<typename T>
		inline std::enable_if_t<traits::uses_type_save_v<T, MatlabOutputArchive> > prologue(const T& value)
		{
			checkNextFieldname(value);	//Check the Fieldname for validity
			startMATLABArray(value);	//Start a new Matlab Array or Struct
		};
		template<typename T>
		inline std::enable_if_t<traits::uses_type_save_v<T, MatlabOutputArchive> > epilogue(const T&)
		{
			finishMATLABArray();	//Finish the Array (write it to the Array above)
		};
		
		//For nested NamedValue
		template<typename T>
		inline std::enable_if_t<is_nested_NamedValue_v<T>> prologue(const T& value)
		{
			setNextFieldname(value.getName());
			startMATLABArray(value);
			clearNextFieldname();
		};
		template<typename T>
		inline std::enable_if_t<is_nested_NamedValue_v<T>> epilogue(const T&)
		{
			finishMATLABArray();
		};


		inline void setNextFieldname(const std::string &str) noexcept
		{
			nextFieldname = str;
		}

		inline void clearNextFieldname() noexcept
		{
			nextFieldname.clear();
		}

		template<typename T>
		inline void checkNextFieldname(T&& /*val*/)
		{
			//TODO: Create proper fieldname and sanitize fieldnames (points, spaces are not allowed!)
			if (nextFieldname.empty())
			{
				throw std::runtime_error{ "No Fieldname defined! (Invalid behavior right now!)" };
			}
		}

		//Starting a new field
		template<typename T>
		inline std::enable_if_t<!traits::has_create_MATLAB_v<MatlabOutputArchive, std::decay_t<T>>> startMATLABArray(const T& val)
		{
			checkNextFieldname(val);

			mxArray *pStruct = nullptr;
			
			if (!Fields.empty())
			{
				pStruct = mxGetField(std::get<1>(Fields.top()), 0, nextFieldname.c_str());
			}
			
			if (pStruct == nullptr)
			{
				constexpr mwSize ndims = 2;
				constexpr mwSize dims[ndims]{ 1,1 }; // Higher dimensional structs are a bit strange... looks like an array of structs (not what we normally want. What we want -> [1,1])
				pStruct = mxCreateStructArray(ndims, dims, 0, nullptr);
				if (pStruct == nullptr)
					throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

				//std::cout << "Creating Field:" << nextFieldname << std::endl;
			}

			if (mxGetClassID(pStruct) != mxSTRUCT_CLASS)
			{
				throw std::runtime_error{ "Updating a mxArray which is not a struct! (Unwanted behavior!)" };
			}

			Fields.push(std::make_tuple(std::move(nextFieldname), pStruct));
		};

		void finishMATLABArray();
	
		//Save all other arithmetics types
		template<typename T>
		std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>, mxArray&> createMATLABArray(const T& value) const
		{
			mxArray *valarray = mxCreateNumericMatrix(1, 1, MATLAB::MATLABClassFinder<std::decay_t<T>>::value, mxREAL);
			if (valarray == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };
			*static_cast<T*>(mxGetData(valarray)) = value;
			return *valarray;
		}

		//Saving Strings
		template<typename T>
		std::enable_if_t<stdext::is_string_v<T>,mxArray&> createMATLABArray(const T& value) const
		{
			mxArray *valarray = mxCreateString(value.c_str());
			
			if (valarray == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

			return *valarray;
		}

		//Save for container types with arithmetic payload type
		template<typename T>
		std::enable_if_t<stdext::is_arithmetic_container_v<T>, mxArray&> createMATLABArray(const T& value) const
		{
			using DataType = std::decay_t<typename T::value_type>;

			mxArray *valarray = mxCreateNumericMatrix(value.size(),1, MATLAB::MATLABClassFinder<DataType>::value, mxREAL);
			if (valarray == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

			//Cast needed since mxGetPr always returns an double pointer!
			DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(valarray));
			assert(dataposition != nullptr);

			for (const auto& tmp : value)
			{
				*dataposition++ = tmp;
			}

			return *valarray;
		}

#ifdef EIGEN_CORE_H
		//Eigen Types 
		template<typename T>
		std::enable_if_t<stdext::is_container_with_eigen_type_v<T>, mxArray&>
			createMATLABArray(const T& value) const
		{
			using EigenMatrix = typename std::decay_t<T>::value_type;
			using DataType = typename EigenMatrix::Scalar;
			const auto& first = value.begin();
			const mwSize rows = first->rows();
			const mwSize cols = first->cols();

			constexpr mwSize ndim = 3;


			const auto& f = [](const auto& dim1,const auto& dim2, const auto& dim3)->std::array<mwSize, 3> {
				mwSize a, b, c;

				// Case necessary since we can use the fact that MATLAB drops singleton dims
				if (dim1 <= 1) //Col Vector Case
				{
					a = dim2;
					b = dim3;
					c = dim1;
				}
				else
				{
					if (dim2 <= 1) //Row Vector Case
					{
						a = dim1;
						b = dim3;
						c = dim2;
					}
					else // Matrix case
					{
						a = dim1;
						b = dim2;
						c = dim3;
					}
				}
				return{ { a,b,c } };
			};

			std::array<mwSize,ndim> dims = f(rows,cols,value.size());

			//This command will double memory consumption since it has to allocate the memory!
			mxArray *valarray = mxCreateNumericArray(ndim, dims.data(), MATLAB::MATLABClassFinder<DataType>::value, mxREAL);
						
			if (valarray == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

			DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(valarray));
		
			assert(dataposition != nullptr);

			//Be aware: of strange storage order!
			for (const auto& tmp : value)
			{
				if constexpr (EigenMatrix::IsRowMajor) {
					Eigen::Map< EigenMatrix, Eigen::Unaligned, Eigen::Stride<1, EigenMatrix::ColsAtCompileTime> >(dataposition, tmp.rows(), tmp.cols()) = tmp;
				}
				else {
					Eigen::Map< EigenMatrix, Eigen::Unaligned>(dataposition, tmp.rows(), tmp.cols()) = tmp;
				}
				if constexpr (EigenMatrix::IsVectorAtCompileTime)	{
					dataposition += rows > cols ? rows : cols;
				}
				else {
					dataposition += rows*cols;
				}		
			}																																									
			return *valarray;
		}

		template<typename T>
		std::enable_if_t<stdext::is_eigen_type_v<T>, mxArray&> createMATLABArray(const Eigen::EigenBase<T>& value) const
		{
			using DataType = typename T::Scalar;

			//Be Aware: Matlab stores the matrix in column-major order, Eigen in row major!
			mxArray *valarray = mxCreateNumericMatrix(value.rows(), value.cols(), MATLAB::MATLABClassFinder<DataType>::value, mxREAL);
			if (valarray == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

			DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(valarray));
			/* Inserting Data into Array */
			if constexpr (T::IsRowMajor && !T::IsVectorAtCompileTime)
			{
				Eigen::Map< T, Eigen::Unaligned, Eigen::Stride<1, T::ColsAtCompileTime> >(dataposition, value.rows(), value.cols()) = value;
			}
			else
			{
				Eigen::Map< T, Eigen::Unaligned>(dataposition, value.rows(), value.cols()) = value;
			}
			return *valarray;
		}

#ifdef EIGEN_CXX11_TENSOR_TENSOR_H
		template<typename T>
		inline std::enable_if_t< stdext::is_eigen_tensor_v<std::decay_t<T>>, mxArray&> createMATLABArray(const Eigen::TensorBase<T>& value) const
		{
			using Type = std::decay_t<T>; // T cannot be const
			using DataType = typename T::Scalar;
			using Index = typename T::Index;
			using Dimensions = typename T::Dimensions;
			using ArrayBase = typename T::Dimensions::Base;

			const auto ndims = value.NumDimensions;
			const auto dims = static_cast<T>(value).dimensions();

			mxArray *valarray = mxCreateNumericArray(ndims, (const std::size_t*)(dims.data()), MATLAB::MATLABClassFinder<DataType>::value, mxREAL);
			if (valarray == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

			DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(valarray));
			assert(dataposition != nullptr);

			/* Reading Data from Array */
			Eigen::TensorMap<Type>(dataposition, dims) = value;
			return *valarray;
		}
#endif
#endif
		//TODO: tuple version
	};

	///-------------------------------------------------------------------------------------------------
	/// <summary>	A matlab input archive. </summary>
	///
	/// <remarks>	Neumann, 17.02.2017. </remarks>
	///-------------------------------------------------------------------------------------------------
	class MatlabInputArchive : public InputArchive<MatlabInputArchive>
	{
		//needed so that the detector idom works with clang-cl (for some unknown reason!)
		friend class InputArchive<MatlabOutputArchive>;
		template <class Default, class AlwaysVoid, template<class...> class Op, class... Args> friend struct stdext::DETECTOR;
	public:
		MatlabInputArchive(const std::experimental::filesystem::path &fpath, const MatlabOptions &options = MatlabOptions::read)
			: InputArchive(this), m_MatlabFile(getMatlabFile(fpath, options))  {};
		~MatlabInputArchive() 
		{
			//Cleanup
			while (!mFields.empty())
			{
				if (mFields.size() == 1)
					mxDestroyArray(std::get<1>(mFields.top()));

				mFields.pop();
			}
			matClose(&m_MatlabFile);
		};
		DISALLOW_COPY_AND_ASSIGN(MatlabInputArchive)
		//TODO: Write load function!
		template<typename T>
		inline void load(Archives::NamedValue<T>& value)
		{
			checkCurrentField();				//Need to check if the current field is a struct or not; If not we cannot nest further!
			loadNextField(value.getName());     //Loads the next Field with given name; (Move Down)
			this->operator()(value.getValue()); //Load Data from the Field or struct.
			releaseField();						//Remove the last Fieldname (Move Up)
		};

		template<typename T>
		inline std::enable_if_t<traits::has_getvalue_MATLAB_v<MatlabInputArchive, std::decay_t<T>>> load(T& value)
		{			
			using Type = std::decay_t<T>; // T cannot be const 
			const auto fieldptr = std::get<1>(mFields.top());
			
			//Check Type
			assert(mxGetClassID(fieldptr) == MATLAB::MATLABClassFinder<Type>::value);
			getValue(value, fieldptr);
		}
		template<typename T>
		inline std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>> getValue(T& loadtarget, mxArray const * const field)
		{
			assert(field != nullptr);
			using DataType = std::decay_t<T>;
			loadtarget = *reinterpret_cast<DataType*>(mxGetData(field));
		};
		template<typename T>
		inline std::enable_if_t<stdext::is_string_v<T>> getValue(T& loadtarget, mxArray const * const field)
		{
			assert(field != nullptr);
			const auto length = mxGetN(field);
			auto res = mxArrayToString(field);
			loadtarget = std::string{ std::move(res), length+1 }; // Copy data into string. 
			mxFree(res); //mxArrayToString uses heap memory for the returned char array. 
		};

		template<typename T>
		inline std::enable_if_t<stdext::is_arithmetic_container_v<std::decay_t<T>>> load(T& value)
		{
			//TODO: Check if this will work for all containers containing arithmetic types!
			using Type = std::decay_t<T>; // T cannot be const
			using DataType = typename std::decay_t<typename std::decay_t<T>::value_type>;
			const auto fieldptr = std::get<1>(mFields.top());
			const auto cols = mxGetN(fieldptr);
			const auto rows = mxGetM(fieldptr);

			assert(mxGetNumberOfDimensions(fieldptr) == 2); //Dimensions in Matlab will always have a minimum of two dimensions!
			assert(std::min(cols, rows) == 1); // 1D-Array!
			assert(mxGetClassID(fieldptr) == MATLAB::MATLABClassFinder<DataType>::value);

			const auto size = std::max(cols, rows);
	
			resizeContainer(value, size);
			
			DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(fieldptr));
			assert(dataposition != nullptr);

			for (auto& elem : value)
			{
				elem = *dataposition;
				++dataposition;
			}
		}

#ifdef EIGEN_CORE_H

		template<typename T>
		inline std::enable_if_t<stdext::is_eigen_type_v<std::decay_t<T>>> load(T& value)
		{
			using Type = std::decay_t<T>; // T cannot be const
			using DataType = typename T::Scalar;
			const auto fieldptr = std::get<1>(mFields.top());

			assert(mxGetNumberOfDimensions(fieldptr) == 2);
			assert(mxGetClassID(fieldptr) == MATLAB::MATLABClassFinder<DataType>::value);

			DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(fieldptr));
			assert(dataposition != nullptr);

			const auto cols = mxGetN(fieldptr);
			const auto rows = mxGetM(fieldptr);
			
			assignEigenType(value, dataposition, rows, cols);
		}

		template<typename T>
		inline std::enable_if_t<stdext::is_container_with_eigen_type_v<std::decay_t<T>>> load(T& value)
		{
			using Type = std::decay_t<T>;
			using EigenType = std::decay_t<typename std::decay_t<T>::value_type>; // T cannot be const
			using DataType = typename EigenType::Scalar;
			
			const auto fieldptr = std::get<1>(mFields.top());
			assert(mxGetClassID(fieldptr) == MATLAB::MATLABClassFinder<DataType>::value);
			
			const auto ndims = mxGetNumberOfDimensions(fieldptr);
			const auto dims = mxGetDimensions(fieldptr);

			assert(ndims == 2 || ndims == 3);

			DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(fieldptr));

			if (ndims == 2) //Special 2D Case -> Means that EigenType should be a Vector
			{
				const auto elemsize = dims[0];
				const auto length = dims[1];

				resizeContainer(value,length);
				
				for (auto &elem : value)
				{					
					assignEigenType(elem, dataposition, elemsize,1);
					dataposition += elemsize;	
				}
			}
			else //if (ndims == 3) // 3D Case -> Means that EigenType is a Matrix!
			{				
				const auto rows = dims[0];
				const auto cols = dims[1];
				const auto size = dims[2];

				resizeContainer(value,size);

				for (auto &elem : value)
				{
					assignEigenType(elem, dataposition, rows, cols);
					dataposition += rows * cols;
				}
			}
		}
#ifdef EIGEN_CXX11_TENSOR_TENSOR_H
		template<typename T>
		inline std::enable_if_t<stdext::is_eigen_tensor_v<std::decay_t<T>>> load(Eigen::TensorBase<T>& value)
		{
			using Type = std::decay_t<T>; // T cannot be const
			using DataType = typename T::Scalar;
			using Index = typename T::Index;
			using Dimensions = typename T::Dimensions;
			using ArrayBase = typename T::Dimensions::Base;

			const auto fieldptr = std::get<1>(mFields.top());

			assert(mxGetClassID(fieldptr) == MATLAB::MATLABClassFinder<DataType>::value);

			ArrayBase dimarray = value.dimensions();
		
			if (dimarray[0] == 0) // If the Tensor Type has unintialized dimensions we will use the dimension given by MATLAB to Map to the tensor!
			{
				const auto ndims = mxGetNumberOfDimensions(fieldptr);
				assert(value.NumDimensions == ndims); // Programming error; Or should we partition equally onto the given ndims? Personally I would say error!
				dimarray.data() = mxGetDimensions(fieldptr);
			}
					
			DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(fieldptr));
			assert(dataposition != nullptr);

			/* Reading Data from Array */
			value = Eigen::TensorMap<Type, Eigen::Unaligned>(dataposition, dimarray);
		}
#endif
#endif
	private:
		MATFile &m_MatlabFile;

		using Field = std::tuple<std::string, mxArray * const>;
		std::stack<Field> mFields;	//Using a stack to hold all Fields; Since the implementation is recursive in save().

		MATFile& getMatlabFile(const std::experimental::filesystem::path &fpath, const MatlabOptions &options) const
		{
			assert(options == MatlabOptions::read || options == MatlabOptions::update);// , "Cannot have a MatlabInputArchive with write-only access!");

			if (!fpath.has_filename())
				throw std::runtime_error{ std::string{ "Could not open file due to missing filename: " } +fpath.string() };

			MATFile *pMAT = matOpen(fpath.string().c_str(), MatlabHelper::getMatlabMode(options));

			if (pMAT == nullptr)
				throw std::runtime_error{ std::string{ "Could not open file: " } +fpath.string() };

			return *pMAT;
		};

		inline void checkCurrentField()
		{
			if (mFields.empty()) // We are at File level!
				return;
			else
			{
				const auto& top = mFields.top();
				const auto& arr = std::get<1>(top);
				if (mxIsStruct(arr))
					return;
				else
				{
					const auto& name = std::get<0>(top);
					throw std::runtime_error{ std::string{ "Unable to nest current field. Field is not a struct. Fieldname: " } +name };
				}

			}

		};

		inline void loadNextField(const std::string &str)
		{
			mxArray * nextarr = nullptr;

			if (mFields.empty())
			{
				nextarr = matGetVariable(&m_MatlabFile,str.c_str());
			}
			else
			{
				const auto& top = mFields.top();
				const auto arr = std::get<1>(top);
				nextarr = mxGetField(arr, 0, str.c_str());
			}
			if (nextarr == nullptr)
				throw std::runtime_error{ std::string{ "Could not access field: " } +str };
			mFields.emplace(str, nextarr);
		};

		inline void releaseField() noexcept
		{
			assert(!mFields.empty());

			const auto top = mFields.top();
			const auto arr = std::get<1>(top);

			//Seems like we only need to destroy the last array within the stack. 
			//Trying to delete all other arrays or only the structs gave an access violation exception from MATLAB! 
			if(mFields.size()==1) 
				mxDestroyArray(arr);

			mFields.pop();
		};

		template<typename Container>
		std::enable_if_t<stdext::is_container_v<std::decay_t<Container>>> resizeContainer(Container& cont,const std::size_t& size)
		{
			static_assert(!stdext::is_associative_container_v<std::decay_t<Container>>, "Need to implement assosiative containers in MATLAB. Please do it ;)");
			if constexpr(std::is_same_v<std::decay_t<Container>, std::array<typename Container::value_type, stdext::array_size<Container>>>)
			{
				//std::array is the only fixed size container. So we use an assert here!
				assert(cont.size() == size);
			}
			else if constexpr(stdext::is_resizeable_container_v<std::decay_t<Container>>)
			{
				cont.resize(size);
			}
			else // Brute Force insert or erase! All STL containers which are not resizeable (or std::array) have insert and erase operations!
			{
				if(cont.size() == size) { 
					// -> Do Nothing; we will be overwriting the elements.
				}
				else if (cont.size() < size)
				{	//too few elements
					using DataType = std::decay_t<typename std::decay_t<Container>::value_type>;

					const auto toinsert = size - cont.size();

					for (auto index{ toinsert + 1 }; --index;) // +1 du to predecrement
					{
						cont.insert(cont.end(),DataType{}); //Insert Default constructed object. 
					}
				}
				else
				{ //too many elements
					auto begin = cont.begin();
					begin += size;
					cont.erase(begin, cont.end());
				}
				//static_assert(!stdext::is_resizeable_container_v<std::decay_t<Container>>, "Don't know how to handle containers which are not resizeable yet! Please add it :) ");
			}
		}

#ifdef EIGEN_CORE_H
		template<typename EigenType>
		std::enable_if_t<stdext::is_eigen_type_v<std::decay_t<EigenType>>> assignEigenType(EigenType& value, typename EigenType::Scalar * dataposition, const std::size_t& rows, const std::size_t& cols)
		{
			/* Inserting Data into Array */
			if constexpr (EigenType::IsRowMajor && !EigenType::IsVectorAtCompileTime)
			{
				value = Eigen::Map< EigenType, Eigen::Unaligned, Eigen::Stride<1, EigenType::ColsAtCompileTime> >(dataposition, rows, cols);
			}
			else
			{
				value = Eigen::Map< EigenType, Eigen::Unaligned>(dataposition, rows, cols);
			}
		}
#endif



	};

}

#endif	// INC_Matlab_Archive_H
// end of Matlab_Archive.h
///---------------------------------------------------------------------------------------------------
