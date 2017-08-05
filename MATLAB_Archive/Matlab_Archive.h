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
		template<class Class, typename ...Args>
		using loadtype_MATLAB_t = decltype(std::declval<Class>().loadType(std::declval<std::decay_t<Args>>()...));
		template<typename MATClass, typename Type>
		class has_loadType_MATLAB : public stdext::is_detected<loadtype_MATLAB_t, MATClass, Type> {};

		//template<typename MATClass, typename Type>
		//class has_loadType_MATLAB : public stdext::is_detected<loadtype_MATLAB_t, MATClass, Type> {};
		template<typename MATClass, typename Type>
		constexpr bool has_loadType_MATLAB_v = has_loadType_MATLAB<MATClass, Type>::value;

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
		//TODO: maybe use Basic_string instead of std::string
		template<>
		struct MATLABClassFinder<std::string> : MATLAB_CellClass {};

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
		friend class OutputArchive<MatlabOutputArchive>;
		//template <typename,typename> friend class traits::has_create_MATLAB;
		
		//needed so that the detector idom works with clang-cl (for some unknown reason!)
		template <class Default, class AlwaysVoid, template<class...> class Op, class... Args> friend struct stdext::DETECTOR;

	public:
		MatlabOutputArchive(const std::experimental::filesystem::path &fpath, const MatlabOptions &options = MatlabOptions::update);
		~MatlabOutputArchive();
		
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


	private:
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
		//inline std::enable_if_t<std::is_same<typename std::decay<T>::type::type, Archives::NamedValue<typename std::decay<T>::type::type::type>>::value> prologue(const T& value)
		{
			setNextFieldname(value.getName());
			startMATLABArray(value);
			clearNextFieldname();
		};
		template<typename T>
		inline std::enable_if_t<is_nested_NamedValue_v<T>> epilogue(const T&)
		//inline std::enable_if_t<std::is_same<typename std::decay<T>::type::type, Archives::NamedValue<typename std::decay<T>::type::type::type>>::value> epilogue(const T&)
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
		std::enable_if_t<std::is_arithmetic<std::decay_t<T>>::value, mxArray&> createMATLABArray(const T& value) const
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
		//std::enable_if_t<stdext::is_string_v<T> ,mxArray&> createMATLABArray(const T& value) const
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
			DataType * dataposition = reinterpret_cast<DataType*>(mxGetPr(valarray));
			for (const auto& tmp : value)
			{
				*dataposition++ = tmp;
			}

			return *valarray;
		}

#ifdef EIGEN_CORE_H
		//Eigen Types 
		template<typename T>
		//std::enable_if_t<std::conjunction<stdext::is_container<std::decay_t<T>>, 
		//								  std::is_base_of<Eigen::EigenBase<std::decay_t<typename T::value_type>>,
		//												  std::decay_t<typename T::value_type>>>::value, mxArray&>
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
		
			if (dataposition == nullptr)
				throw std::runtime_error{ "Unable get data pointer!" };

			//Be aware: of strange storage order!
			for (const auto& tmp : value)
			{
				if (!EigenMatrix::IsRowMajor) {
					Eigen::Map< EigenMatrix, Eigen::Unaligned, Eigen::Stride<1, EigenMatrix::ColsAtCompileTime> >(dataposition, tmp.rows(), tmp.cols()) = tmp;
				}
				else {
					Eigen::Map< EigenMatrix, Eigen::Unaligned>(dataposition, tmp.rows(), tmp.cols()) = tmp;
				}
				if (EigenMatrix::IsVectorAtCompileTime)	{
					dataposition += rows > cols ? rows : cols;
				}
				else {
					dataposition += rows + cols + 1;
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
			if (!T::IsRowMajor && !T::IsVectorAtCompileTime)
			{
				Eigen::Map< T, Eigen::Unaligned, Eigen::Stride<1, T::ColsAtCompileTime> >(dataposition, value.rows(), value.cols()) = value;
			}
			else
			{
				Eigen::Map< T, Eigen::Unaligned>(dataposition, value.rows(), value.cols()) = value;
			}
			return *valarray;
		}
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
		~MatlabInputArchive() { matClose(&m_MatlabFile); };

		//TODO: Write load function!
		template<typename T>
		inline void load(const Archives::NamedValue<T>& value)
		{
			checkCurrentField();				//Need to check if the current field is a struct or not; If not we cannot nest further!
			loadNextField(value.getName());     //Loads the next Field with given name; (Move Down)
			this->operator()(value.getValue()); //Load Data from the Field or struct.
			releaseField();						//Remove the last Fieldname (Move Up)
		};

		template<typename T>
		inline std::enable_if_t<traits::has_loadType_MATLAB_v<MatlabInputArchive,std::remove_reference_t<T>>> load(T& value)
		{			
			using Type = std::remove_reference_t<T>; // T cannot be const 
			const auto fieldptr = std::get<1>(mFields.top());

			//Check Type
			if (mxGetClassID(fieldptr) == MATLABClassFinder<Type>::value)
			{

			}
			else //Wrong type in Field!
			{
				const auto namr = std::get<0>(mFields.top());
				throw std::runtime_error{ std::string{ "Type mismatch. Cannot load value from field: " } +name };
			}
		}

		template<typename T>
		inline void loadType(T& loadtarget)
		{

		}

	private:
		MATFile &m_MatlabFile;

		using Field = std::tuple<std::string, mxArray*>;
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

		}
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
				nextarr = mxGetField(arr, 1, str.c_str());
			}
			if (nextarr == nullptr)
				throw std::runtime_error{ std::string{ "Could not open field: " } +str };
			mFields.emplace(str, nextarr);
		}

		inline void releaseField() noexcept
		{
			assert(!mFields.empty());

			const auto& top = mFields.top();
			const auto arr = std::get<1>(top);
			mxDestroyArray(arr);
			mFields.pop();
		}

		template<typename T>
		inline std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>> loadType(T& loadtarget)
		{
			using DataType = std::decay_t<T>;

		}

	};

}

#endif	// INC_Matlab_Archive_H
// end of Matlab_Archive.h
///---------------------------------------------------------------------------------------------------
