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
		template<class Class, typename Args>
		using write_to_HDF5_t = decltype(std::declval<Class>().write(std::declval<HDF5_Wrapper::HDF5_DatasetWrapper&>(), std::declval<std::decay_t<Args>>()));

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
		HDF5_Wrapper::HDF5_DatatypeOptions			 DefaultDatatypeOptions{};
		HDF5_Wrapper::HDF5_DataspaceOptions			 DefaultDataspaceOptions{};
	};

	class HDF5_OutputArchive : public OutputArchive<HDF5_OutputArchive>
	{
		using ThisClass = HDF5_OutputArchive;
	public:
		HDF5_OutputArchive(const std::experimental::filesystem::path &path, const HDF5_OutputOptions& options) : OutputArchive(this), mFile(openOrCreateFile(path,options)), mOptions(options)
		{
			//std::cout << mFile.getLocation().getHDF5Path() << std::endl;
			//std::cout << mFile.getLocation().getHDF5Fullpath() << std::endl;
		}

		DISALLOW_COPY_AND_ASSIGN(HDF5_OutputArchive)

		template<typename T>
		inline void save(const Archives::NamedValue<T>& value)
		{
			setNextPath(value.getName());		//Sets the name for the next Dataset or Group
			this->operator()(value.getValue()); //Write Data to the Group or Dataset
			clearNextPath();					//Remove the Last Fieldname
		};

		template<typename T>
		inline std::enable_if_t< HDF5_traits::has_write_to_HDF5_v<T> > save(const T& value)
		{
			auto dataset = createOrOpenDataset(value);
			write(dataset,value);
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
		HDF5_Wrapper::HDF5_DatasetWrapper createOrOpenDataset(const T& val)
		{
			// TODO: Maybe remove this function and but it all into write.

			//Open or create the Dataset here!
			using namespace HDF5_Wrapper;


			const auto& currentLoc = mGroupStack.empty() ?  mFile.getLocation() : mGroupStack.top().getLocation() ;

			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

			if constexpr(std::is_arithmetic_v<std::decay_t<T>> || stdext::is_complex_v<std::decay_t<T>>)
			{
				const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
				
				//TODO: Check if dataset already exists! 
								
				//Creating the dataspace!
				const HDF5_DataspaceOptions dataspaceopts;
								
				//Creating the dataset!
				HDF5_DatasetOptions datasetopts(HDF5_DatatypeWrapper(val, datatypeopts) , HDF5_DataspaceWrapper(dataspacetype,dataspaceopts));
				HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));
				return dataset;
			}
			else if constexpr(stdext::is_string_v<std::decay_t<T>>)
			{
				const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();

				//TODO: Check if dataset already exists! 


				//Creating the dataspace!
				const HDF5_DataspaceOptions dataspaceopts;

				//Creating the dataset!
				HDF5_DatasetOptions datasetopts(HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts));
				HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));
				return dataset;
			}
			else if constexpr(stdext::is_arithmetic_container_v<std::decay_t<T>>)
			{
				const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();

				//TODO: Check if dataset already exists! 

				if constexpr (stdext::is_memory_sequentiel_container_v<std::decay_t<T>>)
				{				//Creating the dataspace!
					HDF5_DataspaceOptions dataspaceopts;
					dataspaceopts.dims = std::vector<std::size_t>{ {val.size()} };
					dataspaceopts.maxdims = std::vector<std::size_t>{ { val.size() } };

					//Creating the dataset!
					HDF5_DatasetOptions datasetopts(HDF5_DatatypeWrapper(*val.begin(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts));
					HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));
					return dataset;
				}
				else
				{
					static_assert(!stdext::is_memory_sequentiel_container_v<std::decay_t<T>>, "Case not implemented yet");
				}
			}
			else if constexpr(stdext::is_eigen_type_v<std::decay_t<T>>)
			{
				const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
				HDF5_DataspaceOptions dataspaceopts;
				//dataspaceopts.dims = std::vector<std::size_t>{ { static_cast<std::size_t>(val.cols()), static_cast<std::size_t>(val.rows()) } };
				//dataspaceopts.maxdims = std::vector<std::size_t>{ { static_cast<std::size_t>(val.cols()), static_cast<std::size_t>(val.rows()) } };
				dataspaceopts.dims = std::vector<std::size_t>{ { static_cast<std::size_t>(val.rows()), static_cast<std::size_t>(val.cols()) } };
				dataspaceopts.maxdims = std::vector<std::size_t>{ { static_cast<std::size_t>(val.rows()), static_cast<std::size_t>(val.cols()) } };
				HDF5_DataspaceWrapper dataspace(dataspacetype, dataspaceopts);
				HDF5_DatasetOptions datasetopts(HDF5_DatatypeWrapper(val(0,0), datatypeopts), std::move(dataspace));
				HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));
				return dataset;
			}
			else if constexpr(stdext::is_eigen_tensor_v<std::decay_t<T>>)
			{
				const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
				HDF5_DataspaceOptions dataspaceopts;
				const auto valdims = val.dimensions();
				dataspaceopts.dims.resize(valdims.size() );
				dataspaceopts.maxdims.resize(valdims.size() );
				std::copy(valdims.begin(), valdims.end(), dataspaceopts.dims.begin());
				std::copy(valdims.begin(), valdims.end(), dataspaceopts.maxdims.begin());
				std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.dims.begin());
				std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.maxdims.begin());
				HDF5_DataspaceWrapper dataspace(dataspacetype, dataspaceopts);
				HDF5_DatasetOptions datasetopts(HDF5_DatatypeWrapper(*val.data(), datatypeopts), std::move(dataspace));
				HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));
				return dataset;
			}
			else 
			{
				static_assert(false);
		
				assert(false);
				//TODO: Check if dataset already exists! 
				//Creating the dataspace!
				const HDF5_DataspaceOptions dataspaceopts;

				//Creating the dataset!
				HDF5_DatasetOptions datasetopts(HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts));
				HDF5_DatasetWrapper dataset(currentLoc, nextPath, std::move(datasetopts));
				return dataset;
			}
		}

		//template<typename T>
		//void closeDataset(const T& val, HDF5_Wrapper::HDF5_DatasetWrapper &)
		//{
			//Donne automatically when the Dataset moves out of scope !
		//}

		template <typename T>
		std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>> || stdext::is_complex_v<std::decay_t<T>> > write(HDF5_Wrapper::HDF5_DatasetWrapper& dataset, const T& val)
		{
			using namespace HDF5_Wrapper;
			const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
			const HDF5_DataspaceOptions dataspaceopts;

			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
			dataset.writeData(val, memoryopts);
		}

		template <typename T>
		std::enable_if_t<stdext::is_string_v<std::decay_t<T>>> write(HDF5_Wrapper::HDF5_DatasetWrapper& dataset, const T& val)
		{
			using namespace HDF5_Wrapper;
			const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
			HDF5_DataspaceOptions dataspaceopts;
			HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
			dataset.writeData(val.c_str(), memoryopts);
		}

		template <typename T>
		std::enable_if_t<stdext::is_arithmetic_container_v<std::decay_t<T>>> write(HDF5_Wrapper::HDF5_DatasetWrapper& dataset, const T& val)
		{
			if constexpr (stdext::is_memory_sequentiel_container_v<std::decay_t<T>>)
			{
				using namespace HDF5_Wrapper;
				const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
				const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

				HDF5_DataspaceOptions dataspaceopts;
				dataspaceopts.dims = std::vector<std::size_t>{ { val.size() } };
				dataspaceopts.maxdims = std::vector<std::size_t>{ { val.size() } };
				
				HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.begin(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
				dataset.writeData(val.data(), memoryopts);
			}
			else
			{
				static_assert(!stdext::is_memory_sequentiel_container_v<std::decay_t<T>>, "Case not implemented yet");
			}
		}

		template <typename T>
		std::enable_if_t<stdext::is_eigen_type_v<std::decay_t<T>>> write(HDF5_Wrapper::HDF5_DatasetWrapper& dataset, const T& val)
		{
			using namespace HDF5_Wrapper;
			const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

			const auto rows = val.rows();
			const auto cols = val.cols();

			HDF5_DataspaceOptions dataspaceopts;
			dataspaceopts.dims = std::vector<std::size_t>{ { static_cast<std::size_t>(rows), static_cast<std::size_t>(cols)} };
			dataspaceopts.maxdims = std::vector<std::size_t>{ { static_cast<std::size_t>(rows), static_cast<std::size_t>(cols)  } };

			HDF5_DataspaceWrapper dataspace(dataspacetype, dataspaceopts);

			if constexpr (!(T::IsRowMajor) && !T::IsVectorAtCompileTime)
			{ //Converting from Columnmajor to rowmajor
				Eigen::Matrix<typename T::Scalar, T::RowsAtCompileTime, T::ColsAtCompileTime, Eigen::RowMajor> TransposedMatrix = val;
				HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), std::move(dataspace) };
				dataset.writeData(TransposedMatrix, memoryopts);		
			}
			else
			{
				HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), std::move(dataspace) };
				dataset.writeData(val, memoryopts);
			}
		}

		template <typename T>
		std::enable_if_t<stdext::is_eigen_tensor_v<std::decay_t<T>>> write(HDF5_Wrapper::HDF5_DatasetWrapper& dataset, const T& val)
		{
			using namespace HDF5_Wrapper;
			const auto dataspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
			const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
			
			HDF5_DataspaceOptions dataspaceopts;

			const auto valdims = val.dimensions();
			dataspaceopts.dims.resize(valdims.size());
			dataspaceopts.maxdims.resize(valdims.size());
			std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.dims.begin());
			std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.maxdims.begin());
			//std::copy(valdims.begin(), valdims.end(), dataspaceopts.dims.begin());
			//std::copy(valdims.begin(), valdims.end(), dataspaceopts.maxdims.begin());
			HDF5_DataspaceWrapper dataspace(dataspacetype, dataspaceopts);


			//if constexpr (!(T::Layout))
			//{ //Converting from Columnmajor to rowmajor
			//	Eigen::Tensor<typename T::Scalar, T::NumDimensions, Eigen::RowMajor> TransposedTensor = val;
			//	HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), std::move(dataspace) };
			//	dataset.writeData(TransposedTensor, memoryopts);
			//}
			//else
			//{
				HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), std::move(dataspace) };
				dataset.writeData(val.data(), memoryopts);
			//}

		};
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
