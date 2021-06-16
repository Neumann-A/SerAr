///---------------------------------------------------------------------------------------------------
// file:		LoadConstructor.h
//
// summary: 	Declares the load constructor class
//
// Copyright (c) 2016 Alexander Neumann.
//
// author: Alexander Neumann
// date: 18.10.2016

#ifndef INC_LoadConstructor_H
#define INC_LoadConstructor_H
///---------------------------------------------------------------------------------------------------
#pragma once

#include <SerAr/Core/InputArchive.h>
#include <SerAr/Core/NamedValue.h>

namespace Archives
{
    ///-------------------------------------------------------------------------------------------------
    /// <summary>	A load constructor interface for archives.
    /// 			Common helper class to construct not default constructable objects
    /// 			Use a specialization of this class for not default constructable objects. </summary>
    ///
    /// <typeparam name="ToConstruct">	Type to construct. </typeparam>
    ///-------------------------------------------------------------------------------------------------
    template<typename ToConstruct>
    class LoadConstructor
    {
    public:
        using type = ToConstruct;

        static_assert(std::is_constructible<type, void>::value, "Type is not constructable without arguments. Archives need specialization of LoadConstructor ");

        template <typename Archive>
        static inline type construct(InputArchive<Archive>& ar)
        {
            type ConstructedType{};
            ar(ConstructedType);
            return ConstructedType;
        }

        template <typename Archive>
        static inline type constructWithName(InputArchive<Archive>& ar, char const * const name)
        {
            type ConstructedType{};
            ar(createNamedValue(name, ConstructedType));
            return ConstructedType;
        }

        template <typename Archive>
        static inline type constructWithName(InputArchive<Archive>& ar, const std::string& name)
        {
            type ConstructedType{};
            ar(createNamedValue(name, ConstructedType));
            return ConstructedType;
        }

        template <typename Archive>
        static inline type constructWithName(const std::string& name, InputArchive<Archive>& ar)
        {
            return constructWithName(ar, name);
        }

        template <typename Archive>
        static inline type constructWithName(char const * const name, InputArchive<Archive>& ar)
        {
            return constructWithName(ar, name);
        }

    };
}


#endif	// INC_LoadConstructor_H
// end of LoadConstructor.h
///---------------------------------------------------------------------------------------------------
