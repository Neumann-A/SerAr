///---------------------------------------------------------------------------------------------------
// file:		InputArchive.h
//
// summary: 	Declares the input archive class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander Neumann
// date: 07.01.2017

#ifndef INC_InputArchive_H
#define INC_InputArchive_H
///---------------------------------------------------------------------------------------------------
#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4814) // constexpr is not implicit const Warning
#endif

#include "basics/BasicMacros.h"

#include "ArchiveHelper.h"

namespace Archives
{

	template <typename OptionClass, typename Archive>
	class InputArchive_Options
	{
	public:
		using ArchiveType = Archive;
		using ArchiveOptions = OptionClass;

		inline ArchiveOptions& self() noexcept
		{
			return *static_cast<ArchiveOptions * const>(this);
		}
	};

	class ISerializeable; //Forward declaration

	class IInputArchive
	{
	protected:
		constexpr IInputArchive() = default;
		~IInputArchive() = default;
		DISALLOW_COPY_AND_ASSIGN(IInputArchive)
	public:
		ALLOW_DEFAULT_MOVE_AND_ASSIGN(IInputArchive)
		virtual IInputArchive& unstore(ISerializeable&) = delete;
	};

	template<typename Archive>
	class InputArchive //: private IInputArchive
	{
	public:
		using ArchiveType = Archive;
	private:

		inline ArchiveType& self() noexcept
		{
			return *static_cast<ArchiveType * const>(this);
		}

		// Serializes data
		template <typename T>
		inline void work(T&& head)
		{
			static_assert(!std::is_const_v<T>, "Cannot load into a const value T!");
			//static_assert(!std::is_lvalue_reference<T>::value, "Passed rvalue reference for loading from Input Archive! \n (Impossible since load can not write a value into a temporary)");
			self().beforework(head);
			self().dowork(head);
			self().afterwork(head);
		}

		// Recursion to unwind parameter list (has to be an overload for ADL lookup)
		template <typename T, typename ... Other>
		inline void work(T&& head, Other&& ... tail)
		{
			work(std::forward<T>(head));
			work(std::forward<Other>(tail)...);
		}

		template <typename T>
		inline std::enable_if_t<traits::use_prologue_member<T, ArchiveType>::value> beforework(T&& val)
		{
			self().prologue(val);
		}
		template <typename T>
		inline std::enable_if_t<traits::use_prologue_func<T, ArchiveType>::value> beforework(T&& val)
		{
			prologue(val, self());
		}
		template <typename T>
		inline std::enable_if_t<traits::no_prologue<T, ArchiveType>::value> beforework(T&&)
		{	}

		template <typename T>
		inline std::enable_if_t<traits::use_epilogue_member<T, ArchiveType>::value> afterwork(T&& val)
		{
			self().epilogue(val);
		}
		template <typename T>
		inline std::enable_if_t<traits::use_epilogue_func<T, ArchiveType>::value> afterwork(T&& val)
		{
			epilogue(val, self());
		}
		template <typename T>
		inline std::enable_if_t<traits::no_epilogue<T, ArchiveType>::value> afterwork(T&&)
		{	}
		
		//Archive Member Load. Archive knows how to load the type
		//If this is called with Type T, T must have been already constructed!
		//So either the Archive knows how to load T or the class itself knows how to do it.
		//The Archive never needs to know how to construct T itself!!!!
		template <typename T> inline
		std::enable_if_t<traits::use_archive_member_load_v<T, ArchiveType>, ArchiveType&> dowork(T&& value)
		{
			self().load(std::forward<T&>(value));
			return self();
		}

		//Member Load. Member loads itself
		template <typename T> inline
		std::enable_if_t<traits::use_member_load_v<T, ArchiveType>, ArchiveType&> dowork(T&& value)
		{
			value.load(self());
			return self();
		}

		//There is an external load function which knows how to load T from the Archive
		template <typename T> inline
		std::enable_if_t<traits::use_func_load_v<T, ArchiveType>, ArchiveType&> dowork(T&& value)
		{
			load(value, self());
			return self();
		}

		//There is an member serialization function which knows how to load T from the Archive
		template <typename T> inline
		std::enable_if_t<traits::use_member_serialize_load_v<T, ArchiveType>, ArchiveType&> dowork(T&& value)
		{
			value.serialize(self());
			return self();
		}

		//There is an external serilization function which knows how to load T from the Archive
		template <typename T> inline
		std::enable_if_t<traits::use_func_serialize_load_v<T, ArchiveType>, ArchiveType&> dowork(T&& value)
		{
			serialize(value, self());
			return self();
		}

		template <typename T> inline
		std::enable_if_t<traits::not_any_load_v<T, ArchiveType>, ArchiveType&> dowork(T&&)
		{
			//Game Over
			static_assert(!traits::not_any_load_v<T, ArchiveType>, "Type cannot be loaded from Archive. No implementation has been defined for it!");
#ifdef _MSC_VER
#ifdef __llvm__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif
			static_assert(!traits::not_any_load_v<T, ArchiveType>, __FUNCSIG__);
#ifdef __llvm__
#pragma clang diagnostic pop
#endif

#else
			static_assert(!traits::not_any_load_v<T, ArchiveType>);
#endif
		}

	protected:
		constexpr InputArchive(ArchiveType* const) noexcept {}
		DISALLOW_COPY_AND_ASSIGN(InputArchive)
	public:
		ALLOW_DEFAULT_MOVE_AND_ASSIGN(InputArchive)
		// Serializes all passed in data (only interface)
		template <typename ... Types>
		inline ArchiveType& operator()(Types&& ... args)
		{
			self().work(std::forward<Types>(args)...);
			return self();
		}
	};

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif	// INC_InputArchive_H
// end of InputArchive.h
///---------------------------------------------------------------------------------------------------
