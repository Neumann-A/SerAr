#pragma once
#include <filesystem>
#include <string>
#include <type_traits>

#include <MyCEL/basics/BasicMacros.h>

#include <SerAr/Core/NamedValue.h>
#include <SerAr/Core/NamedEnumVariant.hpp>
#include <SerAr/Core/InputArchive.h>
#include <SerAr/Core/OutputArchive.h>

#include "HDF5_ArchiveHelpers.hpp"
#include "HDF5_Wrappers.h"
namespace Archives
{

    /*****************************************************************************************/
    /****************** Input Archive									 *********************/
    /*****************************************************************************************/

    class HDF5_InputOptions : InputArchive_Options<HDF5_InputOptions, HDF5_InputArchive>
    {
    public:
        bool										 dontReorderData{ false };
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
    private:
        using CurrentGroup = HDF5_Wrapper::HDF5_GroupWrapper;
        //using LastDataset = HDF5_Wrapper::HDF5_DatasetWrapper;
        using File = HDF5_Wrapper::HDF5_FileWrapper;

        File                            mFile;
        std::stack<CurrentGroup>        mGroupStack{};
        std::stack<std::string>         mPathStack{};

        HDF5_InputOptions mOptions;

        static File openFile(const std::filesystem::path &path, const HDF5_InputOptions& options)
        {
            using namespace HDF5_Wrapper;
            HDF5_FileOptions opt{};
            opt.mode = options.FileCreationMode;

            File file{ path, opt };
            return file;
        }

        //TODO: Move those function into another class which can also be used by the Output Archive
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
        void openGroup(const T&, const std::string& path)
        {
            using namespace HDF5_Wrapper;
            HDF5_GroupOptions opts { HDF5_GeneralOptions{.mode=HDF5_GeneralOptions::HDF5_Mode::Open} };
            HDF5_GroupWrapper group{ getCurrentLocation(), path, opts };
            mGroupStack.push(std::move(group));
        }
        void closeLastGroup()
        {
            assert(!mGroupStack.empty());
            mGroupStack.pop(); // Just pop it from the stack. The Destructor will close it!
        };

        template<typename T>
        std::enable_if_t<std::is_arithmetic_v<std::remove_cvref_t<T>> ||
            stdext::is_complex_v<std::remove_cvref_t<T>> ||
            stdext::is_string_v<std::remove_cvref_t<T>> > getData(T& val)
        {
            using namespace HDF5_Wrapper;

            const auto spacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
            const auto datatypeopts{ mOptions.DefaultDatatypeOptions };
            HDF5_DatasetOptions datasetopts{};
            HDF5_DatasetWrapper dataset(getCurrentLocation(), getPath(), std::move(datasetopts));
            if constexpr(!stdext::is_string_v<std::remove_cvref_t<T>>)
            {
                assert(dataset.getDatatype() == HDF5_DatatypeWrapper(val, datatypeopts));
            }
            else
            {
                //assert(HDF5_DatatypeWrapper(H5Tget_super(dataset.getDatatype())) == HDF5_DatatypeWrapper(H5Tget_super(HDF5_DatatypeWrapper(val, datatypeopts))));
            }
            const auto& dataspace{ dataset.getDataspace() };
            assert(dataspace.getDimensions().size() <= 1);
            HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val, datatypeopts), HDF5_DataspaceWrapper(spacetype) };
            dataset.readData(val, memoryopts);
        }

        template<typename T>
        std::enable_if_t<stdext::is_arithmetic_container_v<std::remove_cvref_t<T>> &&
         !stdext::is_associative_container_v<std::remove_cvref_t<T>> > getData(T& val)
        {
            using namespace HDF5_Wrapper;

            if constexpr (stdext::is_memory_sequentiel_container_v<std::remove_cvref_t<T>>)
            {
                const auto spacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
                //HDF5_DataspaceOptions spaceopts;

                const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

                HDF5_DatasetOptions datasetopts{};
                HDF5_DatasetWrapper dataset(getCurrentLocation(), getPath(), std::move(datasetopts));

                HDF5_DatatypeWrapper type(val[0], datatypeopts);
                assert(dataset.getDatatype() == type);

                const auto& dataspace{ dataset.getDataspace() };
                const auto dims = dataspace.getDimensions();

                assert(dims.size() == 1);

                val.resize(dims.at(0));

                HDF5_DataspaceOptions memoryspaceopt;
                memoryspaceopt.dims[0] = val.size();
                memoryspaceopt.maxdims[0] = val.size();
                HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val[0], datatypeopts), HDF5_DataspaceWrapper(spacetype, memoryspaceopt) };
                dataset.readData(val.data(), memoryopts);
            }
            else if constexpr(!stdext::is_associative_container_v<std::remove_cvref_t<T>>)
            {
                const auto spacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
                //HDF5_DataspaceOptions spaceopts;

                const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

                HDF5_DatasetOptions datasetopts{};
                HDF5_DatasetWrapper dataset(getCurrentLocation(), getPath(), std::move(datasetopts));

                HDF5_DatatypeWrapper type(val[0], datatypeopts);
                assert(dataset.getDatatype() == type);

                const auto& dataspace{ dataset.getDataspace() };
                const auto dims = dataspace.getDimensions();

                assert(dims.size() == 1);

                val.resize(dims.at(0));

                HDF5_DataspaceWrapper stordataspace(dataspace);
                stordataspace.removeSelection();
                stordataspace.selectSlab(H5S_SELECT_SET, { 0 }, { 1 }, { 1 }, { 1 });

                HDF5_DataspaceOptions memoryspaceopt;
                memoryspaceopt.dims[0] = val.size();
                memoryspaceopt.maxdims[0] = val.size();
                HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val[0], datatypeopts), stordataspace };
                std::vector<std::int64_t> offset{ 0 };
                for (auto& elem : val)
                {
                    dataset.readData(elem, memoryopts, stordataspace);
                    ++(offset[0]);
                    stordataspace.setOffset(offset);
                }
            }
            else
            {
                static_assert(!stdext::is_memory_sequentiel_container_v<std::remove_cvref_t<T>>, "Case not implemented yet! Please implement it!");
                //How to do it: For sequentiell containers: Write elements one at a time in a for loop into the dataset. Needs selection of appropiate dataspaces!
                // For associative containers: Open 
            }
        }

        template<typename T>
        std::enable_if_t<stdext::is_container_of_strings_v<std::remove_cvref_t<T>> &&
            !stdext::is_associative_container_v<std::remove_cvref_t<T>> > getData(T& val)
        {
            using namespace HDF5_Wrapper;

            //const auto spacetype = DataspaceTypeSelector<std::decay_t<T>>::value();
            //HDF5_DataspaceOptions spaceopts;

            const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

            HDF5_DatasetOptions datasetopts{};
            HDF5_DatasetWrapper dataset(getCurrentLocation(), getPath(), std::move(datasetopts));

            HDF5_DatatypeWrapper type(val[0], datatypeopts);
            assert(dataset.getDatatype() == type);

            const auto& dataspace{ dataset.getDataspace() };
            const auto dims = dataspace.getDimensions();

            assert(dims.size() == 1);

            val.resize(dims.at(0));

            HDF5_DataspaceWrapper stordataspace(dataspace);
            stordataspace.removeSelection();
            stordataspace.selectSlab(H5S_SELECT_SET, { 0 }, { 1 }, { 1 }, { 1 });

            HDF5_DataspaceOptions memoryspaceopt;
            memoryspaceopt.dims[0] = val.size();
            memoryspaceopt.maxdims[0] = val.size();
            HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(val[0], datatypeopts), stordataspace };
            std::vector<std::int64_t> offset{ 0 };
            for (auto& str : val)
            {
                dataset.readData(str, memoryopts, stordataspace);
                ++(offset[0]);
                stordataspace.setOffset(offset);
            }
        
        }
#ifdef EIGEN_CORE_H
        template<typename T>
        std::enable_if_t<stdext::is_eigen_type_v<std::remove_cvref_t<T>>> getData(T& val)
        {
            using namespace HDF5_Wrapper;

            const auto spacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
            HDF5_DataspaceOptions spaceopts;

            const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

            HDF5_DatasetOptions datasetopts{};
            HDF5_DatasetWrapper dataset(getCurrentLocation(), getPath(), std::move(datasetopts));

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
            memoryspaceopt.dims[0] = static_cast<hsize_t>(rows);
            memoryspaceopt.dims[1] = static_cast<hsize_t>(cols);
            memoryspaceopt.maxdims[0] = static_cast<hsize_t>(rows);
            memoryspaceopt.maxdims[1] = static_cast<hsize_t>(cols);
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
                val = Eigen::Map<std::remove_cvref_t<T>, Eigen::Unaligned, Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>>(vec.data(), rows, cols, Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>(1,cols));
                //Map<MatrixXi, 0, OuterStride<> >(array, 3, 3, OuterStride<>(4)) <_ for dynamic size?
            }
            else
            {
                //HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), memorytypeopts), std::move(memoryspace) };
                dataset.readData(val, memoryopts);
            }
        }
#ifdef EIGEN_CXX11_TENSOR_TENSOR_H
        template<typename T>
        std::enable_if_t<stdext::is_eigen_tensor_v<std::remove_cvref_t<T>>> getData(T& val)
        {
            using Type = std::remove_cvref_t<T>; // T cannot be const
            using DataType = typename T::Scalar;
            using Index = typename T::Index;
            using Dimensions = typename T::Dimensions;
            using ArrayBase = typename T::Dimensions::Base;

            using namespace HDF5_Wrapper;

            const auto spacetype = DataspaceTypeSelector<std::remove_cvref_t<T>>::value();
            HDF5_DataspaceOptions spaceopts;

            const auto datatypeopts{ mOptions.DefaultDatatypeOptions };

            HDF5_DatasetOptions datasetopts{};
            HDF5_DatasetWrapper dataset(getCurrentLocation(), getPath(), std::move(datasetopts));

            HDF5_DatatypeWrapper type(*val.data(), datatypeopts);
            assert(dataset.getDatatype() == type);

            const auto& dataspace{ dataset.getDataspace() };
            const auto dims = dataspace.getDimensions();

            //TODO: add code for dynamic sized matrix!
            //TODO: Check dimension of Tensor!

            assert(dims.size() >= 1);

            ArrayBase dimarray = static_cast<T>(val).dimensions();
            const std::size_t elements = std::accumulate(dims.begin(), dims.end(), 0ull);
            const std::size_t size = std::accumulate(dims.begin(), dims.end(), 1ull, std::multiplies<std::size_t>());
            //Eigen::Matrix<typename T::Scalar, Eigen::Dynamic, Eigen::Dynamic> Storage(cols, rows);
            
            if (dimarray[0] == 0) // If the Tensor Type has unintialized dimensions we will use the dimension given by MATLAB to Map to the tensor!
            {
                //Number of Elments agree
                const auto ndims = dims.size();
                if (val.NumDimensions == ndims)
                {
                    for (std::size_t index{ 0 }; index < ndims; ++index)
                    {
                        dimarray[ndims-index-1] = (std::int64_t)(dims[index]); //TODO: Check for Overflow. Narrowing conversion from uint64_t
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
                const std::size_t countTensor = std::accumulate(dimarray.begin(), dimarray.end(), 1ull, std::multiplies<std::size_t>());
                if (countTensor != elements) //To many or to few elements; will either cut or leave values empty
                { //Should this really throw? Even in the case Tensor < Matlab? 
                    throw std::runtime_error{ std::string{ "Element count between the provided Tensor and MATLAB disagree!" } };
                }
            }

            HDF5_DataspaceOptions memoryspaceopt;
            memoryspaceopt.dims.fill(0);
            memoryspaceopt.maxdims.fill(0);
            std::reverse_copy(dims.begin(), dims.end(), memoryspaceopt.dims.begin());
            std::reverse_copy(dims.begin(), dims.end(), memoryspaceopt.maxdims.begin());
            HDF5_MemoryOptions memoryopts{ HDF5_DatatypeWrapper(*val.data(), datatypeopts), HDF5_DataspaceWrapper(spacetype, memoryspaceopt) };

            std::vector<typename T::Scalar> readstorage(size);
            dataset.readData(&readstorage[0], memoryopts);

            static_cast<T&>(val) = Eigen::TensorMap<Type, Eigen::Unaligned>(readstorage.data(), dimarray);
        }
#endif
#endif
    public:
        using Options = HDF5_InputOptions;

        HDF5_InputArchive(const std::filesystem::path &path, const HDF5_InputOptions& options = HDF5_InputOptions{})
            : InputArchive(this), mFile(openFile(path, options)), mOptions(options) {
            static_assert(std::is_same_v<ThisClass, std::remove_cvref_t<decltype(*this)>>);
        }

        DISALLOW_COPY_AND_ASSIGN(HDF5_InputArchive)

        auto list(const std::string& str) -> std::map<std::string,std::string>;
        auto list(const Archives::NamedValue<decltype(nullptr)>& value) -> std::map<std::string,std::string>;
        template<typename T>
        std::enable_if_t<std::is_same<T, typename Archives::NamedValue<T>::internal_type>::value, std::map<std::string,std::string>> list(const Archives::NamedValue<T>& value)
        {
            const bool empty_string = value.getName().empty();
            if (!empty_string) {
                appendPath(value.getName()); //Sets the name for the next Dataset or Group
                if constexpr (!HDF5_ArchiveWriteAble<T, ThisClass>)
                    openGroup(value, getPath());
            }
            const auto tmp {list(value.val)};
            if (!empty_string) {
                if constexpr (!HDF5_ArchiveWriteAble<T, ThisClass>)
                    closeLastGroup();
                removePath(); //Remove the Last Fieldname
            }
            return tmp;
        }

        template<typename T>
        inline void load(Archives::NamedValue<T>& value)
        {
            const bool empty_string = value.getName().empty();
            if (!empty_string) {
                appendPath(value.getName()); //Sets the name for the next Dataset or Group
                if constexpr (!HDF5_ArchiveWriteAble<T, ThisClass>)
                    openGroup(value, getPath());
            }
            this->operator()(value.getValue()); //Write Data to the Group or Dataset
            if (!empty_string) {
                if constexpr (!HDF5_ArchiveWriteAble<T, ThisClass>)
                    closeLastGroup();
                removePath(); //Remove the Last Fieldname
            }
        }

        template<typename T> requires HDF5_ArchiveReadAble<std::remove_cvref_t<T>,ThisClass>
        inline void load(T& value)
        {
            getData(value);
        }
    };

    #define HDF5_ARCHIVE_LOAD(type) extern template void HDF5_InputArchive::load< type &>(NamedValue< type &> &);
    HDF5_ARCHIVE_LOAD(bool)
    HDF5_ARCHIVE_LOAD(short)
    HDF5_ARCHIVE_LOAD(unsigned short)
    HDF5_ARCHIVE_LOAD(int)
    HDF5_ARCHIVE_LOAD(unsigned int)
    HDF5_ARCHIVE_LOAD(long)
    HDF5_ARCHIVE_LOAD(unsigned long)
    HDF5_ARCHIVE_LOAD(long long)
    HDF5_ARCHIVE_LOAD(unsigned long long)
    HDF5_ARCHIVE_LOAD(double)
    HDF5_ARCHIVE_LOAD(float)
    HDF5_ARCHIVE_LOAD(std::string)
    HDF5_ARCHIVE_LOAD(std::vector<short>)
    HDF5_ARCHIVE_LOAD(std::vector<unsigned short>)
    HDF5_ARCHIVE_LOAD(std::vector<int>)
    HDF5_ARCHIVE_LOAD(std::vector<unsigned int>)
    HDF5_ARCHIVE_LOAD(std::vector<long>)
    HDF5_ARCHIVE_LOAD(std::vector<unsigned long>)
    HDF5_ARCHIVE_LOAD(std::vector<long long>)
    HDF5_ARCHIVE_LOAD(std::vector<unsigned long long>)
    HDF5_ARCHIVE_LOAD(std::vector<double>)
    HDF5_ARCHIVE_LOAD(std::vector<float>)
    HDF5_ARCHIVE_LOAD(std::vector<std::string>)
    #undef HDF5_ARCHIVE_LOAD
}
