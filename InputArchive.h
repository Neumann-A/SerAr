#pragma once

#pragma warning(push)

#pragma warning( disable : 4814) // constexpr is not implicit const Warning

#include "..\Basic_Library\Headers\BasicMacros.h"
#include "ArchiveHelper.h"

namespace Archives
{
	class IInputArchive
	{
	protected:
		constexpr IInputArchive() = default;
		~IInputArchive() = default;
		DISALLOW_COPY_AND_ASSIGN(IInputArchive)
	public:
		ALLOW_DEFAULT_MOVE_AND_ASSIGN(IInputArchive)
	};

	template<typename Archive>
	class InputArchive : private IInputArchive
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
			load(self(), value);
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
			serialize(self(), value);
			return self();
		}

		template <typename T> inline
		std::enable_if_t<traits::not_any_load_v<T, ArchiveType>, ArchiveType&> dowork(T&& value)
		{
			//Game Over
			static_assert(!traits::not_any_load_v<T, ArchiveType>, "Type cannot be loaded from Archive. No implementation has been defined for it!");
			static_assert(!traits::not_any_save_v<T, ArchiveType>, __FUNCSIG__);
		}

		//template <typename T> inline
		//ArchiveType& dowork(T&& value)
		//{
		//	//Game Over
		//	static_assert(false, "Logic error! Archive cannot load into an rvalue!");
		//}


	protected:
		constexpr InputArchive(ArchiveType* const) noexcept {};
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

#include "NamedValue.h"

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
		using type = ToConstruct;

		static_assert(std::is_constructible<type,void>::value,"Type is not constructable without arguments. Archives need specialization of LoadConstructor ");
		
		template <typename Archive>
		static inline type construct(InputArchive<Archive>& ar)
		{
			type ConstructedType{};
			ar(ConstructedType);
			return ConstructedType;
		};

		template <typename Archive>
		static inline type constructWithName(InputArchive<Archive>& ar, char const * const name)
		{
			type ConstructedType{};
			ar(Basic::createNamedValue(name,ConstructedType));
			return ConstructedType;
		};

		template <typename Archive>
		static inline type constructWithName(InputArchive<Archive>& ar, const std::string& name)
		{
			type ConstructedType{};
			ar(Basic::createNamedValue(name, ConstructedType));
			return ConstructedType;
		};

		template <typename Archive>
		static inline type constructWithName(const std::string& name, InputArchive<Archive>& ar)
		{
			return constructWithName(ar, name);
		};

		template <typename Archive>
		static inline type constructWithName(char const * const name, InputArchive<Archive>& ar)
		{
			return constructWithName(ar, name);
		};

	};
}

#pragma warning(pop)