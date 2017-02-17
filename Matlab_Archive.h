#pragma once

#pragma once

#include <filesystem>
#include <type_traits>
#include <map>

#include <sstream>
#include <istream>
#include <ostream>

#include <string>
#include <regex>

#include <complex>

#include <exception>

#include <cassert>

#include <Eigen/Core>

#include "../Basic_Library/Headers/std_extensions.h"

#include "../Basic_Library/Headers/BasicMacros.h"
#include "../Basic_Library/Headers/BasicIncludes.h"

//#include "ArchiveHelper.h"
#include "NamedValue.h"
#include "InputArchive.h"
#include "OutputArchive.h"

//#include <engine.h> //To connect to matlab engine (we dont work directly with matlab so does not matter)
//MATLAB includes
#include <mat.h>
#pragma comment (lib, "libmx.lib")
#pragma comment (lib, "libeng.lib")
#pragma comment (lib, "libmex.lib")
#pragma comment (lib, "libmat.lib")
//Needs the following PATH = C:\Program Files\Matlab\R2015b\bin\win64; %PATH%

namespace Archives
{
	namespace traits
	{
		//Member Function type for converting values to strings
		template<class Class, typename ...Args>
		using create_MATLAB_t = decltype( std::declval<Class>().createMATLABArray(std::declval<std::decay_t<Args>>()...) );

		template<typename MATClass, typename Type>
		class has_create_MATLAB : public stdext::is_detected_exact<mxArray&,create_MATLAB_t, MATClass, Type>{};
		template<typename MATClass, typename Type>
		constexpr bool has_create_MATLAB_v = has_create_MATLAB<MATClass, Type>::value;
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
		struct MATLABClassFinder : MATLAB_UnknownClass {};

		template<>
		struct MATLABClassFinder<std::string> : MATLAB_CellClass {};

		template<typename ...Args>
		struct MATLABClassFinder<std::tuple<Args...>> : MATLAB_CellClass {};

		template<>
		struct MATLABClassFinder<const char*> : MATLAB_CharClass {};

		template<>
		struct MATLABClassFinder<bool> : MATLAB_LogicalClass {};

		template<>
		struct MATLABClassFinder<uint8_t> : MATLAB_UInt8Class {};

		template<>
		struct MATLABClassFinder<int8_t> : MATLAB_Int8Class {};

		template<>
		struct MATLABClassFinder<uint16_t> : MATLAB_UInt16Class {};

		template<>
		struct MATLABClassFinder<int16_t> : MATLAB_Int16Class {};

		template<>
		struct MATLABClassFinder<uint32_t> : MATLAB_UInt32Class {};
		template<>
		struct MATLABClassFinder<unsigned long> : MATLAB_UInt32Class {};

		template<>
		struct MATLABClassFinder<int32_t> : MATLAB_Int32Class {};
		template<>
		struct MATLABClassFinder<long> : MATLAB_Int32Class {};

		template<>
		struct MATLABClassFinder<uint64_t> : MATLAB_UInt64Class {};

		template<>
		struct MATLABClassFinder<int64_t> : MATLAB_Int64Class {};

		template<>
		struct MATLABClassFinder<double> : MATLAB_DoubleClass {};

		template<>
		struct MATLABClassFinder<float> : MATLAB_SingleClass {};


	}

	//Enum to represent the different Matlab file modes!
	//read: Opens file for reading only; determines the current version of the MAT-file by inspecting the files and preserves the current version.
	//update: Opens file for update, both reading and writing. If the file does not exist, does not create a file (equivalent to the r+ mode of fopen). Determines the current version of the MAT-file by inspecting the files and preserves the current version.
	//write: Opens file for writing only; deletes previous contents, if any.
	//write_v4: Creates a MAT-file compatible with MATLAB® Versions 4 software and earlier.
	//write_v6: Creates a MAT-file compatible with MATLAB Version 5 (R8) software or earlier. Equivalent to wL mode.
	//write_local :Opens file for writing character data using the default character set for your system. Use MATLAB Version 6 or 6.5 software to read the resulting MAT-file.
	//			     	  If you do not use the wL mode switch, MATLAB writes character data to the MAT-file using Unicode® character encoding by default.
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
				assert(false); //Invalid Enuma!
				return "";
			}
			}() };
			return mode;
		}

	};
	
	class MatlabOutputArchive : public OutputArchive<MatlabOutputArchive>
	{
		friend class OutputArchive<MatlabOutputArchive>;
		//template <typename,typename> friend class traits::has_create_MATLAB;
		
		//needed so that the detector idom works with clang-cl (for some unknown reason!)
		template <class Default, class AlwaysVoid, template<class...> class Op, class... Args> friend struct stdext::DETECTOR;

	public:
		MatlabOutputArchive(const std::experimental::filesystem::path &fpath, const MatlabOptions &options = MatlabOptions::update) 
			: OutputArchive(this), m_filepath(fpath), m_options(options), m_MatlabFile(getMatlabFile(fpath, options)) {};

		~MatlabOutputArchive() 
		{ 

			//Cleanup
			while (!Fields.empty())
			{
				mxDestroyArray(std::get<1>(Fields.top()));
				Fields.pop();
			}
			matClose(&m_MatlabFile);
		};

	
		//Handle single named value
		//template<typename T>
		//inline std::enable_if_t<std::negation<std::is_same<typename std::decay<T>::type::type, Archives::NamedValue<typename std::decay<T>::type::type::type>>::type>::value> save(const Archives::NamedValue<T>& value)
		//{
		//	setNextFieldname(value.getName());
		//	std::cout << "Normal Named Value! " << value.getName() << std::endl;
		//	this->operator()(value.getValue());
		//	clearNextFieldname();
		//};
		//std::enable_if_t<std::is_same<typename std::decay<T>::type::type, Archives::NamedValue<typename std::decay<T>::type::type::type>>::value>
		
		template<typename T>
		inline void save(const Archives::NamedValue<T>& value)
		{
			setNextFieldname(value.getName());
			this->operator()(value.getValue());
			clearNextFieldname();
		};


		template<typename T>
		inline std::enable_if_t<traits::has_create_MATLAB_v<MatlabOutputArchive, std::decay_t<T>>> save(const T& value)
		{
			//TODO: Build Fieldname if none has been set. Important! Otherwise matlab will throw an runtime exception!
			if (nextFieldname.empty()) //We need to create a fieldname 
				nextFieldname = typeid(T).name();

			auto& arrdata = createMATLABArray<std::decay_t<T>>(value);
			
			Fields.push(std::make_tuple(std::move(nextFieldname), &arrdata));

			finishMATLABArray();
		}


	private:
		const std::experimental::filesystem::path m_filepath;
		MatlabOptions m_options;
		MATFile &m_MatlabFile;
		
		using Field = std::tuple<std::string, mxArray*>;
		std::stack<Field> Fields;
	
		std::string nextFieldname;

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
			checkNextFieldname(value);
			startMATLABArray(value);
		};
		template<typename T>
		inline std::enable_if_t<traits::uses_type_save_v<T, MatlabOutputArchive> > epilogue(const T&)
		{
			finishMATLABArray();
		};
		
		//For nested NamedValue
		template<typename T>
		inline std::enable_if_t<std::is_same<typename std::decay<T>::type::type, Archives::NamedValue<typename std::decay<T>::type::type::type>>::value> prologue(const T& value)
		{
			setNextFieldname(value.getName());
			startMATLABArray(value);
			clearNextFieldname();
		};
		template<typename T>
		inline std::enable_if_t<std::is_same<typename std::decay<T>::type::type, Archives::NamedValue<typename std::decay<T>::type::type::type>>::value> epilogue(const T&)
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
		inline void checkNextFieldname(T&& val)
		{
			//TODO: Creat proper fieldname
			if (nextFieldname.empty())
			{

			}
		}

		template<typename T>
		inline std::enable_if_t<!traits::has_create_MATLAB_v<MatlabOutputArchive, std::decay_t<T>>> startMATLABArray(const T&)
		{
			constexpr mwSize ndims = 2;
			constexpr mwSize dims[ndims]{ 1,1 }; // Higher dimensional structs are a bit strange... looks like an array of structs (not what we normally want)
			mxArray *pStruct = mxCreateStructArray(ndims, dims, 0, nullptr);
			if (pStruct == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

			//std::cout << "Creating Field:" << nextFieldname << std::endl;

			if (nextFieldname.empty()) //We may to create a fieldname 
			{
				if (!Fields.empty()) //We have to create a new fieldname 
				{
					nextFieldname = typeid(T).name();
					Fields.push(std::make_tuple(std::move(nextFieldname), pStruct));
				}
				else
				{
					mxDestroyArray(pStruct);
				}
			}
			else
			{
				Fields.push(std::make_tuple(std::move(nextFieldname), pStruct));
			}
		};

		void finishMATLABArray()
		{
			if (Fields.empty())
				return;

			//assert(!Fields.empty());// , "Trying to pop more mxArrays from the stack than had been pushed"); //Programming error!

			auto TopField = Fields.top(); //Thats the field we have to add! (Child mxArray)
			Fields.pop(); //Remove it

			if (Fields.empty()) //at the bottom lvl; write array to mat
			{
				int status = matPutVariable(&m_MatlabFile, std::get<0>(TopField).c_str(), std::get<1>(TopField)); // This throws an expection and catches it itself!
				if (status != 0)
					throw std::runtime_error{ "Unable to write Array to MATLAB file! (Out of Memory?)" };
				mxDestroyArray(std::get<1>(TopField)); //release array and automaticlly destroys all arrays which have been added into the array!
			}
			else // we are recursive
			{
				auto& BottomField = Fields.top(); 
				auto& arr = std::get<1>(BottomField); //Parent mxArray

				if (mxIsStruct(arr))
				{
					auto index = mxAddField(arr, std::get<0>(TopField).c_str());

					if (index == -1)
						throw std::runtime_error{ "Could not add Field to MATLAB struct! (Out of Memory?)" };

					mxSetFieldByNumber(arr, 0, index, std::get<1>(TopField)); // Add Child to parent

					//no destruction of the TopField array here!
				}
				else if (mxIsCell(arr))
				{
					std::cout << "MATLAB case not handled. Debug me or add correct case" << std::endl;
					//TODO: Implement mxArray insert for cell arrays;
					assert(false);// , "Case (Cell) not handeled by Archive currently!");
				}
				else if (mxIsNumeric(arr))
				{
					std::cout << "MATLAB case not handled. Debug me or add correct case" << std::endl;
					//TODO: Implement mxArray insert for numeric arrays;
					assert(false);// , "Case (Numeric) not handeled by Archive currently!");
				}
				else
				{
					std::cout << "Unknown MATLAB case. Debug me or add correct case" << std::endl;
					assert(true);// , "Case (Unknown) not handeled by Archive!");
				}
			}
		}


		template<typename T>
		std::enable_if_t<std::is_arithmetic<std::decay_t<T>>::value, mxArray&> createMATLABArray(const T& value) const
		{
			mxArray *valarray = mxCreateNumericMatrix(1, 1, MATLAB::MATLABClassFinder<std::decay_t<T>>::value, mxREAL);
			if (valarray == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };
			*static_cast<T*>(mxGetData(valarray)) = value;
			return *valarray;
		}

		template<typename T>
		std::enable_if_t<std::is_same<T, std::basic_string<typename T::value_type>>::value ,mxArray&> createMATLABArray(const T& value) const
		{
			mxArray *valarray = mxCreateString(value.c_str());//mxCreateNumericMatrix(1, 1, MATLAB::MATLABClassFinder<std::decay_t<T>>::value, mxREAL);
			
			if (valarray == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

			return *valarray;
		}
		
		template<typename T>
		std::enable_if_t<std::conjunction<stdext::is_container<std::decay_t<T>>, std::is_arithmetic<std::decay_t<typename T::value_type>>,
			std::negation<std::is_same<T, std::basic_string<typename T::value_type>>>>::value, mxArray&> createMATLABArray(const T& value) const
		{
			using DataType = std::decay_t<typename T::value_type>;

			mxArray *valarray = mxCreateNumericMatrix(value.size(),1, MATLAB::MATLABClassFinder<DataType>::value, mxREAL);
			if (valarray == nullptr)
				throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

			DataType * dataposition = mxGetPr(valarray);
			for (const auto& tmp : value)
			{
				*dataposition++ = tmp;
			}

			return *valarray;
		}

#ifdef EIGEN_CORE_H
		template<typename T>
		std::enable_if_t<std::conjunction<stdext::is_container<std::decay_t<T>>, 
										  std::is_base_of<Eigen::EigenBase<std::decay_t<typename T::value_type>>,
														  std::decay_t<typename T::value_type>>>::value, mxArray&>
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

				if (!EigenMatrix::IsRowMajor)
				{
					Eigen::Map< EigenMatrix, Eigen::Unaligned, Eigen::Stride<1, EigenMatrix::ColsAtCompileTime> >(dataposition, tmp.rows(), tmp.cols()) = tmp;
				}
				else
				{
					Eigen::Map< EigenMatrix, Eigen::Unaligned>(dataposition, tmp.rows(), tmp.cols()) = tmp;
				}
				if (EigenMatrix::IsVectorAtCompileTime)
				{
					dataposition += rows > cols ? rows : cols;
				}
				else
				{
					dataposition += rows + cols + 1;
				}
				
			}
																																									
			return *valarray;
		}

		template<typename T>
		std::enable_if_t<std::is_base_of<Eigen::EigenBase<std::decay_t<T>>, std::decay_t<T>>::value, mxArray&> createMATLABArray(const Eigen::EigenBase<T>& value) const
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


	class MatlabInputArchive : public InputArchive<MatlabInputArchive>
	{
	public:
		MatlabInputArchive(const std::experimental::filesystem::path &fpath, const MatlabOptions &options = MatlabOptions::read)
			: InputArchive(this), m_MatlabFile(getMatlabFile(fpath, options))  {};
		~MatlabInputArchive() { matClose(&m_MatlabFile); };
	private:
		MATFile &m_MatlabFile;

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
	};

}