///---------------------------------------------------------------------------------------------------
// file:		HDF5_ArchiveHelpers.h
//
// summary: 	Declares the hdf 5 archive helpers
//
// Copyright (c) 2021 Alexander Neumann.
//
// author: Alexander
// date: 08.03.2017

#ifndef INC_HDF5_ArchiveHelpers_H
#define INC_HDF5_ArchiveHelpers_H
///---------------------------------------------------------------------------------------------------
#pragma once


#include <vector>

#include <SerAr/Core/ArchiveHelper.h>

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
///             //Member Function type for converting values to strings
    template <typename T, typename Ar>
    concept HDF5_ArchiveWriteAble = SerAr::IsOutputArchive<Ar>&& requires(const T& value, Ar& ar)
    {
        ar.write(value);
    };
    template <typename T, typename Ar>
    concept HDF5_ArchiveReadAble = SerAr::IsInputArchive<Ar>&& requires(T& value, Ar& ar)
    {
        ar.getData(value);
    };

    /*****************************************************************************************/
    /****************** HDF5 Archive Helper								 *********************/
    /*****************************************************************************************/

    class HDF5_ArchiveHelper
    {
    public:
        template<typename T>
        static std::vector<std::size_t> calculateDimensions(const T& val)
        {
            if constexpr(stdext::is_arithmetic_container_v<std::decay_t<T>>)
            {
                return std::vector<std::size_t>{ { val.size() } };
            }
            else if constexpr (stdext::is_eigen_type_v<std::decay_t<T>>)
            {
                return std::vector<std::size_t>{ { static_cast<std::size_t>(val.rows()), static_cast<std::size_t>(val.cols()) } };
            }
            else if constexpr (stdext::is_container_with_eigen_type_v< std::decay_t<T>>)
            {
                //TODO: check for a dynamic matrix
                const auto begin = val.begin();
                return std::vector<std::size_t>{ { val.size(), static_cast<std::size_t>(*begin.rows()), static_cast<std::size_t>(*begin.cols()) } };
            }
            else
            {
                static_assert("Can not calculate dimensions for given type! Implementation not defined!");
                return { 0 };
            }
        }

        // template<typename T>
        // static HDF5_Wrapper::HDF5_DataspaceWrapper getDataspaceSelection(const T& val)
        // {

        // }
    };
}

#endif
