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

#include <algorithm>
#include <functional>
#include <numeric>

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
	class HDF5_OutputArchive;	// Forward declare archive for Options;
	class HDF5_InputArchive;	// Forward declare archive for Options;

///-------------------------------------------------------------------------------------------------
/// <signature>	HDF5_traits</signature>
///
/// <summary>	defines type traits for the HDF5 archives </summary>
///-------------------------------------------------------------------------------------------------
	namespace HDF5_traits
	{
		//Member Function type for converting values to strings
		template<class Class, typename Args>
		using write_to_HDF5_t = decltype(std::declval<Class>().write(std::declval<std::decay_t<Args&>>()));

		template<typename Type>
		class has_write_to_HDF5 : public stdext::is_detected_exact<void, write_to_HDF5_t, HDF5_OutputArchive, Type> {};
		template<typename Type>
		static constexpr bool has_write_to_HDF5_v = has_write_to_HDF5<Type>::value;

		template<class Class, typename Args>
		using getData_from_HDF5_t = decltype(std::declval<Class>().getData(std::declval<Args&>()));
		template<typename Type>
		class has_getData_from_HDF5 : public stdext::is_detected_exact<void, getData_from_HDF5_t, HDF5_InputArchive, Type> {};
		template<typename Type>
		static constexpr bool has_getData_from_HDF5_v = has_getData_from_HDF5<Type>::value;
	}
	

	/*****************************************************************************************/
	/****************** Output Archive									 *********************/
	/*****************************************************************************************/

	///-------------------------------------------------------------------------------------------------
	/// <summary>	A hdf 5 output options. </summary>
	///
	/// <seealso cref="T:OutputArchive_Options{HDF5_OutputOptions, HDF5_OutputArchive}"/>
	///-------------------------------------------------------------------------------------------------
	class HDF5_OutputOptions : OutputArchive_Options<HDF5_OutputOptions, HDF5_OutputArchive>
	{
		friend class OutputArchive<HDF5_OutputOptions>;
		//needed so that the detector idom works with clang-cl (for some unknown reason!)
		template <class Default, class AlwaysVoid, template<class...> class Op, class... Args> friend struct stdext::DETECTOR;

	public:
		HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode FileCreationMode{ HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode::CreateOrOverwrite };
		HDF5_Wrapper::HDF5_DatatypeOptions			 DefaultDatatypeOptions{};
		HDF5_Wrapper::HDF5_DataspaceOptions			 DefaultDataspaceOptions{};
	};

	///-------------------------------------------------------------------------------------------------
	/// <summary>	HDF5 output archive. </summary>
	///
	/// <seealso cref="T:OutputArchive{HDF5_OutputArchive}"/>
	///-------------------------------------------------------------------------------------------------
	class HDF5_OutputArchive : public OutputArchive<HDF5_OutputArchive>
	{
		using ThisClass = HDF5_OutputArchive;
	public:
		HDF5_OutputArchive(const std::experimental::filesystem::path &path, const HDF5_OutputOptions& options)
			: OutputArchive(this), mFile(openOrCreateFile(path, options)), mOptions(options) {
			static_assert(std::is_same_v<ThisClass, std::decay_t<decltype(*this)>>);
		};

		DISALLOW_COPY_AND_ASSIGN(HDF5_OutputArchive)

		template<typename T>
		inline void save(const Archives::NamedValue<T>& value)
		{
			setNextPath(value.getName());		//Sets the name for the next Dataset or Group
			this->operator()(value.getValue()); //Write Data to the Group or Dataset
			clearNextPath();					//Remove the Last Fieldname
		};

		template<typename T>
		inline std::enable_if_t< HDF5_traits::has_write_to_HDF5<std::decay_t<T>>::value > save(const T& value)
		{
			write(value);
		};
	
		//If the type does not have a write to HDF5 function here it means we need to create/open a Group!
		template<typename T>
		inline std::enable_if_t<(is_nested_NamedValue_v<T> || !traits::use_archive_member_save_v<T, ThisClass> )>
			prologue(const T& value)
		{
			if constexpr(is_nested_NamedValue_v<T>)
				setNextPath(value.getName());
			
			createOrOpenGroup(value);

			clearNextPath();
		};

		template<typename T>
		inline std::enable_if_t<(is_nested_NamedValue_v<T> || !traits::use_archive_member_save_v<T, ThisClass>) >
			epilogue(const T& value)
		{
			closeLastGroup(value);
		};

	private:
		
		using CurrentGroup = HDF5_Wrapper::HDF5_GroupWrapper;
		using File = HDF5_Wrapper::HDF5_FileWrapper;
		
		File mFile;
		std::stack<CurrentGroup> mGroupStack;
		std::string nextPath;
		HDF5_OutputOptions mOptions;
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
	
			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();
			HDF5_GroupOptions opts;
			opts.mode = HDF5_GeneralOptions::HDF5_Mode::OpenOrCreate;
			HDF5_GroupWrapper group(currentLoc, nextPath, opts );
			mGroupStack.push(std::move(group));
		}

		template<typename T>
		void closeLastGroup(const T&)
		{
			assert(!mGroupStack.empty());
			mGroupStack.pop();
		}
		
		//template<typename T>
		//HDF5_Wrapper::HDF5_DatasetWrapper createDataset(const T& val)
		//{
		//	// TODO: Maybe remove this function and but it all into write.

		//	//Open or create the Dataset here!
		//	using namespace HDF5_Wrapper;


		//	const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ?  static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top() ;

		//	const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

		//	else if constexpr(stdext::is_eigen_tensor_v<std::decay_t<T>>)
		//	{
		//		const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
		//		HDF5_DataspaceOptions dataspaceopts;
		//		const auto valdims = val.dimensions();
		//		dataspaceopts.dims.resize(valdims.size() );
		//		dataspaceopts.maxdims.resize(valdims.size() );
		//		//std::copy(valdims.begin(), valdims.end(), dataspaceopts.dims.begin());
		//		//std::copy(valdims.begin(), valdims.end(), dataspaceopts.maxdims.begin());
		//		std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.dims.begin());
		//		std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.maxdims.begin());
		//		HDF5_DataspaceWrapper dataspace(dataspacetype, dataspaceopts);
		//		HDF5_DatasetOptions datasetopts(HDF5_DatatypeWrapper(*val.data(), datatypeopts), std::move(dataspace));
		//		HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));
		//		return dataset;
		//	}
		//	else 
		//	{
		//		//static_assert(false);
		//
		//		assert(false);
		//		//TODO: Check if dataset already exists! 
		//		//Creating the dataspace!
		//		const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
		//		const HDF5_DataspaceOptions dataspaceopts;

		//		//Creating the dataset!
		//		HDF5_DatasetOptions datasetopts(HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts));
		//		HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));
		//		return dataset;
		//	}
		//}

		public: // For some reason the write functions must be public for clang to detect that the class can use them.
		template <typename T>
		std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>> || stdext::is_complex_v<std::decay_t<T>> > write(const T& val)
		{
			using namespace HDF5_Wrapper;

			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			//Creating the dataset! 
			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
			const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const HDF5_DataspaceOptions dataspaceopts;
			HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
			HDF5_DatasetOptions datasetopts;
			HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(storeopts), std::move(datasetopts));

			//Creating the Memory space
			const auto memoryspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };
			const HDF5_DataspaceOptions memoryspaceopt;
			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val, memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };

			//Write the Data
			dataset.writeData(val, memoryopts);
		}

		template <typename T>
		std::enable_if_t<stdext::is_string_v<std::decay_t<T>>> write(const T& val)
		{
			using namespace HDF5_Wrapper;

			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			//Creating the dataset! 
			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
			const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const HDF5_DataspaceOptions dataspaceopts;
			HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
			HDF5_DatasetOptions datasetopts;
			HDF5_DatasetWrapper dataset(currentLoc, nextPath,  std::move(storeopts), std::move(datasetopts));

			//Creating the Memory space
			const auto memoryspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };
			const HDF5_DataspaceOptions memoryspaceopt;
			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val, memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };

			dataset.writeData(val.c_str(), memoryopts);
		}

		template <typename T>
		std::enable_if_t<stdext::is_arithmetic_container_v<std::decay_t<T>>> write(const T& val)
		{
			if constexpr (stdext::is_memory_sequentiel_container_v<std::decay_t<T>>)
			{
				using namespace HDF5_Wrapper;

				const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

				//Creating the dataset! 
				const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
				const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
				
				//Settings storage dimensions
				HDF5_DataspaceOptions dataspaceopts;
				dataspaceopts.dims = std::vector<std::size_t>{ { val.size() } };
				dataspaceopts.maxdims = std::vector<std::size_t>{ { val.size() } };

				HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(*val.begin(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
				HDF5_DatasetOptions datasetopts;
				HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(storeopts), std::move(datasetopts));

				//Creating the memory space
				const auto memoryspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
				const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };

				//Settings memory dimensions
				HDF5_DataspaceOptions memoryspaceopt;
				memoryspaceopt.dims = std::vector<std::size_t>{ { val.size() } };
				memoryspaceopt.maxdims = std::vector<std::size_t>{ { val.size() } };
				HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val[0], memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };

				dataset.writeData(val, memoryopts);
			}
			else
			{
				static_assert(!stdext::is_memory_sequentiel_container_v<std::decay_t<T>>, "Case not implemented yet");
			}
		}

		template <typename T>
		std::enable_if_t<stdext::is_eigen_type_v<std::decay_t<T>>> write(const T& val)
		{
			using namespace HDF5_Wrapper;
			
			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			//Creating the dataset! 
			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
			const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();

			//Settings storage dimensions
			HDF5_DataspaceOptions dataspaceopts;
			dataspaceopts.dims = std::vector<std::size_t>{ { static_cast<std::size_t>(val.rows()), static_cast<std::size_t>(val.cols()) } };
			dataspaceopts.maxdims = std::vector<std::size_t>{ { static_cast<std::size_t>(val.rows()), static_cast<std::size_t>(val.cols()) } };

			HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
			HDF5_DatasetOptions datasetopts;
			HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(storeopts), std::move(datasetopts));
			
			//Creating the memory space
			const auto memoryspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };

			//Settings memory dimensions
			HDF5_DataspaceOptions memoryspaceopt;
			memoryspaceopt.dims = std::vector<std::size_t>{ { static_cast<std::size_t>(val.rows()), static_cast<std::size_t>(val.cols()) } };
			memoryspaceopt.maxdims = std::vector<std::size_t>{ { static_cast<std::size_t>(val.rows()), static_cast<std::size_t>(val.cols()) } };
			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };

			//HDF5_DataspaceWrapper memoryspace(memoryspacetype, memoryspaceopt);

			if constexpr (!(T::IsRowMajor) && !T::IsVectorAtCompileTime)
			{ //Converting from Columnmajor to rowmajor
				Eigen::Matrix<typename T::Scalar, T::RowsAtCompileTime, T::ColsAtCompileTime, Eigen::RowMajor> TransposedMatrix = val;
				//HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), memorytypeopts), std::move(memoryspace) };
				dataset.writeData(TransposedMatrix, memoryopts);		
			}
			else
			{
				//HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), memorytypeopts), std::move(memoryspace) };
				dataset.writeData(val, memoryopts);
			}
		}

		template <typename T>
		std::enable_if_t<stdext::is_eigen_tensor_v<std::decay_t<T>>> write(const T& val)
		{
			using namespace HDF5_Wrapper;

			using namespace HDF5_Wrapper;

			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			//Creating the dataset! 
			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
			const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();

			//Settings storage dimensions
			HDF5_DataspaceOptions dataspaceopts;
			const auto valdims = val.dimensions();
			dataspaceopts.dims.resize(valdims.size());
			dataspaceopts.maxdims.resize(valdims.size());
			std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.dims.begin());
			std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.maxdims.begin());

			HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
			HDF5_DatasetOptions datasetopts;
			HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(storeopts), std::move(datasetopts));

			//Creating the memory space
			const auto memoryspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };

			//Settings memory dimensions
			HDF5_DataspaceOptions memoryspaceopt;
			memoryspaceopt.dims.resize(valdims.size());
			memoryspaceopt.maxdims.resize(valdims.size());
			std::reverse_copy(valdims.begin(), valdims.end(), memoryspaceopt.dims.begin());
			std::reverse_copy(valdims.begin(), valdims.end(), memoryspaceopt.maxdims.begin());
			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };
;
			dataset.writeData(*val.data(), memoryopts);
		};
	};



	/*****************************************************************************************/
	/****************** Input Archive									 *********************/
	/*****************************************************************************************/

	class HDF5_InputOptions : InputArchive_Options<HDF5_InputOptions, HDF5_InputArchive>
	{
	public:
		HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode FileCreationMode{ HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode::Open };
		HDF5_Wrapper::HDF5_DatatypeOptions			 DefaultDatatypeOptions{};
		HDF5_Wrapper::HDF5_DataspaceOptions			 DefaultDataspaceOptions{};
	};


	class HDF5_InputArchive : public InputArchive<HDF5_InputArchive>
	{
		using ThisClass = HDF5_InputArchive;
		friend class OutputArchive<ThisClass>;
		//needed so that the detector idom works with clang-cl (for some unknown reason!)
		template <class Default, class AlwaysVoid, template<class...> class Op, class... Args> friend struct stdext::DETECTOR;
		
	public:
		HDF5_InputArchive(const std::experimental::filesystem::path &path, const HDF5_InputOptions& options)
			: InputArchive(this), mFile(openFile(path, options)), mOptions(options) {
			static_assert(std::is_same_v<ThisClass, std::decay_t<decltype(*this)>>);
		};

		DISALLOW_COPY_AND_ASSIGN(HDF5_InputArchive)

		template<typename T>
		inline void load(Archives::NamedValue<T>& value)
		{
			//if constexpr(is_NamedValue_v<T>)
			//{
			//	setNextPath(value.getName());
			//	openGroup(value);
			//	clearNextPath();

			//	this->operator()(value.getValue());
			//}
			//else
			//{
				setNextPath(value.getName());		//Sets the name for the next Dataset or Group
				this->operator()(value.getValue()); //Write Data to the Group or Dataset
				clearNextPath();	//Remove the Fieldname
			//}
		};

		template<typename T>
		inline std::enable_if_t< HDF5_traits::has_getData_from_HDF5_v<std::decay_t<T>> > load(T& value)
		{
			getData(value);
		};

		//If the type does not have a write to HDF5 function here it means we need to create/open a Group!
		template<typename T>
		inline std::enable_if_t<(is_nested_NamedValue_v<std::decay_t<T>> || !traits::use_archive_member_load_v<std::decay_t<T>, ThisClass>)>
			prologue(const T& value)
		{
			if constexpr(is_nested_NamedValue_v<T>)
				setNextPath(value.getName());

			openGroup(value);

			clearNextPath();
		};

		template<typename T>
		inline std::enable_if_t<(is_nested_NamedValue_v<std::decay_t<T>> || !traits::use_archive_member_load_v<std::decay_t<T>, ThisClass>) >
			epilogue(const T&)
		{
			closeLastGroup();
		};

	private:
		using CurrentGroup = HDF5_Wrapper::HDF5_GroupWrapper;
		//using LastDataset = HDF5_Wrapper::HDF5_DatasetWrapper;
		using File = HDF5_Wrapper::HDF5_FileWrapper;

		File							mFile;
		std::stack<CurrentGroup>		mGroupStack;
		std::string nextPath;

		HDF5_InputOptions mOptions;

		static File openFile(const std::experimental::filesystem::path &path, const HDF5_InputOptions& options)
		{
			using namespace HDF5_Wrapper;
			HDF5_FileOptions opt{};
			opt.mode = options.FileCreationMode;

			File file{ path, opt };
			return file;
		}

		//TODO: Move those function into another class which can also be used by the Output Archive
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
		bool isValidNextLocation(const std::string&) const
		{
			//TODO: Check the Location name for invalid characters
			return true;
		}

		template<typename T>
		void openGroup(const T&)
		{
			using namespace HDF5_Wrapper;

			assert(!nextPath.empty());
			
			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			HDF5_GroupOptions opts;
			opts.mode = HDF5_GeneralOptions::HDF5_Mode::Open;
			HDF5_GroupWrapper group{ currentLoc, nextPath, opts };
			mGroupStack.push(std::move(group));
		}

		void closeLastGroup()
		{
			assert(!mGroupStack.empty());
			mGroupStack.pop(); // Just pop it from the stack. The Destructor will close it!
		};

		template<typename T>
		std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>> || stdext::is_complex_v<std::decay_t<T>> > getData(T& val)
		{
			using namespace HDF5_Wrapper;

			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			const auto spacetype = DataspaceTypeSelector<std::decay_t<T>>::value();

			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

			HDF5_DatasetOptions datasetopts{};
			HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));
			
			assert(dataset.getDatatype() == HDF5_DatatypeWrapper(val, datatypeopts));

			const auto& dataspace{ dataset.getDataspace() };

			assert(dataspace.getDimensions().size() <= 1);

			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(spacetype) };
			dataset.readData(val, memoryopts);
		}

		template<typename T>
		std::enable_if_t<stdext::is_string_v<std::decay_t<T>> > getData(T& val)
		{
			using namespace HDF5_Wrapper;

			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			const auto spacetype = DataspaceTypeSelector<std::decay_t<T>>::value();

			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

			HDF5_DatasetOptions datasetopts{};
			HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));

			assert(dataset.getDatatype() == HDF5_DatatypeWrapper(val, datatypeopts));
			
			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(spacetype) };
			dataset.readData(val, memoryopts);
		}

		template<typename T>
		std::enable_if_t<stdext::is_arithmetic_container_v<std::decay_t<T>> &&
						 stdext::is_memory_sequentiel_container_v<std::decay_t<T>> > getData(T& val)
		{
			using namespace HDF5_Wrapper;

			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			const auto spacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			HDF5_DataspaceOptions spaceopts;

			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

			HDF5_DatasetOptions datasetopts{};
			HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));

			HDF5_DatatypeWrapper type(val[0], datatypeopts);
			assert(dataset.getDatatype() == type);

			const auto& dataspace{ dataset.getDataspace() };
			const auto dims = dataspace.getDimensions();

			assert(dims.size() == 1);

			val.resize(dims.at(0));
				
			HDF5_DataspaceOptions memoryspaceopt;
			memoryspaceopt.dims = std::vector<std::size_t>{ { val.size() } };
			memoryspaceopt.maxdims = std::vector<std::size_t>{ { val.size() } };
			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val[0], datatypeopts), HDF5_DataspaceWrapper(spacetype, memoryspaceopt) };
			dataset.readData(val.data(), memoryopts);
		}

		template<typename T>
		std::enable_if_t<stdext::is_eigen_type_v<std::decay_t<T>>> getData(T& val)
		{
			using namespace HDF5_Wrapper;

			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			const auto spacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			HDF5_DataspaceOptions spaceopts;

			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

			HDF5_DatasetOptions datasetopts{};
			HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));

			HDF5_DatatypeWrapper type(val(0,0), datatypeopts);
			assert(dataset.getDatatype() == type);

			const auto& dataspace{ dataset.getDataspace() };
			const auto dims = dataspace.getDimensions();

			//TODO: add code for dynamic sized matrix!
			//TODO: Check dimension of Vector!

			assert(dims.size() == 2);

			const std::size_t cols{ dims.at(1) }, rows{ dims.at(0) };

			//Eigen::Matrix<typename T::Scalar, Eigen::Dynamic, Eigen::Dynamic> Storage(cols, rows);

			HDF5_DataspaceOptions memoryspaceopt;
			memoryspaceopt.dims = std::vector<std::size_t>{ { static_cast<std::size_t>(rows), static_cast<std::size_t>(cols) } };
			memoryspaceopt.maxdims = std::vector<std::size_t>{ { static_cast<std::size_t>(rows), static_cast<std::size_t>(cols) } };
			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val(0,0), datatypeopts), HDF5_DataspaceWrapper(spacetype, memoryspaceopt) };

			//TODO: add code for dynamic sized matrix!
			if constexpr (!(T::IsRowMajor) && !T::IsVectorAtCompileTime)
			{ //Converting from Columnmajor to rowmajor
				std::vector<typename T::Scalar> vec(cols*rows);
				//Eigen::Matrix<typename T::Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> TransposedMatrix(dims.at);
				//HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), memorytypeopts), std::move(memoryspace) };
				dataset.readData(vec.data(), memoryopts);
				//Eigen::Map< EigenMatrix, Eigen::Unaligned, Eigen::Stride<1, EigenMatrix::ColsAtCompileTime> >
				//val = Eigen::Map<std::decay_t<T>, Eigen::Unaligned>(vec.data(),rows,cols);
				//val = Eigen::Map<std::decay_t<T>, Eigen::Unaligned, Eigen::Stride<1, std::decay_t<T>::ColsAtCompileTime>>(vec.data(), rows, cols);
				val = Eigen::Map<std::decay_t<T>, Eigen::Unaligned, Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>>(vec.data(), rows, cols, Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>(1,cols));
				//Map<MatrixXi, 0, OuterStride<> >(array, 3, 3, OuterStride<>(4)) <_ for dynamic size?
			}
			else
			{
				//HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), memorytypeopts), std::move(memoryspace) };
				dataset.readData(val, memoryopts);
			}
		}
		
		template<typename T>
		std::enable_if_t<stdext::is_eigen_tensor_v<std::decay_t<T>>> getData(T& val)
		{
			using Type = std::decay_t<T>; // T cannot be const
			using DataType = typename T::Scalar;
			using Index = typename T::Index;
			using Dimensions = typename T::Dimensions;
			using ArrayBase = typename T::Dimensions::Base;

			using namespace HDF5_Wrapper;

			const HDF5_LocationWrapper& currentLoc = mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();

			const auto spacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			HDF5_DataspaceOptions spaceopts;

			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

			HDF5_DatasetOptions datasetopts{};
			HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));

			HDF5_DatatypeWrapper type(*val.data(), datatypeopts);
			assert(dataset.getDatatype() == type);

			const auto& dataspace{ dataset.getDataspace() };
			const auto dims = dataspace.getDimensions();

			//TODO: add code for dynamic sized matrix!
			//TODO: Check dimension of Tensor!

			assert(dims.size() >= 1);

			ArrayBase dimarray = static_cast<T>(val).dimensions();
			const std::size_t elements = std::accumulate(dims.begin(), dims.end(), 0);
			const std::size_t size = std::accumulate(dims.begin(), dims.end(), 0, std::multiplies<std::size_t>());
			//Eigen::Matrix<typename T::Scalar, Eigen::Dynamic, Eigen::Dynamic> Storage(cols, rows);
			if (dimarray[0] == 0) // If the Tensor Type has unintialized dimensions we will use the dimension given by MATLAB to Map to the tensor!
			{
				//Number of Elments agree
				const auto ndims = dims.size();
				if (val.NumDimensions == ndims)
				{
					for (std::size_t index{ 0 }; index < ndims; ++index)
					{
						dimarray[index] = (std::int64_t)(dims[index]); //TODO: Check for Overflow. Narrowing conversion from uint64_t
					}
				}
				else if (size % val.NumDimensions == 0)
					// && value.NumDimensions != ndims; Number of Dimensions does not agree but number of elements can be equally maped to the Tensor
				{
					const std::int64_t elemperdim = size / val.NumDimensions;
					for (auto& elem : dimarray)
					{
						elem = elemperdim;
					}
				}
				else // Cannot map the Data! -> User Error -> throw Exception
				{
					throw std::runtime_error{ "Cannot partition MATLAB data into tensor. Number of elements wrong. (Maybe add padding to data?)" };
				}
				static_cast<T&>(val).resize(dimarray);
			}
			else
			{
				const std::size_t countTensor = std::accumulate(dimarray.begin(), dimarray.end(), 0ull, std::multiplies<std::size_t>());
				if (countTensor != elements) //To many or to few elements; will either cut or leave values empty
				{ //Should this really throw? Even in the case Tensor < Matlab? 
					throw std::runtime_error{ std::string{ "Element count between the provided Tensor and MATLAB disagree!" } };
				}
			}

			HDF5_DataspaceOptions memoryspaceopt;
			memoryspaceopt.dims.resize(dims.size());
			memoryspaceopt.maxdims.resize(dims.size());
			std::reverse_copy(dims.begin(), dims.end(), memoryspaceopt.dims.begin());
			std::reverse_copy(dims.begin(), dims.end(), memoryspaceopt.maxdims.begin());
			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), HDF5_DataspaceWrapper(spacetype, memoryspaceopt) };

			std::vector<typename T::Scalar> readstorage(size);
			dataset.readData(readstorage.data(), memoryopts);

			static_cast<T&>(val) = Eigen::TensorMap<Type, Eigen::Unaligned>(readstorage, dimarray);
		}

	};

}

#endif	// INC_HDF5_Archive_H

///---------------------------------------------------------------------------------------------------
