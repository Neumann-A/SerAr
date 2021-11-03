#pragma once

#include "Matlab_Common.hpp"

#include <MyCEL/stdext/is_optional.hpp>

#include <SerAr/Core/InputArchive.h>

namespace SerAr {
    using namespace Archives;
    ///-------------------------------------------------------------------------------------------------
    /// <summary>	A matlab input archive. </summary>
    ///
    /// <remarks>	Neumann, 17.02.2017. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class Matlab_InputArchive : public InputArchive<Matlab_InputArchive>
    {
    public:
        using Options = MatlabOptions;

        Matlab_InputArchive(const std::filesystem::path &fpath, const MatlabOptions &options = MatlabOptions::read);
        ~Matlab_InputArchive();
        ALLOW_DEFAULT_MOVE_AND_ASSIGN(Matlab_InputArchive)
        DISALLOW_COPY_AND_ASSIGN(Matlab_InputArchive)

        template<typename T>
        inline void load(NamedValue<T>& value)
        {
            checkCurrentField();				//Need to check if the current field is a struct or not; If not we cannot nest further!
            const auto opt = loadNextField(value.name, stdext::IsOptional<std::remove_cvref_t<T>>); //Loads the next Field with given name; (Move Down)
            if constexpr (stdext::IsOptional<std::remove_cvref_t<T>>) {
                if(!opt) {
                    value.val=std::nullopt;
                    return;
                } else {
                    typename std::remove_cvref_t<T>::value_type tmp;
                    this->operator()(tmp);		//Load Data from the Field or struct.
                    value.val = std::move(tmp);
                }
            } else {
                this->operator()(value.val);		//Load Data from the Field or struct.
            }
            if(opt) { releaseField(); }						//Remove the last Fieldname (Move Up)
        }
        template<typename T>
        inline void load(NamedValueWithDesc<T>& value)
        {
            checkCurrentField();				//Need to check if the current field is a struct or not; If not we cannot nest further!
            loadNextField(value.name);			//Loads the next Field with given name; (Move Down)
            this->operator()(value.val);		//Load Data from the Field or struct.
            releaseField();						//Remove the last Fieldname (Move Up)
        }

        inline void load(std::filesystem::path& path) {
            std::string tmp;
            this->operator()(tmp);
            path = std::filesystem::path(tmp);
        }
        template<typename T> 
        requires (HasGetValueMATLAB<Matlab_InputArchive,std::remove_cvref_t<T>>)
        inline void load(T& value)
        {
            using Type = std::decay_t<T>; // T cannot be const 
            const auto fieldptr = std::get<1>(mFields.top());
            
            //Check Type
            assert(mxGetClassID(fieldptr) == MATLAB::MATLABClassFinder<Type>::value);
            getValue(value, fieldptr);
        }
        template<typename T>
        requires (std::is_arithmetic_v<std::remove_cvref_t<T>>)
        inline void getValue(T& loadtarget, mxArray const * const field)
        {
            assert(field != nullptr);
            using DataType = std::remove_cvref_t<T>;
            loadtarget = *reinterpret_cast<DataType*>(mxGetData(field));
        };

        template<typename T>
        requires(stdext::is_string_v<T>)
        inline void getValue(T& loadtarget, mxArray const * const field)
        {
            assert(field != nullptr);
            const auto length = mxGetN(field);
            auto res = mxArrayToString(field);
            loadtarget = std::string{ std::move(res), length+1 }; // Copy data into string. 
            mxFree(res); //mxArrayToString uses heap memory for the returned char array. 
        };

        auto list(const Archives::NamedValue<decltype(nullptr)>& value) -> std::map<std::string,std::string>;
        template<typename T>
        requires(std::is_same_v<T, typename Archives::NamedValue<T>::internal_type>)
        std::map<std::string,std::string> list(const Archives::NamedValue<T>& value)
        {
            checkCurrentField();				//Need to check if the current field is a struct or not; If not we cannot nest further!
            loadNextField(value.name);			//Loads the next Field with given name; (Move Down)
            const auto tmp {list(value.val)};
            releaseField();						//Remove the last Fieldname (Move Up)
            return tmp;
        }
        auto list(const std::string& str) -> std::map<std::string,std::string>;

        template<typename T>
        requires(stdext::is_arithmetic_container_v<std::remove_cvref_t<T>>)
        inline void load(T& value)
        {
            //TODO: Check if this will work for all containers containing arithmetic types!
            //using Type = std::remove_cvref_t<T>; // T cannot be const
            using DataType = typename std::remove_cvref_t<typename std::remove_cvref_t<T>::value_type>;
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

#if defined(EIGEN_CORE_H)|| defined(EIGEN_CORE_MODULE_H)

        template<typename T>
        inline std::enable_if_t<stdext::is_eigen_type_v<std::remove_cvref_t<T>>> load(T& value)
        {
            //using Type = std::remove_cvref_t<T>; // T cannot be const
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
        inline std::enable_if_t<stdext::is_container_with_eigen_type_v<std::remove_cvref_t<T>>> load(T& value)
        {
            using Type = std::remove_cvref_t<T>;
            using EigenType = std::remove_cvref_t<typename std::remove_cvref_t<T>::value_type>; // T cannot be const
            using DataType = typename EigenType::Scalar;
            
            const auto fieldptr = std::get<1>(mFields.top());
            assert(mxGetClassID(fieldptr) == MATLAB::MATLABClassFinder<DataType>::value);
            
            const auto ndims = mxGetNumberOfDimensions(fieldptr);
            const auto dims = mxGetDimensions(fieldptr);

            if(!(ndims == 2 || ndims == 3))
                throw std::runtime_error{"Cannot load data from MATLAB! Dimension mismatch"};

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
        inline std::enable_if_t<stdext::is_eigen_tensor_v<std::remove_cvref_t<T>>> load(T& value)
        {
            using Type = std::remove_cvref_t<T>; // T cannot be const
            using DataType = typename T::Scalar;
            using Index = typename T::Index;
            using Dimensions = typename T::Dimensions;
            using ArrayBase = typename T::Dimensions::Base;

            const auto fieldptr = std::get<1>(mFields.top());

            assert(mxGetClassID(fieldptr) == MATLAB::MATLABClassFinder<DataType>::value);

            ArrayBase dimarray = static_cast<T>(value).dimensions();
        
            const auto ndims = mxGetNumberOfDimensions(fieldptr);
            const auto dims = mxGetDimensions(fieldptr);

            std::size_t elemcountMATLAB{ 1 };
            for (std::size_t index{ 0 }; index < ndims; ++index )
            {
                elemcountMATLAB = elemcountMATLAB * dims[index]; //TODO: Check for Overflow!
            }

            if (dimarray[0] == 0) // If the Tensor Type has unintialized dimensions we will use the dimension given by MATLAB to Map to the tensor!
            {
                //Number of Elments agree
                if (value.NumDimensions == ndims)
                {
                    for (std::size_t index{ 0 }; index < ndims; ++index)
                    {
                        dimarray[index] = (std::int64_t)(dims[index]); //TODO: Check for Overflow. Narrowing conversion from uint64_t
                    }
                }
                else if (elemcountMATLAB % value.NumDimensions == 0)
                    // && value.NumDimensions != ndims; Number of Dimensions does not agree but number of elements can be equally maped to the Tensor
                {
                    const std::int64_t elemperdim = elemcountMATLAB / value.NumDimensions;
                    for (auto& elem : dimarray)
                    {
                        elem = elemperdim;
                    }
                }
                else // Cannot map the Data! -> User Error -> throw Exception
                {
                    throw std::runtime_error{"Cannot partition MATLAB data into tensor. Number of elements wrong. (Maybe add padding to data?)"};
                }
                static_cast<T&>(value).resize(dimarray);
            }
            else
            {
                const std::size_t countTensor = std::accumulate(dimarray.begin(), dimarray.end(), 1ull, std::multiplies<std::size_t>());
                if (countTensor != elemcountMATLAB) //To many or to few elements; will either cut or leave values empty
                { //Should this really throw? Even in the case Tensor < Matlab? 
                    throw std::runtime_error{ std::string{"Element count between the provided Tensor and MATLAB disagree!"} };
                }
            }
            DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(fieldptr));
            assert(dataposition != nullptr); //NULLPTR means no data -> throw instead since user error?

            /* Reading Data from Array */
            static_cast<T&>(value) = Eigen::TensorMap<Type, Eigen::Unaligned>(dataposition, dimarray);
        }
#endif
#endif
    private:
        std::unique_ptr<MATFile, void(*)(MATFile*)> m_MatlabFile;

        using Field = std::tuple<std::string, mxArray * const>;
        std::stack<Field> mFields;	//Using a stack to hold all Fields; Since the implementation is recursive in save().

        std::unique_ptr<MATFile, void(*)(MATFile*)> getMatlabFile(const std::filesystem::path &fpath, const MatlabOptions &options) const;

        void checkCurrentField();
        std::optional<mxArray*> loadNextField(const std::string &str, bool optional = false);
        void releaseField() noexcept;

        template<typename Container> 
        requires(stdext::is_container_v<std::remove_cvref_t<Container>>)
        void resizeContainer(Container& cont,const std::size_t& size)
        {
            static_assert(!stdext::is_associative_container_v<std::remove_cvref_t<Container>>, "Need to implement assosiative containers in MATLAB. Please do it ;)");
            if constexpr(std::is_same_v<std::remove_cvref_t<Container>, std::array<typename Container::value_type, stdext::array_size<Container>>>)
            {
                //std::array is the only fixed size container. So we use an assert here!
                assert(cont.size() == size);
            }
            else if constexpr(stdext::is_resizeable_container_v<std::remove_cvref_t<Container>>)
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
                    using DataType = std::remove_cvref_t<typename std::remove_cvref_t<Container>::value_type>;

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

#if defined(EIGEN_CORE_H)|| defined(EIGEN_CORE_MODULE_H)
        template<typename EigenType>
        std::enable_if_t<stdext::is_eigen_type_v<std::remove_cvref_t<EigenType>>> assignEigenType(EigenType& value, typename EigenType::Scalar * dataposition, const std::size_t& rows, const std::size_t& cols)
        {
            //TODO: Resize EigenType if necessary
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

    #define MATLAB_ARCHIVE_LOAD(type) \
        extern template void Matlab_InputArchive::load< type &>(NamedValue< type &> &); \
        extern template void Matlab_InputArchive::load< type >(NamedValue< type > &);
    MATLAB_ARCHIVE_LOAD(bool)
    MATLAB_ARCHIVE_LOAD(short)
    MATLAB_ARCHIVE_LOAD(unsigned short)
    MATLAB_ARCHIVE_LOAD(int)
    MATLAB_ARCHIVE_LOAD(unsigned int)
    //MATLAB_ARCHIVE_LOAD(long)
    //MATLAB_ARCHIVE_LOAD(unsigned long)
    MATLAB_ARCHIVE_LOAD(long long)
    MATLAB_ARCHIVE_LOAD(unsigned long long)
    MATLAB_ARCHIVE_LOAD(double)
    MATLAB_ARCHIVE_LOAD(float)
    MATLAB_ARCHIVE_LOAD(std::string)
    MATLAB_ARCHIVE_LOAD(std::filesystem::path)
    MATLAB_ARCHIVE_LOAD(std::vector<short>)
    MATLAB_ARCHIVE_LOAD(std::vector<unsigned short>)
    MATLAB_ARCHIVE_LOAD(std::vector<int>)
    MATLAB_ARCHIVE_LOAD(std::vector<unsigned int>)
    //MATLAB_ARCHIVE_LOAD(std::vector<long>)
    //MATLAB_ARCHIVE_LOAD(std::vector<unsigned long>)
    MATLAB_ARCHIVE_LOAD(std::vector<long long>)
    MATLAB_ARCHIVE_LOAD(std::vector<unsigned long long>)
    MATLAB_ARCHIVE_LOAD(std::vector<double>)
    MATLAB_ARCHIVE_LOAD(std::vector<float>)
    MATLAB_ARCHIVE_LOAD(std::vector<std::string>)
    //MATLAB_ARCHIVE_LOAD(std::vector<std::filesystem::path>)
    MATLAB_ARCHIVE_LOAD(std::optional<bool>)
    MATLAB_ARCHIVE_LOAD(std::optional<short>)
    MATLAB_ARCHIVE_LOAD(std::optional<unsigned short>)
    MATLAB_ARCHIVE_LOAD(std::optional<int>)
    MATLAB_ARCHIVE_LOAD(std::optional<unsigned int>)
    //MATLAB_ARCHIVE_LOAD(std::optional<long>)
    //MATLAB_ARCHIVE_LOAD(std::optional<unsigned long>)
    MATLAB_ARCHIVE_LOAD(std::optional<long long>)
    MATLAB_ARCHIVE_LOAD(std::optional<unsigned long long>)
    MATLAB_ARCHIVE_LOAD(std::optional<double>)
    MATLAB_ARCHIVE_LOAD(std::optional<float>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::string>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::filesystem::path>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<short>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<unsigned short>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<int>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<unsigned int>>)
    //MATLAB_ARCHIVE_LOAD(std::optional<std::vector<long>>)
    //MATLAB_ARCHIVE_LOAD(std::optional<std::vector<unsigned long>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<long long>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<unsigned long long>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<double>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<float>>)
    //MATLAB_ARCHIVE_LOAD(std::optional<std::vector<std::string>>)
    //MATLAB_ARCHIVE_LOAD(std::optional<std::vector<std::filesystem::path>>)
    #undef MATLAB_ARCHIVE_LOAD
}