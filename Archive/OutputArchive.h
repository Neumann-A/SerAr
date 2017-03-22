///---------------------------------------------------------------------------------------------------
// file:		OutputArchive.h
//
// summary: 	Declares the output archive class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander Neumann
// date: 07.01.2017

#ifndef INC_OutputArchive_H
#define INC_OutputArchive_H
///---------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma once
#pragma warning(push)
#endif

//#pragma warning( disable : 4814) // in c++14 constexpr is not implicit const 

#include "basics/BasicMacros.h"
#include "ArchiveHelper.h"

namespace Archives
{
	class ISerializeable; //Forward declaration

	//template <class Archive, class T> 
	//inline void prologue(const T&, Archive&)
	//{ }

	//template <class Archive, class T> 
	//inline void epilogue(const T& , Archive&)
	//{ }
	
	//TODO:: Define general Options!
	template <typename OptionClass, typename Archive>
	class OutputArchive_Options
	{
	public:
		using ArchiveType = Archive;
		using ArchiveOptions = OptionClass;

		inline ArchiveOptions& self() noexcept
		{
			return *static_cast<ArchiveOptions * const>(this);
		}
	};
	
	class IOutputArchive
	{
	protected:
		constexpr IOutputArchive() = default;
		~IOutputArchive() = default;
		DISALLOW_COPY_AND_ASSIGN(IOutputArchive)
	public:
		ALLOW_DEFAULT_MOVE_AND_ASSIGN(IOutputArchive)
		virtual IOutputArchive& store(ISerializeable&) = delete;
	};

	template <typename Archive>
	class OutputArchive : private IOutputArchive
	{
	public:
		using ArchiveType = Archive;
		
		//const OutputArchive_Options& getArchiveOptions()
		//{
		//	return mOptions;
		//}

	private:
		inline ArchiveType& self() noexcept
		{
			return *static_cast<ArchiveType * const>(this);
		}

		// Does all the work for only element
		template <typename T>
		inline void worksplitter(T&& head)
		{			
			//std::cout << "Called: " << __FUNCTION__  << "\n" << " with Type: " << typeid(head).name() << std::endl;
			// Would be nice if we could forward here!
			self().beforework(head);
			self().dowork(head);
			self().afterwork(head);
		}

		// Compile Time Recursion to unwind parameter list (has to be an overload for ADL lookup)
		template <typename T, typename ... Other>
		inline void worksplitter(T&& head, Other&& ... tail)
		{
			worksplitter(std::forward<T>(head));
			worksplitter(std::forward<Other>(tail)...);
		}

		template <typename T>
		inline std::enable_if_t<traits::use_prologue_member<T, ArchiveType>::value> beforework(T&& val)
		{ 
			self().prologue(std::forward<T>(val));
		}
		template <typename T>
		inline std::enable_if_t<traits::use_prologue_func<T, ArchiveType>::value> beforework(T&& val)
		{
			prologue(std::forward<T>(val), self());
		}
		template <typename T>
		inline std::enable_if_t<traits::no_prologue<T, ArchiveType>::value> beforework(T&&)
		{	}

		template <typename T>
		inline std::enable_if_t<traits::use_epilogue_member<T, ArchiveType>::value> afterwork(T&& val)
		{
			self().epilogue(std::forward<T>(val));
		}
		template <typename T>
		inline std::enable_if_t<traits::use_epilogue_func<T, ArchiveType>::value> afterwork(T&& val)
		{
			epilogue(std::forward<T>(val), self());
		}
		template <typename T>
		inline std::enable_if_t<traits::no_epilogue<T, ArchiveType>::value> afterwork(T&&)
		{	}

		//Archive can directly save the Type (should be the case for all POD types)
		template <typename T> inline
		std::enable_if_t<traits::use_archive_member_save_v<std::decay_t<T>, ArchiveType>, ArchiveType&> dowork(T&& value)
		{
			self().save(std::forward<T>(value));
			return self();
		}

		//Member save. Type knows how to save itself
		template <typename T> inline
		std::enable_if_t<traits::use_member_save_v<std::decay_t<T>,ArchiveType>, ArchiveType&> dowork(T&& value)
		{
			const_cast<T&>(value).save(self());
			return self();
		}

		//There is an external save function which knows how to save T to the Archive
		template <typename T> inline
		std::enable_if_t<traits::use_func_save_v<std::decay_t<T>, ArchiveType>, ArchiveType&> dowork(T&& value)
		{
			save(std::forward<T>(value), self());
			return self();
		}

		//There is an member serialization function which knows how to save T to the Archive
		template <typename T> inline
		std::enable_if_t<traits::use_member_serialize_save_v<std::decay_t<T>, ArchiveType>, ArchiveType&> dowork(T&& value) //Serialize can not be const!
		{

			const_cast<std::remove_const_t<std::remove_reference_t<T>>&>(value).serialize(self());
			return self();
		}

		//There is an external serilization function which knows how to save T to the Archive
		template <typename T> inline
		std::enable_if_t<traits::use_func_serialize_save_v<std::decay_t<T>, ArchiveType>, ArchiveType&> dowork(T&& value) //Serialize can not be const!
		{
			serialize(const_cast<std::remove_const_t<std::remove_reference_t<T>>&>(value), self());
			return self();
		}

		//We do not have a clue how to save/serialize T....
		template <typename T = void> inline
		std::enable_if_t<Archives::traits::not_any_save_v<std::decay_t<T>, ArchiveType>, ArchiveType&> dowork(T&&)
		{
			//Game Over
			static_assert(!traits::not_any_save_v<T, ArchiveType>, "Type cannot be saved to Archive. No implementation has been defined for it!");
#ifdef _MSC_VER
			static_assert(!traits::not_any_save_v<T, ArchiveType>, __FUNCSIG__);
#else
			static_assert(!traits::not_any_save_v<T, ArchiveType>);
#endif
			return self();
		}

	protected:
		constexpr OutputArchive(ArchiveType * const) noexcept {};
		//CRTP class should not be assigned and copied
		DISALLOW_COPY_AND_ASSIGN(OutputArchive)
	public:
		ALLOW_DEFAULT_MOVE_AND_ASSIGN(OutputArchive)
		// Serializes all passed in data (only interface)
		template <typename ... Types>
		inline ArchiveType& operator()(Types&& ... args)
		{
			self().worksplitter(std::forward<Types>(args)...);
			return self();
		}
	};
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif	// INC_OutputArchive_H
// end of OutputArchive.h
///---------------------------------------------------------------------------------------------------
