#pragma once
#include <filesystem>
#include <string>
#include <type_traits>
#include <cassert>

#include <string_view>

#include <MyCEL/basics/BasicMacros.h>
#include <MyCEL/stdext/std_extensions.h>

#include <SerAr/Core/NamedValue.h>
#include <SerAr/Core/NamedEnumVariant.hpp>
#include <SerAr/Core/InputArchive.h>
#include <SerAr/Core/OutputArchive.h>

#include "HDF5_ArchiveHelpers.hpp"
#include "HDF5_Wrappers.h"
namespace Archives
{

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
        bool										 dontReorderData {false} ;
        HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode FileCreationMode{ HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode::CreateOrOverwrite };
        HDF5_Wrapper::HDF5_DatatypeOptions			 DefaultDatatypeOptions{};
        HDF5_Wrapper::HDF5_DataspaceOptions			 DefaultDataspaceOptions{};

        constexpr HDF5_OutputOptions() = default;
        constexpr HDF5_OutputOptions(const HDF5_Wrapper::HDF5_GeneralOptions::HDF5_Mode& mode) : FileCreationMode(mode) {}
    };

    ///-------------------------------------------------------------------------------------------------
    /// <summary>	HDF5 output archive. </summary>
    ///
    /// <seealso cref="T:OutputArchive{HDF5_OutputArchive}"/>
    ///-------------------------------------------------------------------------------------------------
    class HDF5_OutputArchive : public OutputArchive<HDF5_OutputArchive>
    {
        using ThisClass = HDF5_OutputArchive;

    private:
        
        using CurrentGroup = HDF5_Wrapper::HDF5_GroupWrapper;
        using File = HDF5_Wrapper::HDF5_FileWrapper;
        
        File mFile;
        std::stack<CurrentGroup> mGroupStack{};
        std::stack<std::string>  mPathStack{};
        HDF5_OutputOptions mOptions;
        static File openOrCreateFile(const std::filesystem::path &path, const HDF5_OutputOptions& options)
        {
            using namespace HDF5_Wrapper;
            HDF5_FileOptions opt{};
            opt.mode = options.FileCreationMode;
            File file{ path, opt };
            return file; 
        }

        bool isValidNextLocation(const std::string&) const
        {
            //TODO: Check the Location name for invalid characters
            return true;
        }
        void appendPath(const std::string& str)
        {
            if (!isValidNextLocation(str))
                throw std::runtime_error{ "Invalid HDF5 path string!" };
            assert(!str.empty());
            mPathStack.push(str);
        }
        const auto& getPath()
        {
            const auto& path = mPathStack.top();
            assert(!path.empty());
            return path;
        }
        void removePath()
        {
            assert(!mPathStack.empty());
            mPathStack.pop();
        }
        const auto& getCurrentLocation()
        {
            using namespace HDF5_Wrapper;
            return mGroupStack.empty() ? static_cast<const HDF5_LocationWrapper&>(mFile) : mGroupStack.top();
        }
        template<typename T>
        void createOrOpenGroup(const T&, const std::string& path)
        {
            using namespace HDF5_Wrapper;
            HDF5_GroupOptions opts { HDF5_GeneralOptions{.mode=HDF5_GeneralOptions::HDF5_Mode::OpenOrCreate} };
            mGroupStack.emplace(getCurrentLocation(), path, opts);
        }

        template<typename T>
        void closeLastGroup(const T&)
        {
            assert(!mGroupStack.empty());
            mGroupStack.pop();
        }
        
        public: // For some reason the write functions must be public for clang to detect that the class can use them.
        template <typename T>
        requires(std::is_arithmetic_v<std::remove_cvref_t<T>> || stdext::is_complex_v<std::remove_cvref_t<T>>)
        void write(const T& val)
        {
            using namespace HDF5_Wrapper;
            //Creating the dataset! 
            const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
            const auto dataspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
            const HDF5_DataspaceOptions dataspaceopts;
            HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
            HDF5_DatasetOptions datasetopts;
            HDF5_DatasetWrapper dataset(getCurrentLocation(), getPath(), std::move(storeopts), std::move(datasetopts));
            //Creating the Memory space
            const auto memoryspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
            const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };
            const HDF5_DataspaceOptions memoryspaceopt;
            HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val, memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };
            //Write the Data
            dataset.writeData(val, memoryopts);
        }

        template <typename T>
        requires(stdext::is_string_v<std::remove_cvref_t<T>>)
        void write(const T& val)
        {
            using namespace HDF5_Wrapper;
            //Creating the dataset! 
            const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
            const auto dataspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
            const HDF5_DataspaceOptions dataspaceopts;
            HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
            HDF5_DatasetOptions datasetopts;
            HDF5_DatasetWrapper dataset(getCurrentLocation(), getPath(), std::move(storeopts), std::move(datasetopts));
            //Creating the Memory space
            const auto memoryspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
            const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };
            const HDF5_DataspaceOptions memoryspaceopt;
            HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val, memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };
            //Write the Data
            dataset.writeData(val.c_str(), memoryopts); //for variable string type
            //dataset.writeData(val, memoryopts); // For fixed size string type
        }
        
        template <typename T>
        requires(stdext::is_arithmetic_container_v<std::remove_cvref_t<T>>)
        void write(const T& val)
        {
            using namespace HDF5_Wrapper;

            const auto& currentLoc {getCurrentLocation()};
            if constexpr (stdext::is_memory_sequentiel_container_v<std::remove_cvref_t<T>>)
            {
                //Creating the dataset! 
                const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
                const auto dataspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
                
                //Settings storage dimensions
                HDF5_DataspaceOptions dataspaceopts;
                dataspaceopts.dims[0] = val.size();
                dataspaceopts.maxdims = dataspaceopts.dims;

                HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(*val.begin(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
                HDF5_DatasetOptions datasetopts;
                HDF5_DatasetWrapper dataset(currentLoc, getPath(), std::move(storeopts), std::move(datasetopts));

                //Creating the storage dataspace selection (default is enough -> All space)

                //Creating the memory space
                const auto memoryspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
                const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };

                //Settings memory dimensions
                HDF5_DataspaceOptions memoryspaceopt;
                memoryspaceopt.dims[0] = val.size();
                memoryspaceopt.maxdims = memoryspaceopt.dims;

                HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val[0], memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };

                dataset.writeData(val, memoryopts);
            }
            else if constexpr (!stdext::is_associative_container_v<std::remove_cvref_t<T>>)
            {
                //Creating the dataset! 
                const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
                const auto dataspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();

                //Settings storage dimensions
                HDF5_DataspaceOptions dataspaceopts;
                dataspaceopts.dims[0] = val.size();
                dataspaceopts.maxdims = dataspaceopts.dims;

                HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(*val.begin(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
                HDF5_DatasetOptions datasetopts;
                HDF5_DatasetWrapper dataset(currentLoc, getPath(), std::move(storeopts), std::move(datasetopts));
            
                //Creating the storage dataspace selection
                HDF5_DataspaceWrapper stordataspace(dataspacetype, dataspaceopts);
                stordataspace.removeSelection();
                stordataspace.selectSlab(H5S_SELECT_SET, { 0 }, { 1 }, { 1 }, { 1 });

                //Creating the memory space
                const auto memoryspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
                const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };
                HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val[0], memorytypeopts), stordataspace };

                std::vector<std::int64_t> offset{ 0 };
                for (const auto& str : val)
                {
                    dataset.writeData(str, memoryopts, stordataspace);
                    ++(offset[0]);
                    stordataspace.setOffset(offset);
                }
            }
            else
            {
                static_assert(!stdext::is_memory_sequentiel_container_v<std::remove_cvref_t<T>>,
                              "Case not implemented yet! Please implement it!");
                //How to do it: For sequentiell containers: Write elements one at a time in a for loop into the dataset. Needs selection of appropiate dataspaces!
                // For associative containers: Open 
            }
        }

        template <typename T>
        requires( stdext::is_container_of_strings_v<std::remove_cvref_t<T>> &&
                 !stdext::is_associative_container_v<std::remove_cvref_t<T>>)
        void write(const T& val)
        {
                using namespace HDF5_Wrapper;

                //Creating the dataset! 
                const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
                const auto dataspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();

                //Settings storage dimensions
                HDF5_DataspaceOptions dataspaceopts;
                dataspaceopts.dims[0] = val.size();
                dataspaceopts.maxdims[0] = val.size();

                HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(*val.begin(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
                HDF5_DatasetOptions datasetopts;
                HDF5_DatasetWrapper dataset(getCurrentLocation(), getPath(), std::move(storeopts), std::move(datasetopts));
                
                //Creating the storage dataspace selection
                HDF5_DataspaceWrapper stordataspace(dataspacetype, dataspaceopts);
                stordataspace.removeSelection();
                stordataspace.selectSlab(H5S_SELECT_SET, { 0 }, { 1 }, { 1 }, { 1 });

                //Creating the memory space
                //const auto memoryspacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
                const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };
                HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val[0], memorytypeopts), stordataspace };

                std::vector<std::int64_t> offset{ 0 };
                for (const auto& str : val)
                {
                    dataset.writeData(str.c_str(), memoryopts, stordataspace);
                    ++(offset[0]);
                    stordataspace.setOffset(offset);
                }	
        }
#ifdef EIGEN_CORE_H
        template <stdext::IsEigen3Type T>
        void write(const T& val)
        {
            using namespace HDF5_Wrapper;
            
            const auto& currentLoc {getCurrentLocation()};
            //Creating the dataset! 
            const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
            const auto dataspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();

            //Settings storage dimensions
            HDF5_DataspaceOptions dataspaceopts;
            std::array<hsize_t,2> dims { static_cast<hsize_t>(val.rows()), static_cast<hsize_t>(val.cols()) };
            std::copy_n(dims.begin(),dims.size(),dataspaceopts.dims.begin()); 
            dataspaceopts.maxdims = dataspaceopts.dims;

            HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
            HDF5_DatasetOptions datasetopts;
            HDF5_DatasetWrapper dataset(currentLoc, getPath(), std::move(storeopts), std::move(datasetopts));
            
            //Creating the memory space
            const auto memoryspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
            const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };

            //Settings memory dimensions
            HDF5_DataspaceOptions memoryspaceopt;
            std::array<hsize_t,2> memdims { static_cast<hsize_t>(val.rows()), static_cast<hsize_t>(val.cols()) };
            std::copy_n(memdims.begin(),memdims.size(),memoryspaceopt.dims.begin()); 
            memoryspaceopt.maxdims = memoryspaceopt.dims;

            HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };

            if (mOptions.dontReorderData)
            {
                if constexpr (!(T::IsRowMajor) && !T::IsVectorAtCompileTime)
                { //Converting from Columnmajor to rowmajor
                    Eigen::Matrix<typename T::Scalar, T::RowsAtCompileTime, T::ColsAtCompileTime, Eigen::RowMajor> TransposedMatrix = val;
                    dataset.writeData(TransposedMatrix, memoryopts);
                }
                else
                {
                    dataset.writeData(val, memoryopts);
                }
            }
            else
            {
                dataset.writeData(val, memoryopts);
            }
        }

        template <stdext::IsEigen3Type T, template <typename> class Container>
        void write(const Container<T>& val)
        {
            using namespace HDF5_Wrapper;
            const auto& currentLoc {getCurrentLocation()};

            using EigenType = std::remove_cvref_t<typename std::remove_cvref_t<T>::value_type>;
            using Scalar = typename EigenType::Scalar;

            //Creating the dataset! 
            const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
            const auto dataspacetype = DataspaceTypeSelector<EigenType>::value();

            //Settings storage dimensions
            HDF5_DataspaceOptions dataspaceopts;
            std::array<hsize_t,3> dims { val.size(),static_cast<hsize_t>(val[0].rows()), static_cast<hsize_t>(val[0].cols()) };
            std::copy_n(dims.begin(),dims.size(),dataspaceopts.dims.begin()); 
            dataspaceopts.maxdims = dataspaceopts.dims;

            HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(Scalar{}, datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
            HDF5_DatasetOptions datasetopts;
            HDF5_DatasetWrapper dataset(currentLoc, getPath(), std::move(storeopts), std::move(datasetopts));
            
            //Creating the memory space
            const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };
            const auto memoryspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();

            //Settings memory dimensions
            HDF5_DataspaceOptions memoryspaceopt;
            std::array<hsize_t,3> memdims { val.size(),static_cast<hsize_t>(val[0].rows()), static_cast<hsize_t>(val[0].cols()) };
            std::copy_n(memdims.begin(),memdims.size(),memoryspaceopt.dims.begin()); 
            memoryspaceopt.maxdims = memoryspaceopt.dims;
            HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(Scalar{}, memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };

            //HDF5_DataspaceWrapper memoryspace(memoryspacetype, memoryspaceopt);

            if constexpr (!(EigenType::IsRowMajor) && !EigenType::IsVectorAtCompileTime)
            { //Converting from Columnmajor to rowmajor
                Eigen::Matrix<typename EigenType::Scalar, EigenType::RowsAtCompileTime, EigenType::ColsAtCompileTime, Eigen::RowMajor> TransposedMatrix = val;
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
        std::enable_if_t<stdext::is_eigen_tensor_v<std::remove_cvref_t<T>>> write(const T& val)
        {
            using namespace HDF5_Wrapper;

            using namespace HDF5_Wrapper;

            const auto& currentLoc {getCurrentLocation()};

            //Creating the dataset! 
            const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
            const auto dataspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();

            //Settings storage dimensions
            HDF5_DataspaceOptions dataspaceopts;
            const auto valdims = val.dimensions();
            dataspaceopts.dims.fill(0);
            dataspaceopts.maxdims.fill(0);
            std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.dims.begin());
            std::reverse_copy(valdims.begin(), valdims.end(), dataspaceopts.maxdims.begin());

            HDF5_StorageOptions storeopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), HDF5_DataspaceWrapper(dataspacetype, dataspaceopts) };
            HDF5_DatasetOptions datasetopts;
            HDF5_DatasetWrapper dataset(currentLoc, getPath(), std::move(storeopts), std::move(datasetopts));

            //Creating the memory space
            const auto memoryspacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
            const auto memorytypeopts{ mOptions.DefaultDatatypeOptions };

            //Settings memory dimensions
            HDF5_DataspaceOptions memoryspaceopt;
            memoryspaceopt.dims.fill(0);
            memoryspaceopt.maxdims.fill(0);
            std::reverse_copy(valdims.begin(), valdims.end(), memoryspaceopt.dims.begin());
            std::reverse_copy(valdims.begin(), valdims.end(), memoryspaceopt.maxdims.begin());
            HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), memorytypeopts), HDF5_DataspaceWrapper(memoryspacetype, memoryspaceopt) };
            dataset.writeData(*val.data(), memoryopts);
        }
#endif
    public:
        using Options = HDF5_OutputOptions;
        HDF5_OutputArchive(const std::filesystem::path &path, const HDF5_OutputOptions& options = HDF5_OutputOptions{})
            : OutputArchive(this), mFile(openOrCreateFile(path, options)), mOptions(options) {
            static_assert(std::is_same_v<ThisClass, std::remove_cvref_t<decltype(*this)>>);
        };

        DISALLOW_COPY_AND_ASSIGN(HDF5_OutputArchive)

        template<typename T>
        inline void save(const Archives::NamedValue<T>& value)
        {
            const bool empty_string = value.getName().empty();
            if (!empty_string) {
                appendPath(value.getName()); //Sets the name for the next Dataset or Group
                if constexpr (!HDF5_ArchiveWriteAble<T, ThisClass>)
                    createOrOpenGroup(value, getPath());
            }
            this->operator()(value.getValue()); //Write Data to the Group or Dataset
            if (!empty_string) {
                if constexpr (!HDF5_ArchiveWriteAble<T, ThisClass>)
                    closeLastGroup(value);
                removePath(); //Remove the Last Fieldname
            }
        }

        template<stdext::IsContainer T>
        requires (!HDF5_ArchiveWriteAble<T, ThisClass>)
        inline void save(const T& values )
        {
            for(auto& elem: values) {
                this->operator()(elem);
            }
        }

        template <typename T> requires HDF5_ArchiveWriteAble<T, ThisClass>
        inline void save(const T& value)
        {
            write(value);
        }
    };
    
    #define HDF5_ARCHIVE_SAVE(type) \
        extern template void HDF5_OutputArchive::save< type & >(const NamedValue< type &> &); \
        extern template void HDF5_OutputArchive::save< const type & >(const NamedValue< const type &> &); \
        extern template void HDF5_OutputArchive::save< type >(const NamedValue< type > &); \
        extern template void HDF5_OutputArchive::save< const type >(const NamedValue< const type > &); 
    HDF5_ARCHIVE_SAVE(bool)
    HDF5_ARCHIVE_SAVE(short)
    HDF5_ARCHIVE_SAVE(unsigned short)
    HDF5_ARCHIVE_SAVE(int)
    HDF5_ARCHIVE_SAVE(unsigned int)
    HDF5_ARCHIVE_SAVE(long)
    HDF5_ARCHIVE_SAVE(unsigned long)
    HDF5_ARCHIVE_SAVE(long long)
    HDF5_ARCHIVE_SAVE(unsigned long long)
    HDF5_ARCHIVE_SAVE(double)
    HDF5_ARCHIVE_SAVE(float)
    HDF5_ARCHIVE_SAVE(std::string)
    HDF5_ARCHIVE_SAVE(std::vector<short>)
    HDF5_ARCHIVE_SAVE(std::vector<unsigned short>)
    HDF5_ARCHIVE_SAVE(std::vector<int>)
    HDF5_ARCHIVE_SAVE(std::vector<unsigned int>)
    HDF5_ARCHIVE_SAVE(std::vector<long>)
    HDF5_ARCHIVE_SAVE(std::vector<unsigned long>)
    HDF5_ARCHIVE_SAVE(std::vector<long long>)
    HDF5_ARCHIVE_SAVE(std::vector<unsigned long long>)
    HDF5_ARCHIVE_SAVE(std::vector<double>)
    HDF5_ARCHIVE_SAVE(std::vector<float>)
    HDF5_ARCHIVE_SAVE(std::vector<std::string>)
    #undef HDF5_ARCHIVE_SAVE
}
