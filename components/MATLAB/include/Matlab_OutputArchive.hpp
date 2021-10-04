#pragma once

#include "Matlab_Common.hpp"

#include <SerAr/Core/OutputArchive.h>

namespace SerAr {
    using namespace Archives;
    ///-------------------------------------------------------------------------------------------------
    /// <summary>	A matlab output archive. </summary>
    ///
    /// <remarks>	Neumann, 17.02.2017. </remarks>
    ///-------------------------------------------------------------------------------------------------
    class Matlab_OutputArchive : public OutputArchive<Matlab_OutputArchive>
    {
        //TODO: Make the Code for the Matlab_OutputArchive cleaner. The Code for the MatlabInputArchive seems much cleaner than this one!

    public:
        using Options = MatlabOptions;

        Matlab_OutputArchive(const std::filesystem::path &fpath, const MatlabOptions &options = MatlabOptions::update);
        ~Matlab_OutputArchive();

        ALLOW_DEFAULT_MOVE_AND_ASSIGN(Matlab_OutputArchive)
        DISALLOW_COPY_AND_ASSIGN(Matlab_OutputArchive)
        template<typename T>
        inline void save(const NamedValue<T>& value)
        {
            setNextFieldname(value.name);   //Set the Name of the next Field

            if constexpr (!HasCreateMATLAB<Matlab_OutputArchive, std::remove_cvref_t<T>>)
                startMATLABArray(value);	    //Start a new Matlab Array or Struct

            this->operator()(value.val);    //Write Data to the Field/struct

            if constexpr (!HasCreateMATLAB<Matlab_OutputArchive, std::remove_cvref_t<T>>)
                finishMATLABArray();	        //Finish the Array (write it to the Array above)

            clearNextFieldname();           //Remove the last Fieldname
        }
        template<typename T>
        requires(!HasCreateMATLAB<Matlab_OutputArchive, std::remove_cvref_t<T>>)
        inline void save(const std::vector<T>& values)
        {
            for (auto& elem : values) {
                this->operator()(elem);
            }
        }
        
        template<typename T>
        inline void save(const std::optional<T>& opt) {
            if(opt)
                this->operator()(*opt);
        }

        inline void save(const std::filesystem::path& path) {
            this->operator()(path.string());
        }

        template<typename T> requires (HasCreateMATLAB<Matlab_OutputArchive, std::remove_cvref_t<T>>)
        inline void save(const T& value)
        {
            using Type = std::remove_reference_t<T>;
            //TODO: Build Fieldname if none has been set. Important! Otherwise matlab will throw an runtime exception!
            if (getCurrentFieldname().empty()) { //We need to create a fieldname 
                setNextFieldname(typeid(T).name()); //TODO: Sanitize Fieldnames(remove spaces, points etc)! Currently wont work correctly without a (allowed) fieldname
            }
            static_assert(!std::is_same< MATLAB::MATLABClassFinder<Type>, MATLAB::MATLAB_UnknownClass>::value,"T is not known within MATLAB. Will be unable to create mxArrray*!");
            auto& arrdata = createMATLABArray(value);
            Fields.push(std::make_tuple(getCurrentFieldname(), &arrdata));
            finishMATLABArray();
        }

    //private:
    private:
        std::filesystem::path m_filepath;
        //MatlabOptions m_options;
        std::unique_ptr<MATFile, void(*)(MATFile*)> m_MatlabFile;

        using Field = std::tuple<std::string, mxArray*>;	
        std::stack<Field> Fields;	//Using a stack to hold all Fields; Since the implementation is recursive in save().
        std::string nextFieldname;

        std::unique_ptr<MATFile, void(*)(MATFile*)> getMatlabFile(const std::filesystem::path &fpath, const MatlabOptions &options = MatlabOptions::update) const;

        inline void checkmxArrayPtr(mxArray* ptr ) const {
            if(ptr==nullptr)
                throw std::runtime_error{ fmt::format("Unable create new mxArray! (Out of memory?)") };
        }

        inline void setNextFieldname(const std::string &str) noexcept { nextFieldname = str; }
        inline void clearNextFieldname() noexcept { nextFieldname.clear(); }
        inline auto getCurrentFieldname() noexcept { return nextFieldname; }
        inline void checkNextFieldname() {
            //TODO: Create proper fieldname and sanitize fieldnames (points, spaces are not allowed!)
            if (getCurrentFieldname().empty()) {
                throw std::runtime_error{ "No Fieldname defined! (Invalid behavior right now!)" };
            }
        }

        //Starting a new field
        template<typename T> requires (!HasCreateMATLAB<Matlab_OutputArchive, std::remove_cvref_t<T>>)
        inline void startMATLABArray(const T& val) {
            mxArray *pStruct = nullptr;
            
            if (!Fields.empty()) {
                pStruct = mxGetField(std::get<1>(Fields.top()), 0, getCurrentFieldname().c_str());
            }
            
            if (pStruct == nullptr) {
                constexpr mwSize ndims = 2;
                constexpr mwSize dims[ndims]{ 1,1 }; // Higher dimensional structs are a bit strange... looks like an array of structs (not what we normally want. What we want -> [1,1])
                pStruct = mxCreateStructArray(ndims, dims, 0, nullptr); //MatException here. 
                checkmxArrayPtr(pStruct);
            }

            if (mxGetClassID(pStruct) != mxSTRUCT_CLASS) {
                throw std::runtime_error{ "Updating a mxArray which is not a struct! (Unwanted behavior!)" };
            }

            Fields.push(std::make_tuple(getCurrentFieldname(), pStruct));
            clearNextFieldname();
        }

        void finishMATLABArray();
    
        //Save all other arithmetics types
        template<typename T> 
        requires(std::is_arithmetic_v<std::remove_cvref_t<T>>)
        mxArray& createMATLABArray(const T& value) const {
            mxArray *valarray = mxCreateNumericMatrix(1, 1, MATLAB::MATLABClassFinder<std::remove_cvref_t<T>>::value, mxREAL);
            checkmxArrayPtr(valarray);
            *static_cast<T*>(mxGetData(valarray)) = value;
            return *valarray;
        }

        //Saving Strings
        template<typename T>
        requires(stdext::is_string_v<T>)
        mxArray& createMATLABArray(const T& value) const {
            mxArray *valarray = mxCreateString(value.c_str());
            checkmxArrayPtr(valarray);
            return *valarray;
        }

        //Save for container types with arithmetic payload type
        template<typename T>
        requires(stdext::is_arithmetic_container_v<T>)
        mxArray& createMATLABArray(const T& value) const {
            using DataType = std::remove_cvref_t<typename T::value_type>;
            mxArray *valarray = mxCreateNumericMatrix(value.size(),1, MATLAB::MATLABClassFinder<DataType>::value, mxREAL);
            if (valarray == nullptr)
                throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };
            //Cast needed since mxGetPr always returns an double pointer!
            DataType * dataposition = static_cast<DataType*>(mxGetData(valarray));
            assert(dataposition != nullptr);
            for (const auto& tmp : value) {
                *dataposition++ = tmp;
            }
            return *valarray;
        }

        template<typename T>
        requires(stdext::is_container_of_strings_v<T>)
        mxArray& createMATLABArray(const T& values) const {
            std::vector<const char*> cstr;
            std::transform(values.begin(),values.end(),cstr.begin(),[](auto& str) {return str.c_str();});
            //using DataType = std::remove_cvref_t<typename T::value_type>;
            mxArray *valarray = mxCreateCharMatrixFromStrings(cstr.size(),cstr.data());
            checkmxArrayPtr(valarray);
            return *valarray;
        }

#if defined(EIGEN_CORE_H)|| defined(EIGEN_CORE_MODULE_H)
        //Eigen Types 
        template <stdext::IsEigen3Type T>
        mxArray& createMATLABArray(const std::vector<T>& value) const
        {
            using EigenMatrix = std::remove_cvref_t<T>;
            using DataType = typename EigenMatrix::Scalar;
            const auto& first = value.begin();
            const mwSize rows = static_cast<mwSize>(first->rows());
            const mwSize cols = static_cast<mwSize>(first->cols());

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
            checkmxArrayPtr(valarray);

            DataType * dataposition = static_cast<DataType*>(mxGetData(valarray));
        
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

        template <stdext::IsEigen3Type T>
        mxArray& createMATLABArray(const T& value) const
        {
            using DataType = typename T::Scalar;

            //Be Aware: Matlab stores the matrix in column-major order, Eigen in row major!
            mxArray *valarray = mxCreateNumericMatrix(static_cast<std::size_t>(value.rows()), static_cast<std::size_t>(value.cols()), MATLAB::MATLABClassFinder<DataType>::value, mxREAL);
            checkmxArrayPtr(valarray);

            DataType * dataposition = static_cast<DataType*>(mxGetData(valarray));
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

// #ifdef EIGEN_CXX11_TENSOR_TENSOR_H
//         template<typename T>
//         requires(!stdext::is_string_v<T> && stdext::is_eigen_tensor_v<std::remove_cvref_t<T>>)
//         mxArray& createMATLABArray(const T& value) const
//         {
//             using Type = std::remove_cvref_t<T>; // T cannot be const
//             using DataType = typename T::Scalar;
//             using Index = typename T::Index;
//             using Dimensions = typename T::Dimensions;
//             using ArrayBase = typename T::Dimensions::Base;

//             const auto ndims = value.NumDimensions;
//             const auto dims = static_cast<T>(value).dimensions();

//             mxArray *valarray = mxCreateNumericArray(ndims, (const std::size_t*)(dims.data()), MATLAB::MATLABClassFinder<DataType>::value, mxREAL);
//             if (valarray == nullptr)
//                 throw std::runtime_error{ "Unable create new mxArray! (Out of memory?)" };

//             DataType * dataposition = reinterpret_cast<DataType*>(mxGetData(valarray));
//             assert(dataposition != nullptr);

//             /* Reading Data from Array */
//             Eigen::TensorMap<Type>(dataposition, dims) = static_cast<T>(value);
//             return *valarray;
            
//         }
// #endif
#endif
        //TODO: tuple version
    };


    #define MATLAB_ARCHIVE_SAVE(type) \
        extern template void Matlab_OutputArchive::save< type & >(const NamedValue< type &> &); \
        extern template void Matlab_OutputArchive::save< const type & >(const NamedValue< const type &> &); \
        extern template void Matlab_OutputArchive::save< type >(const NamedValue< type > &); \
        extern template void Matlab_OutputArchive::save< const type >(const NamedValue< const type > &); 
    MATLAB_ARCHIVE_SAVE(bool)
    MATLAB_ARCHIVE_SAVE(short)
    MATLAB_ARCHIVE_SAVE(unsigned short)
    MATLAB_ARCHIVE_SAVE(int)
    MATLAB_ARCHIVE_SAVE(unsigned int)
    //MATLAB_ARCHIVE_SAVE(long)
    //MATLAB_ARCHIVE_SAVE(unsigned long)
    MATLAB_ARCHIVE_SAVE(long long)
    MATLAB_ARCHIVE_SAVE(unsigned long long)
    MATLAB_ARCHIVE_SAVE(double)
    MATLAB_ARCHIVE_SAVE(float)
    MATLAB_ARCHIVE_SAVE(std::string)
    MATLAB_ARCHIVE_SAVE(std::filesystem::path)
    MATLAB_ARCHIVE_SAVE(std::vector<short>)
    MATLAB_ARCHIVE_SAVE(std::vector<unsigned short>)
    MATLAB_ARCHIVE_SAVE(std::vector<int>)
    MATLAB_ARCHIVE_SAVE(std::vector<unsigned int>)
    //MATLAB_ARCHIVE_SAVE(std::vector<long>)
    //MATLAB_ARCHIVE_SAVE(std::vector<unsigned long>)
    MATLAB_ARCHIVE_SAVE(std::vector<long long>)
    MATLAB_ARCHIVE_SAVE(std::vector<unsigned long long>)
    MATLAB_ARCHIVE_SAVE(std::vector<double>)
    MATLAB_ARCHIVE_SAVE(std::vector<float>)
    //MATLAB_ARCHIVE_SAVE(std::vector<std::string>)
    MATLAB_ARCHIVE_SAVE(std::vector<std::filesystem::path>)
    MATLAB_ARCHIVE_SAVE(std::optional<bool>)
    MATLAB_ARCHIVE_SAVE(std::optional<short>)
    MATLAB_ARCHIVE_SAVE(std::optional<unsigned short>)
    MATLAB_ARCHIVE_SAVE(std::optional<int>)
    MATLAB_ARCHIVE_SAVE(std::optional<unsigned int>)
    //MATLAB_ARCHIVE_SAVE(std::optional<long>)
    //MATLAB_ARCHIVE_SAVE(std::optional<unsigned long>)
    MATLAB_ARCHIVE_SAVE(std::optional<long long>)
    MATLAB_ARCHIVE_SAVE(std::optional<unsigned long long>)
    MATLAB_ARCHIVE_SAVE(std::optional<double>)
    MATLAB_ARCHIVE_SAVE(std::optional<float>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::string>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::filesystem::path>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<short>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<unsigned short>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<int>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<unsigned int>>)
    //MATLAB_ARCHIVE_SAVE(std::optional<std::vector<long>>)
    //MATLAB_ARCHIVE_SAVE(std::optional<std::vector<unsigned long>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<long long>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<unsigned long long>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<double>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<float>>)
    //MATLAB_ARCHIVE_SAVE(std::optional<std::vector<std::string>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<std::filesystem::path>>)
    #undef MATLAB_ARCHIVE_SAVE
}