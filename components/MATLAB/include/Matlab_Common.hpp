#pragma once

#include <cassert>
#include <cstdint>
#include <concepts>

#include <filesystem>
#include <type_traits>
#include <utility>
#include <map>
#include <iosfwd>
#include <string>
#include <regex>
#include <complex>
#include <exception>
#include <numeric>

#include <fmt/format.h>

#include <MyCEL/stdext/std_extensions.h>
#include <MyCEL/basics/BasicMacros.h>
#include <MyCEL/basics/BasicIncludes.h>

#include <SerAr/Core/NamedValue.h>
#include <SerAr/Core/NamedValueWithDesc.h>
#include <SerAr/Core/NamedEnumVariant.hpp>

#ifdef __STDC__
#define SERAR__STDC__GUARD
#else
#define __STDC__
#endif
#include <cstdint>
#ifndef UINT64_T
#define UINT64_T std::uint64_t
#endif
#include <mat.h>
#ifndef SERAR__STDC__GUARD
#undef __STDC__
#endif


namespace SerAr
{
    using namespace Archives;
    template<typename Ar, typename T>
    concept HasCreateMATLAB = requires (const T& value, Ar& ar) {
        ar.createMATLABArray(value);
    };
    template<typename Ar, typename T>
    concept HasGetValueMATLAB = requires (T& value, Ar& ar) {
        ar.getValue(value,std::declval<mxArray const * const>());
    };

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
            static_assert(std::is_same_v<T,void>,"MATLABClassFinder: Unknown type!");
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
#if !defined(_MSC_VER)
        template<>
        struct MATLABClassFinder<long long int> : MATLAB_Int64Class {};
        template<>
        struct MATLABClassFinder<long long unsigned int> : MATLAB_UInt64Class {};
#endif
#if defined(EIGEN_CORE_H)|| defined(EIGEN_CORE_MODULE_H)
        template<typename T>
        struct MATLABClassFinder<Eigen::EigenBase<T>> : MATLABClassFinder<typename T::Scalar> {};
#ifdef EIGEN_CXX11_TENSOR_TENSOR_H
        template<typename T>
        struct MATLABClassFinder<Eigen::TensorBase<T>> : MATLABClassFinder<typename T::Scalar> {};
#endif
#endif
    }
    //Enum to represent the different Matlab file modes!
    //read: Opens file for reading only; determines the current version of the MAT-file by inspecting the files and preserves the current version.
    //update: Opens file for update, both reading and writing. If the file does not exist, does not create a file (equivalent to the r+ mode of fopen). Determines the current version of the MAT-file by inspecting the files and preserves the current version.
    //write: Opens file for writing only; deletes previous contents, if any.
    //write_v4: Creates a MAT-file compatible with MATLAB� Versions 4 software and earlier.
    //write_v6: Creates a MAT-file compatible with MATLAB Version 5 (R8) software or earlier. Equivalent to wL mode.
    //write_local : Opens file for writing character data using the default character set for your system. Use MATLAB Version 6 or 6.5 software to read the resulting MAT-file.
    //              If you do not use the wL mode switch, MATLAB writes character data to the MAT-file using Unicode� character encoding by default.
    //              Equivalent to w6 mode.
    //write_v7: Creates a MAT-file compatible with MATLAB Version 7.0 (R14) software or earlier. Equivalent to wz mode.
    //write_compressed: Opens file for writing compressed data. By default, the MATLAB save function compresses workspace variables as they are saved to a MAT-file. To use the same compression ratio when creating a MAT-file with the matOpen function, use the wz option.
    //                  Equivalent to w7 mode.
    //write_v73: Creates a MAT-file in an MATLAB-based format that can store objects that occupy more than 2 GB.
    enum class MatlabOptions {read, update, write, write_v4, write_v6, write_local, write_v7, write_compressed, write_v73};
    
    class MatlabHelper
    {
    public:
        static const char* getMatlabMode(const MatlabOptions& option) noexcept;
    };
}