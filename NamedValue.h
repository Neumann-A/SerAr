///---------------------------------------------------------------------------------------------------
// file:		NamedValue.h
//
// summary: 	Declares the named value class
//
// Copyright (c) 2016 Alexander Neumann.
//
// author: Alexander
// date: 12.09.2016

#ifndef INC_NamedValue_H
#define INC_NamedValue_H
///---------------------------------------------------------------------------------------------------
#pragma once

#include <type_traits>
#include <string>

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
		//If T is an Array: keep the type
		//If T is a l value: store a reference (T&)
		//If T is a r value: copy the value (T) (if used as intended this is an unlikly case but sometimes happens)
		using Type = typename std::conditional<std::is_array<typename std::remove_reference<T>::type>::value,
												typename std::remove_cv<T>::type,
												typename std::conditional<std::is_lvalue_reference<T>::value,
																		T,	typename std::decay<T>::type>::type>::type;
	private:
		const char * const valname;
		const T val;

		//Disallow assignment of NamedValue; 
		//could run in problems with r value typed NamedValue
		NamedValue & operator=(const NamedValue &) = delete;
		
		//Needed for testing;
		//NamedValue(NamedValue const &) = delete;
	public:

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets type name. </summary>
		///
		/// <returns>	The type name. </returns>
		///-------------------------------------------------------------------------------------------------
		inline static const char & getTypeName() noexcept { return typeid(Type).name(); };

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor for named value. </summary>
		///
		/// <param name="name"> 	The name of the value. </param>
		/// <param name="value">	[in,out] The value. </param>
		///-------------------------------------------------------------------------------------------------
		inline constexpr explicit NamedValue(const char * name, T&& value) : valname(name), val(std::forward<T>(value))	{};

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the value. </summary>
		///
		/// <returns>	The Value </returns>
		///-------------------------------------------------------------------------------------------------
		inline constexpr Type getValue() const noexcept { return val; };

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the name. </summary>
		///
		/// <returns>	The name of the value. </returns>
		///-------------------------------------------------------------------------------------------------
		inline constexpr const char * getName() const noexcept { return valname; };

	};

	///-------------------------------------------------------------------------------------------------
	/// <summary>	Creates named value. </summary>
	///
	/// <typeparam name="T">	Generic type parameter. </typeparam>
	/// <param name="name"> 	The name of the value. </param>
	/// <param name="value">	[in,out] The value. </param>
	///
	/// <returns>	The new named value. </returns>
	///-------------------------------------------------------------------------------------------------
	template<typename T>
	inline constexpr NamedValue<T> createNamedValue(const char * const name, T&& value) noexcept
	{
		return NamedValue<T>{name, std::forward<T>(value)};
	}

	///-------------------------------------------------------------------------------------------------
	/// <summary>	Creates named value. </summary>
	///
	/// <typeparam name="T">	Generic type parameter. </typeparam>
	/// <param name="name"> 	The name of the value. </param>
	/// <param name="value">	[in,out] The value. </param>
	///
	/// <returns>	The new named value. </returns>
	///-------------------------------------------------------------------------------------------------
	template<typename T>
	inline NamedValue<T> createNamedValue(const std::string& name, T&& value)
	{
		return NamedValue<T>{name.c_str(), std::forward<T>(value)};
	}

};

#define BASIC_CREATE_NV(val) Basic::createNamedValue(#val,val)

#endif	// INC_NamedValue_H
// end of NamedValue.h
///---------------------------------------------------------------------------------------------------
