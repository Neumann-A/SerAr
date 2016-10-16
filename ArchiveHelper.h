#pragma once

#include <type_traits>
#include "../Basic_Library/Headers/std_extensions.h"

namespace Archives
{
	namespace traits
	{
		//Function type for member save
		template<class T, typename ...Args>
		using member_save_t = decltype(std::declval<T&>().save(std::declval<Args&>()...));
		//Function type for save function
		template<class T, typename ...Args>
		using func_save_t = decltype(save(std::declval<T&>(), std::declval<Args&>()...));

		//Function type for member load
		template<class T, typename ...Args>
		using member_load_t = decltype(std::declval<T&>().load(std::declval<Args&>()...));
		//Function type for load function
		template<class T, typename ...Args>
		using func_load_t = decltype(load(std::declval<T&>(), std::declval<Args&>()...));

		//Function type for member serialize
		template<class T, typename ...Args>
		using member_serialize_t = decltype(std::declval<T&>().serialize(std::declval<Args&>()...));
		//Function type for serialize function
		template<class T, typename ...Args>
		using func_serialize_t = decltype(serialize(std::declval<T&>(), std::declval<Args&>()...));

		//Function type for member load
		template<class T, typename ...Args>
		using prologue_member_t = decltype(std::declval<T&>().prologue(std::declval<Args&>()...));
		//Function type for load function
		template<class T, typename ...Args>
		using epilogue_member_t = decltype(std::declval<T&>().epilogue(std::declval<Args&>()...));

		template<class T, typename ...Args>
		using prologue_member_t = decltype(std::declval<T&>().prologue(std::declval<Args&>()...));
		//Function type for load function
		template<class T, typename ...Args>
		using epilogue_member_t = decltype(std::declval<T&>().epilogue(std::declval<Args&>()...));

		template<typename ...Args>
		using prologue_func_t = decltype(prologue(std::declval<Args&>()...));
		//Function type for load function
		template<typename ...Args>
		using epilogue_func_t = decltype(epilogue(std::declval<Args&>()...));

		/**************************************Helpers*********************************************************/

		//Checks if Archive has a prologue member function
		template<typename ToTest, typename ArchiveType>
		class has_prologue_member : public stdext::is_detected<prologue_member_t, ArchiveType, ToTest> {};
		//Checks if Archive has a eiplogue member function
		template<typename ToTest, typename ArchiveType>
		class has_epilogue_member : public stdext::is_detected<epilogue_member_t, ArchiveType, ToTest> {};
		
		//Checks if Archive has a prologue function
		template<typename ToTest, typename ArchiveType>
		class has_prologue_func : public stdext::is_detected<prologue_member_t, ToTest, ArchiveType> {};
		//Checks if Archive has a eiplogue function
		template<typename ToTest, typename ArchiveType>
		class has_epilogue_func : public stdext::is_detected<epilogue_member_t, ToTest, ArchiveType> {};

		template<typename ToTest, typename ArchiveType>
		class use_prologue_member : public has_prologue_member<ToTest, ArchiveType> {};
		template<typename ToTest, typename ArchiveType>
		class use_epilogue_member : public has_epilogue_member<ToTest, ArchiveType> {};

		template<typename ToTest, typename ArchiveType>
		class use_prologue_func : public std::conjunction<has_prologue_func<ToTest, ArchiveType>, std::negation<has_prologue_member<ToTest, ArchiveType>>> {};
		template<typename ToTest, typename ArchiveType>
		class use_epilogue_func : public std::conjunction<has_prologue_func<ToTest, ArchiveType>, std::negation<has_epilogue_member<ToTest, ArchiveType>>> {};

		template<typename ToTest, typename ArchiveType>
		class no_prologue : public std::negation<std::disjunction<has_prologue_func<ToTest, ArchiveType>, has_prologue_member<ToTest, ArchiveType>>> {};
		template<typename ToTest, typename ArchiveType>
		class no_epilogue : public std::negation<std::disjunction<has_prologue_func<ToTest, ArchiveType>, has_epilogue_member<ToTest, ArchiveType>>> {};

		//Checks if ToTest has a save function for itself
		template<typename ToTest, typename ArchiveType>
		class has_member_save : public stdext::is_detected<member_save_t, ToTest, ArchiveType> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool has_member_save_v = has_member_save<ToTest, ArchiveType>::value;

		//Checks if there is a save function for ToTest
		template<typename ToTest, typename ArchiveType>
		class has_func_save : public stdext::is_detected<func_save_t, ToTest, ArchiveType> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool has_func_save_v = has_func_save<ToTest, ArchiveType>::value;

		//Checks if the Archive has a save function for ToTest
		template<typename ArchiveType, typename ToTest>
		class archive_has_member_save : public stdext::is_detected<member_save_t, ArchiveType, ToTest> {};
		template<typename ArchiveType, typename ToTest>
		constexpr bool archive_has_member_save_v = archive_has_member_save<ArchiveType, ToTest>::value;

		//Checks if the ToTest has a load function for itself
		template<typename ToTest, typename ArchiveType>
		class has_member_load : public stdext::is_detected<member_load_t, ToTest, ArchiveType> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool has_member_load_v = has_member_load<ToTest, ArchiveType>::value;
		//Checks if there is a load function for ToTest
		template<typename ToTest, typename ArchiveType>
		class has_func_load : public stdext::is_detected<func_load_t, ToTest, ArchiveType> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool has_func_load_v = has_func_load<ToTest, ArchiveType>::value;

		//Checks if the Archive has a load function for the type
		template<typename ArchiveType, typename ToTest>
		class archive_has_member_load : public stdext::is_detected<member_load_t, ArchiveType, ToTest> {};
		template<typename ArchiveType, typename ToTest>
		constexpr bool archive_has_member_load_v = archive_has_member_load<ArchiveType, ToTest>::value;

		//Checks if ToTest has a serialization function for itself
		template<typename ToTest, typename ArchiveType>
		class has_member_serialize : public stdext::is_detected<member_serialize_t, ToTest, ArchiveType> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool has_member_serialize_v = has_member_serialize<ToTest, ArchiveType>::value;
		//Checks if there is a serialize function for ToTest
		template<typename ToTest, typename ArchiveType>
		class has_func_serialize : public stdext::is_detected<func_serialize_t, ToTest, ArchiveType> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool has_func_serialize_v = has_func_serialize<ToTest, ArchiveType>::value;

		//Counts the save functions
		template<typename ToTest, typename ArchiveType>
		constexpr std::uint16_t count_save = archive_has_member_save_v<ArchiveType, ToTest> + has_member_save_v<ToTest, ArchiveType> + has_func_save_v<ToTest, ArchiveType> + has_member_serialize_v<ToTest, ArchiveType> + has_func_serialize_v<ToTest, ArchiveType>;

		//Counts the load functions
		template<typename ToTest, typename ArchiveType>
		constexpr std::uint16_t count_load = archive_has_member_load_v<ArchiveType, ToTest> + has_member_load_v<ToTest, ArchiveType> + has_func_load_v<ToTest, ArchiveType> + has_member_serialize_v<ToTest, ArchiveType> + has_func_serialize_v<ToTest, ArchiveType>;

		/****************************************Helpers for selection logic*********************************************************/
		//Helper Booleans to determine which save function to use!
		template<typename ToTest, typename ArchiveType>
		class use_archive_member_save : public archive_has_member_save<ArchiveType, ToTest> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_archive_member_save_v = use_archive_member_save<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class use_member_save : public std::conjunction< std::negation<use_archive_member_save<ToTest, ArchiveType>>, has_member_save<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_member_save_v = use_member_save<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class use_func_save : public std::conjunction<std::negation<use_member_save<ToTest, ArchiveType>> , has_func_save<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_func_save_v = use_func_save<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class use_member_serialize_save : public std::conjunction<std::negation<use_func_save<ToTest, ArchiveType>>, has_member_serialize<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_member_serialize_save_v = use_member_serialize_save<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class use_func_serialize_save : public std::conjunction<std::negation<use_member_serialize_save<ToTest, ArchiveType>> , has_func_serialize<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_func_serialize_save_v = use_func_serialize_save<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class has_type_save : public std::disjunction<has_member_save<ToTest, ArchiveType>,
			has_func_save< ToTest, ArchiveType>, has_member_serialize<ToTest, ArchiveType>,
			has_func_serialize<ToTest, ArchiveType> > {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool has_type_save_v = has_type_save<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class no_type_save : public std::negation<has_type_save<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool no_type_save_v = no_type_save<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class uses_type_save : public std::conjunction< has_type_save<ToTest, ArchiveType>,	std::negation<use_archive_member_save<ToTest, ArchiveType>>>{};
		template<typename ToTest, typename ArchiveType>
		constexpr bool uses_type_save_v = uses_type_save<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class not_any_save : public std::conjunction< std::negation<use_archive_member_save<ToTest, ArchiveType> >, no_type_save<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool not_any_save_v = not_any_save<ToTest, ArchiveType>::value;

		//Helper Booleans to determine which load function to use!
		template<typename ToTest, typename ArchiveType>
		class use_archive_member_load : public archive_has_member_load<ArchiveType, ToTest> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_archive_member_load_v = use_archive_member_load<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class use_member_load : public std::conjunction<std::negation<use_archive_member_load<ToTest, ArchiveType>>, has_member_load<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_member_load_v = use_member_load<ToTest, ArchiveType>::value;
		
		template<typename ToTest, typename ArchiveType>
		class use_func_load : public std::conjunction<std::negation<use_member_load<ToTest, ArchiveType>>, has_func_load<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_func_load_v = use_func_load<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class use_member_serialize_load : public std::conjunction<std::negation<use_func_load<ToTest, ArchiveType>>, has_member_serialize<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_member_serialize_load_v = use_member_serialize_load<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class use_func_serialize_load : public std::conjunction<std::negation<use_member_serialize_load<ToTest, ArchiveType>>, has_func_serialize<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool use_func_serialize_load_v = use_func_serialize_load<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class no_type_load : public std::negation<std::disjunction<use_func_serialize_load<ToTest, ArchiveType>,
																   use_member_serialize_load<ToTest, ArchiveType>,
																   use_func_load< ToTest, ArchiveType>,
																   use_member_load<ToTest, ArchiveType>>> {};

		template<typename ToTest, typename ArchiveType>
		constexpr bool no_type_load_v = no_type_load<ToTest, ArchiveType>::value;

		template<typename ToTest, typename ArchiveType>
		class not_any_load : public std::conjunction<std::negation<use_archive_member_load<ToTest, ArchiveType>>, no_type_load<ToTest, ArchiveType>> {};
		template<typename ToTest, typename ArchiveType>
		constexpr bool not_any_load_v = not_any_load<ToTest, ArchiveType>::value;

	}
}
