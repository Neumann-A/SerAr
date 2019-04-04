///---------------------------------------------------------------------------------------------------
// file:		C:\Sources\Repos\Everything\Archive\Archive\NamedValueWithDescription.h
//
// summary: 	Declares the named value with description class
//
// Copyright (c) 2019 Alexander Neumann.
//
// author: Neumann
// date: 03.04.2019
#pragma once
#ifndef INC_NamedValueWithDescription_H
#define INC_NamedValueWithDescription_H
///---------------------------------------------------------------------------------------------------
#include <type_traits>
#include <utility>
#include <string_view>

#include "basics/BasicMacros.h"

#include "NamedValue.h"

namespace Archives
{
    ///-------------------------------------------------------------------------------------------------
    /// <summary>	NamedValueWithDesc holds information of a value, its name, a description and the type
    /// 			
    /// 			Compiler should be able to completly remove this class at compile time if used
    /// 			with constexpr data. (Not the common case but could be usefull)
    ///			   </summary>
    ///
    /// <typeparam name="T"> Type of Value stored </typeparam>
    ///-------------------------------------------------------------------------------------------------	
    template <typename T>
    class NamedValueWithDesc
    {
    public:
        using type = T;
        //If T is an Array: keep the type
        //If T is a l value: store a reference (T&)
        //If T is a r value: copy the value (T) (if used as intended this is an unlikly case but sometimes happens)
        using internal_type = typename std::conditional<std::is_array<typename std::remove_reference<T>::type>::value,
            typename std::remove_cv<T>::type,
            typename std::conditional<std::is_lvalue_reference<T>::value,
            T&, typename std::decay<T>::type>::type>::type;
        using is_nested = is_nested_NamedValue<T>;
        std::string_view name;
        std::string_view desc;
        internal_type val;
     
        //Disallow assignment; 
        NamedValueWithDesc& operator=(const NamedValueWithDesc&) = delete;

        //Needed for testing;
        //NamedValue(NamedValue const &) = delete;

        ///-------------------------------------------------------------------------------------------------
        /// <summary>	Constructor for named value. </summary>
        ///
        /// <param name="name"> 	The name of the value. </param>
        /// <param name="value">	[in,out] The value. </param>
        ///-------------------------------------------------------------------------------------------------
        //BASIC_ALWAYS_INLINE explicit NamedValueWithDesc(std::string_view valname, T&& value, std::string_view valdesc ="")
        //    : name(std::move(valname)), desc(std::move(valdesc)), val(std::forward<T>(value)) {}
        BASIC_ALWAYS_INLINE explicit NamedValueWithDesc(std::string_view valname, std::string_view valdesc, T&& value)
           : name(valname), desc(valdesc), val(std::forward<T>(value)) {}
		BASIC_ALWAYS_INLINE explicit NamedValueWithDesc(T&& value, std::string_view valname, std::string_view valdesc)
			: name(valname), desc(valdesc), val(std::forward<T>(value)) {}
        template<std::size_t N, std::size_t M>
        BASIC_ALWAYS_INLINE explicit NamedValueWithDesc(const char (&valname)[N], const char (&valdesc)[M], T&& value)
            : name(valname), desc(valdesc), val(std::forward<T>(value)) {}
		template<std::size_t N, std::size_t M>
		BASIC_ALWAYS_INLINE explicit NamedValueWithDesc(T&& value, const char(&valname)[N], const char(&valdesc)[M])
			: name(valname), desc(valdesc), val(std::forward<T>(value)) {}
    };
    template<std::size_t N, std::size_t M, typename T>
    NamedValueWithDesc(const char(&valname)[N], const char(&valdesc)[M], T&& value) -> NamedValueWithDesc<T>;

	template<std::size_t N, std::size_t M, typename T>
	NamedValueWithDesc(T&& value, const char(&valname)[N], const char(&valdesc)[M]) -> NamedValueWithDesc<T>;

    ///-------------------------------------------------------------------------------------------------
    /// <summary>	Gets type name. </summary>
    ///
    /// <returns>	The type name. </returns>
    ///-------------------------------------------------------------------------------------------------
    template<typename T>
    auto getNamedValueTypeName(const NamedValue<T>&)
    {
        return typeid(T).name();
    }
    template<typename T>
    auto getNamedValueTypeName(const NamedValueWithDesc<T>&)
    {
        return typeid(T).name();
    }
       
    template<typename T>
    inline NamedValue<T> createNamedValue(std::string_view name, T&& value)
    {
        return NamedValue<T>{name, std::forward<T>(value)};
    }

};

//#define ARCHIVE_CREATE_NV(val) Archives::createNamedValue(#val,val)


#endif	// INC_NamedValueWithDescription_H
// end of C:\Sources\Repos\Everything\Archive\Archive\NamedValueWithDescription.h
///---------------------------------------------------------------------------------------------------
