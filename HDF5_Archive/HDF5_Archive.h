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

#include <experimental/filesystem>
#include <type_traits>
#include <utility>
#include <map>
#include <iosfwd>
#include <string>
#include <regex>
#include <complex>
#include <exception>
#include <memory>
#include <stack>
#include <cassert>

//#ifdef EIGEN_CORE_H
//#include <Eigen/Core>
//#endif

//If you want dynamic lib for HDF5 define H5_BUILT_AS_DYNAMIC_LIB
#ifdef _DEBUG
#ifdef H5_BUILT_AS_DYNAMIC_LIB
//#pragma comment(lib,"hdf5_hl_cpp_D.lib")
#pragma comment(lib,"hdf5_cpp_D.lib")
//#pragma comment(lib,"hdf5_hl_D.lib")
#pragma comment(lib,"hdf5_D.lib")
//#pragma comment(lib,"szip.lib")
//#pragma comment(lib,"zlib.lib")
#else
//#pragma comment(lib,"libhdf5_hl_cpp_D.lib")
#pragma comment(lib,"libhdf5_cpp_D.lib")
//#pragma comment(lib,"libhdf5_hl_D.lib")
#pragma comment(lib,"libhdf5_D.lib")
//#pragma comment(lib,"libszip.lib")
//#pragma comment(lib,"libzlib.lib")
#endif
#else
#ifdef H5_BUILT_AS_DYNAMIC_LIB
//#pragma comment(lib,"hdf5_hl_cpp.lib")
#pragma comment(lib,"hdf5_cpp.lib")
//#pragma comment(lib,"hdf5_hl.lib")
#pragma comment(lib,"hdf5.lib")
//#pragma comment(lib,"szip.lib")
//#pragma comment(lib,"zlib.lib")
#else
//#pragma comment(lib,"libhdf5_hl_cpp.lib")
#pragma comment(lib,"libhdf5_cpp.lib")
//#pragma comment(lib,"libhdf5_hl.lib")
#pragma comment(lib,"libhdf5.lib")
//#pragma comment(lib,"libszip.lib")
//#pragma comment(lib,"libzlib.lib")
#endif
#endif

#include <hdf5.h>
#include <hdf5_hl.h>
#include <H5Cpp.h>

#include "stdext/std_extensions.h"
#include "basics/BasicMacros.h"
#include "basics/BasicIncludes.h"

#include "Archive/NamedValue.h"
#include "Archive/InputArchive.h"
#include "Archive/OutputArchive.h"

namespace Archives
{
	namespace traits
	{
		//Member Function type for converting values to strings
		template<class Class, typename ...Args>
		using create_HDF5Dataset_t = decltype(std::declval<Class>().createDataset(std::declval<std::decay_t<Args>>()...));

		template<typename Type>
		class has_create_HDF5Dataset : public stdext::is_detected_exact<void, create_HDF5Dataset_t, std::string, Type> {};
		template<typename Type>
		static constexpr bool has_create_HDF5Dataset_v = has_create_HDF5Dataset<Type>::value;

		//Member Function type for converting values to strings
		template<typename Class,class Type, typename ...Args>
		using get_HDF5Dimension_t = decltype(Class::getDimension(std::declval<Type>()));

		template<typename Class, typename Type>
		class has_get_HDF5Dimension_t : public stdext::is_detected<get_HDF5Dimension_t, Class, Type> {};

	}


	namespace HDF5Detail
	{
		//using  std::integral_constant<const H5::PredType&, H5::PredType::NATIVE_DOUBLE> =	H5_DoubleClass;
		/*using  std::integral_constant<mxClassID, mxCELL_CLASS>		MATLAB_CellClass;
		using  std::integral_constant<mxClassID, mxSTRUCT_CLASS>	MATLAB_StructClass;
		using  std::integral_constant<mxClassID, mxLOGICAL_CLASS>	MATLAB_LogicalClass;
		using  std::integral_constant<mxClassID, mxCHAR_CLASS>		MATLAB_CharClass;
		using  std::integral_constant<mxClassID, mxVOID_CLASS>		MATLAB_VoidClass;
		using  std::integral_constant<mxClassID, mxDOUBLE_CLASS>	MATLAB_DoubleClass;
		using  std::integral_constant<mxClassID, mxSINGLE_CLASS>	MATLAB_SingleClass;
		using  std::integral_constant<mxClassID, mxINT8_CLASS>		MATLAB_Int8Class;
		using std::integral_constant<mxClassID, mxUINT8_CLASS>	MATLAB_UInt8Class;
		using std::integral_constant<mxClassID, mxINT16_CLASS>	MATLAB_Int16Class;
		using std::integral_constant<mxClassID, mxUINT16_CLASS>	MATLAB_UInt16Class;
		using std::integral_constant<mxClassID, mxINT32_CLASS>	MATLAB_Int32Class;
		using std::integral_constant<mxClassID, mxUINT32_CLASS>	MATLAB_UInt32Class;
		using std::integral_constant<mxClassID, mxINT64_CLASS>	MATLAB_Int64Class;
		using std::integral_constant<mxClassID, mxUINT64_CLASS>	MATLAB_UInt64Class;
		using std::integral_constant<mxClassID, mxFUNCTION_CLASS>	MATLAB_FunctionClass;
		using std::integral_constant<mxClassID, mxOPAQUE_CLASS>	MATLAB_OpaqueClass;
		using std::integral_constant<mxClassID, mxOBJECT_CLASS>	MATLAB_ObjectClass;
		using std::integral_constant<mxClassID, mxINDEX_CLASS>	MATLAB_IndexClass;*/

		template <typename T>
		struct PredTypeSelector;
		
		//TODO:: Do we need to check endianess of the types if we write it into the file? (Could be in issue with different architectures)

		template<>
		struct PredTypeSelector<std::int8_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_INT8; };
		};
		template<>
		
		struct PredTypeSelector<std::uint8_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_UINT8; };
		};

		template<>
		struct PredTypeSelector<std::int16_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_INT16; };
		};

		template<>
		struct PredTypeSelector<std::uint16_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_UINT16; };;
		};

		template<>
		struct PredTypeSelector<std::int32_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_INT32; };
		};

		template<>
		struct PredTypeSelector<std::uint32_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_UINT32; };
		};

		template<>
		struct PredTypeSelector<std::int64_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_INT64; };
		};

		template<>
		struct PredTypeSelector<std::uint64_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_UINT64; };
		};

		template<>
		struct PredTypeSelector<std::float_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_FLOAT; };
		};

		template<>
		struct PredTypeSelector<std::double_t>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_DOUBLE; };
		};

		template<>
		struct PredTypeSelector<long double>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_LDOUBLE; };
		};

		template<>
		struct PredTypeSelector<std::string>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_CHAR; };
		};

		template<>
		struct PredTypeSelector<const char*>
		{
			static inline const H5::PredType& value() noexcept { return H5::PredType::NATIVE_CHAR; };
		};

		template<typename T>
		class DataTypeSelector
		{
			static_assert(true, "HDF5 Archive does not know the DataType");
		};

		template<>
		struct DataTypeSelector<std::int8_t>
		{
			using type = H5::IntType;
		};

		template<>
		struct DataTypeSelector<std::uint8_t>
		{
			using type = H5::IntType;
		};

		template<>
		struct DataTypeSelector<std::int16_t>
		{
			using type = H5::IntType;
		};

		template<>
		struct DataTypeSelector<std::uint16_t>
		{
			using type = H5::IntType;
		};

		template<>
		struct DataTypeSelector<std::int32_t>
		{
			using type = H5::IntType;
		};

		template<>
		struct DataTypeSelector<std::uint32_t>
		{
			using type = H5::IntType;
		};

		template<>
		struct DataTypeSelector<std::int64_t>
		{
			using type = H5::IntType;
		};

		template<>
		struct DataTypeSelector<std::uint64_t>
		{
			using type = H5::IntType;
		};

		template<>
		struct DataTypeSelector<std::float_t>
		{
			using type = H5::FloatType;
		};

		template<>
		struct DataTypeSelector<std::double_t>
		{
			using type = H5::FloatType;
		};

		template<>
		struct DataTypeSelector<long double>
		{
			using type = H5::FloatType;
		};

		template<>
		struct DataTypeSelector<std::string>
		{
			using type = H5::StrType;
		};

		template<>
		struct DataTypeSelector<const char*>
		{
			using type = H5::StrType;
		};

		//Not specialized
		template<typename T, typename = void>
		struct DataSpaceTypeSelector
		{
			static constexpr inline H5S_class_t value() { return H5S_NO_CLASS; };
			static inline H5::DataSpace dataspace() { return H5::DataSpace{ value() }; };
		};

		//Arithmetic Types
		template<typename T>
		struct DataSpaceTypeSelector<T, std::enable_if_t<std::is_arithmetic<T>::value>>
		{
			static constexpr inline H5S_class_t value() { return H5S_SCALAR; };
			static inline H5::DataSpace dataspace() { return H5::DataSpace{ value() }; };
		};

		//Container Types
		template<typename T>
		struct DataSpaceTypeSelector<T, std::enable_if_t<stdext::is_container<T>::value>>
		{
			static constexpr inline H5S_class_t value() { return H5S_SIMPLE; };
			static inline H5::DataSpace dataspace() { return H5::DataSpace{ value() }; };
		};
		//std::string::allocator_type;
		//std::string::traits_type;
		//std::string::value_type;

		//Strings
		template<typename T>
		struct DataSpaceTypeSelector<T, std::enable_if_t<std::is_same<T, std::basic_string<typename T::value_type, typename T::traits_type, typename T::allocator_type>>::value> >
		{
			static constexpr inline H5S_class_t value() { return H5S_SCALAR; };
			static inline H5::DataSpace dataspace() { return H5::DataSpace{ value() }; };
		};


		
		struct DimensionCalculator
		{
			template<typename T>
			static inline std::enable_if_t<std::is_arithmetic<std::remove_reference_t<T>>::value, std::vector<std::size_t>> getDimension(const T&)
			{
				return std::vector<std::size_t>{ 1 };
			};

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets the Dimension of the value/type. The first element of the array is the dimension
			/// 			from the inner type/value whereas the last value is the dimension of the outmost type/value  </summary>
			///
			/// <typeparam name="T">	Generic type parameter. </typeparam>
			/// <param name="val">	[in] The value. </param>
			///
			/// <returns>	The dimension. </returns>
			///-------------------------------------------------------------------------------------------------
			template<typename T>
			static inline std::enable_if_t< std::conjunction<stdext::is_container<std::remove_reference_t<T>>,
				traits::has_get_HDF5Dimension_t<DimensionCalculator, typename std::remove_reference_t<T>::value_type  > >::value, std::vector<std::size_t>> getDimension(const T& val)
			{
				using ElementType = typename std::remove_reference_t<T>::value_type;

				std::vector<std::size_t> bigdim;

				for (const auto& elem : val)
				{
					if (std::is_arithmetic<std::remove_reference_t<ElementType>>::value) // In this case the we can skip the loop since it is a container of single elements
					{
						return std::vector<std::size_t>{val.size()};
					}
					else
					{
						auto test = DimensionCalculator::getDimension(elem); // Vector with dimensions of elem
						
						if (bigdim.empty()) //Init bigdim once
						{
							bigdim.resize(test.size(), 1);
						}

						auto it2 = bigdim.begin();
						for (auto it = test.begin(); it != test.end() || it2 != bigdim.end(); it++, it2++)
						{
							if (*it > *it2)
							{
								*it2 = *it;
							}
						}
					}
				}

				if (bigdim.size() <= 1)
					return std::vector<std::size_t>{bigdim.front(), val.size()};

				//std::reverse(bigdim.begin(), bigdim.end());
				bigdim.push_back(val.size());
				//std::reverse(bigdim.begin(), bigdim.end());

				return bigdim;
			};

			template<typename T>
			static inline std::enable_if_t<traits::has_get_HDF5Dimension_t<DimensionCalculator, std::remove_reference_t<T>>::value, std::vector<std::size_t>> getDimensionOuterFirst(const T& val)
			{
				auto dims = getDimension(val);
				std::reverse(dims.begin(), dims.end());
				return dims;
			}

		};

	}

	class HDF5_OutputArchive; // Forward declare archive for Options;

	class HDF5_OutputOptions : OutputArchive_Options<HDF5_OutputOptions, HDF5_OutputArchive>
	{
	public:
		unsigned int flags{ H5F_ACC_TRUNC };
		//Valid values of flags include:
		//
		//		H5F_ACC_TRUNC - Truncate file, if it already exists, erasing all data previously stored in the file.
		//		H5F_ACC_EXCL - Fail if file already exists.H5F_ACC_TRUNC and H5F_ACC_EXCL are mutually exclusive
		//		H5F_ACC_RDONLY - Open file as read - only, if it already exists, and fail, otherwise
		//		H5F_ACC_RDWR - Open file for read / write, if it already exists, and fail, otherwise
		//
		H5::FileCreatPropList createPropsList{ H5::FileCreatPropList::DEFAULT };
		H5::FileAccPropList   accesPropsList{ H5::FileAccPropList::DEFAULT };
	};

	class HDF5_OutputArchive : public OutputArchive<HDF5_OutputArchive>
	{
	private:
		HDF5_OutputOptions		  _options;

		std::stack<std::unique_ptr<H5::CommonFG> > _groupstack;

		const H5::H5File&		  _file;

		H5::DataSet   _currentdataset;
		bool		  _datasetopened{ false };
		//bool		  _creategroup{ false };

	public:
		HDF5_OutputArchive(const std::experimental::filesystem::path &path, const HDF5_OutputOptions& options = HDF5_OutputOptions{})
			: OutputArchive(this), _options(options), _groupstack(createStack(path,options)) , _file(getFile())
		{
#ifndef _DEBUG
			H5::Exception::dontPrint();
#endif
			///* Save old error handler */
			//herr_t(*old_func)(void*);
			//void *old_client_data;
			//H5::Exception::getAutoPrint(&old_func, &old_client_data);
			///* Restore old error handler */
			//H5::Exception::setAutoPrint(&old_func, &old_client_data);
		};

		~HDF5_OutputArchive() 
		{
			//If programming is correct the groupstack should now contain only one element!
			assert(_groupstack.size() == 1);
			//_groupstack.pop(); // This should pop _file from the stack!
		};

		template<typename T>
		inline void save(const Archives::NamedValue<T>& val)
		{
			this->operator()(val.getValue());
		}
		
		template<typename T>
		inline void prologue(const Archives::NamedValue<T>& val)
		{
			try
			{
				openDatasetOrGroup(val.getName(), val.getValue());
			}
			catch (H5::Exception& err)
			{
				throw std::runtime_error{ err.getCDetailMsg() };
			}
			catch (...)
			{
				std::cerr << "Unknown error!" << std::endl;
				H5::Exception::printErrorStack();
			}
		}

		template<typename T>
		inline void epilogue(const Archives::NamedValue<T>& val)
		{
			if (_datasetopened) // If we are at dataset level we close the dataset!
			{
				_currentdataset = H5::DataSet{}; //Reset Dataset
				_datasetopened = false;
			}
			else // If we are not at datasetlevel we are going down one in the group stack
			{
				_groupstack.pop();
			}
		}

		template<typename T>
		inline std::enable_if_t<std::is_arithmetic<T>::value> save(const T& val)
		{
			const auto memtype = HDF5Detail::PredTypeSelector<std::decay_t<T>>::value();
			const auto memspace = HDF5Detail::DataSpaceTypeSelector<std::decay_t<T>>::dataspace();
			const auto filespace = HDF5Detail::DataSpaceTypeSelector<std::decay_t<T>>::dataspace();
			_currentdataset.write(&val, memtype, memspace, filespace);
		}

		//template<typename T>
		//inline std::enable_if_t < std::conjunction< stdext::is_container<T>, std::is_arithmetic<std::remove_reference_t<T::value_type>> > > save(const T& val)
		//{

		//}

		private:
			// The HDF5 C++ Wrapper does not allow exception free finding of Groups and Datasets !
			bool hasGroup(const std::string& str)
			{
				hid_t dataset_id = H5Gopen2(_groupstack.top()->getLocId(), str.c_str(), H5P_DEFAULT);
				return (H5I_INVALID_HID != dataset_id);
			};
			bool hasDataset(const std::string& str)
			{
				return (H5Lexists(_groupstack.top()->getLocId(), str.c_str(), H5P_DEFAULT) > 0 ? true : false);
			};

			//Open dataset
			template <typename T>
			inline void openDatasetOrGroup(const std::string& name, T&& val)
			{
				if (hasDataset(name))
				{
					_currentdataset = _groupstack.top()->openDataSet(name); //openDataSet will throw if DataSet is not found!
				}
				else
				{
					createDataset(name, val);
				}
				_datasetopened = true;
			};

			//Open Group (nested NamedValue case) 
			template <typename T> 
			inline void openDatasetOrGroup(const std::string& name, Archives::NamedValue<T>& val)
			{
				if (hasGroup(name))
				{
					auto openedgroup = _groupstack.top()->openGroup(val.getName()); 	//Will throw exception if group is not found!
					_groupstack.push(openedgroup); // will not be executed if exception is thrown!
				}
				else
				{
					auto createdgroup = _groupstack.top()->createGroup(groupname);
					_groupstack.push(createdgroup);
				}
			};

			template <typename T>
			inline std::enable_if_t<std::is_arithmetic<std::remove_reference_t<T>>::value> createDataset(const std::string& datasetname, T&& val)
			{
				const auto datatype = HDF5Detail::PredTypeSelector<std::decay_t<T>>::value();
				const auto dataspace = HDF5Detail::DataSpaceTypeSelector<std::decay_t<T>>::dataspace();
				_currentdataset = _groupstack.top()->createDataSet(datasetname, datatype, dataspace);
			}

			//Create the stack of unique ptrs and start with the file!
			inline std::stack<std::unique_ptr<H5::CommonFG> > createStack(const std::experimental::filesystem::path &path, const HDF5_OutputOptions& options = HDF5_OutputOptions{})
			{
				auto File = std::make_unique<H5::H5File>(path.string(), options.flags, options.createPropsList, options.accesPropsList);
				std::stack<std::unique_ptr<H5::CommonFG> > ret;
				ret.push(std::move(File));
				return ret;
			}

			//Just to have easier access to the File settings and so on! (Only to be used in constructor initializier list!)
			inline H5::H5File& getFile()
			{
				return dynamic_cast<H5::H5File&>(*(_groupstack.top()));
			}

	};


	class HDF5_InputArchive : public InputArchive<HDF5_InputArchive>
	{
	private:
		H5::H5File _file;
	public:
		HDF5_InputArchive(const std::experimental::filesystem::path &path)
			: InputArchive(this), _file(path.string(), H5F_ACC_RDONLY)
		{};
	};

}

#endif	// INC_HDF5_Archive_H

///---------------------------------------------------------------------------------------------------
