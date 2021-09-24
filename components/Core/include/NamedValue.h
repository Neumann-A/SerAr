///---------------------------------------------------------------------------------------------------
// file:		NamedValue.h
//
// summary: 	Declares the named value class
//
// Copyright (c) 2016 Alexander Neumann.
//
// author: Alexander
// date: 12.09.2016
#pragma once
#ifndef INC_NamedValue_H
#define INC_NamedValue_H
///---------------------------------------------------------------------------------------------------
#include <type_traits>
#include <utility>
#include <string>

#include <MyCEL/basics/BasicMacros.h>

namespace Archives
{
    ///-------------------------------------------------------------------------------------------------
    /// <summary>	NamedValue holds information of a value, its name and type
    /// 			
    /// 			Compiler should be able to completly remove this class at compile time if used
    /// 			with constexpr data. (Not the common case but could be usefull)
    ///			   </summary>
    ///
    /// <typeparam name="T"> Type of Value stored </typeparam>
    ///-------------------------------------------------------------------------------------------------	
    template <typename T>
    class NamedValue
    {
    public:
        using type = T;
        //If T is an Array: keep the type
        //If T is a l value: store a reference (T&)
        //If T is a r value: copy the value (T) (if used as intended this is an unlikly case but sometimes happens)
        using internal_type = typename std::conditional_t<std::is_array_v<typename std::remove_reference_t<T>>,
                                                typename std::remove_cv<T>::type,
                                                typename std::conditional_t<std::is_lvalue_reference_v<T>,
                                                                        T&,	typename std::decay_t<T>>>;
    public:
        const std::string name;
        internal_type val;

        //Disallow assignment of NamedValue; 
        //could run in problems with r value typed NamedValue
        NamedValue & operator=(const NamedValue &) = delete;
        
        //Needed for testing;
        //NamedValue(NamedValue const &) = delete;
    public:
        ///-------------------------------------------------------------------------------------------------
        /// <summary>	Constructor for named value. </summary>
        ///
        /// <param name="name"> 	The name of the value. </param>
        /// <param name="value">	[in,out] The value. </param>
        ///-------------------------------------------------------------------------------------------------
        BASIC_ALWAYS_INLINE explicit NamedValue(std::string valname, T&& value) : name(std::move(valname)), val(std::forward<T>(value)) {}

        ///-------------------------------------------------------------------------------------------------
        /// <summary>	Gets the value. </summary>
        ///
        /// <returns>	The Value </returns>
        ///-------------------------------------------------------------------------------------------------
        BASIC_ALWAYS_INLINE internal_type getValue() const noexcept { return val; }

        ///-------------------------------------------------------------------------------------------------
        /// <summary>	Gets the name. </summary>
        ///
        /// <returns>	The name of the value. </returns>
        ///-------------------------------------------------------------------------------------------------
        BASIC_ALWAYS_INLINE const std::string& getName() const noexcept { return name; }
    };


    template<typename T>
    NamedValue(std::string valname, T&& value) -> NamedValue<std::remove_cvref_t<T>>;
    template<typename T>
    NamedValue(std::string valname, T& value) -> NamedValue<std::remove_cvref_t<T>&>;

    template<typename T>
    inline NamedValue<T> createNamedValue(std::string name, T&& value)
    {
        return NamedValue<T>{std::move(name), std::forward<T>(value)};
    }
    template <typename T>
    inline NamedValue<T> createNamedValue(T&& value)
    {
        return NamedValue<T>{getTypeNameDescription(value), std::forward<T>(value)};
    }


    template<typename T, typename _ = std::void_t<> >
    struct is_NamedValue : std::false_type {};

    template<typename T>
    struct is_NamedValue<T, std::void_t<typename std::decay_t<T>::type,
        typename std::decay_t<T>::internal_type>> : std::is_same<typename std::decay_t<T>,
        Archives::NamedValue<typename std::decay_t<T>::type>> {};

    template<typename T>
    static constexpr bool is_NamedValue_v = is_NamedValue<T>::value;

    template<typename T, typename _ = std::void_t<> >
    struct is_nested_NamedValue : std::false_type {};

    template<typename T>
    struct is_nested_NamedValue<T, std::void_t<typename std::decay_t<T>::type,
        typename std::decay_t<T>::type::type,
        typename std::decay_t<T>::internal_type>> : std::is_same<typename std::decay_t<T>::type,
        Archives::NamedValue<typename std::decay_t<T>::type::type>> {};

    template<typename T>
    static constexpr bool is_nested_NamedValue_v = is_nested_NamedValue<T>::value;

    template<typename T>
    concept IsNamedValue = std::same_as<std::remove_cvref_t<T>, NamedValue<typename std::remove_cvref_t<T>::type>>;
}

#define ARCHIVE_CREATE_NV(val) Archives::createNamedValue(#val,val)

#endif	// INC_NamedValue_H
// end of NamedValue.h
///---------------------------------------------------------------------------------------------------
