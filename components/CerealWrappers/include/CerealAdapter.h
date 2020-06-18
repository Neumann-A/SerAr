#pragma once

#include <type_traits>

#include "ArchiveHelper.h"
#include "CerealAdapter.h"

#include <cereal\cereal.hpp>

namespace Archives
{
	namespace helpers
	{
		namespace traits
		{
			using namespace Archives::traits;

			template<typename ToTest, typename CerealArchive>
			class cereal_has_save_func : public stdext::is_detected<func_save_t, CerealArchive, ToTest> {};
			template<typename ToTest, typename CerealArchive>
			class cereal_has_save_member : public stdext::is_detected<member_save_t, CerealArchive, ToTest> {};

			template<typename ToTest, typename CerealArchive>
			class cereal_has_load_func : public stdext::is_detected<func_load_t, CerealArchive, ToTest> {};
			template<typename ToTest, typename CerealArchive>
			class cereal_has_load_member : public stdext::is_detected<member_load_t, CerealArchive, ToTest> {};

			template<typename ToTest, typename CerealArchive>
			class use_cereal_save_member : public cereal_has_save_member<ToTest, CerealArchive> {};
			template<typename ToTest, typename CerealArchive>
			constexpr bool use_cereal_save_member_v = use_cereal_save_member<ToTest, CerealArchive>::value;
			template<typename ToTest, typename CerealArchive>
			class use_cereal_save_func : public std::conjunction<cereal_has_save_func<ToTest, CerealArchive>, std::negation<use_cereal_save_member<ToTest, CerealArchive>>> {};
			template<typename ToTest, typename CerealArchive>
			constexpr bool use_cereal_save_func_v = use_cereal_save_func<ToTest, CerealArchive>::value;

			template<typename ToTest, typename CerealArchive>
			class use_cereal_save : public std::disjunction<use_cereal_save_member<ToTest, CerealArchive>, use_cereal_save_func<ToTest, CerealArchive>> {};
			template<typename ToTest, typename CerealArchive>
			constexpr bool use_cereal_save_v = use_cereal_save<ToTest, CerealArchive>::value;

			template<typename ToTest, typename WrapperArchive>
			class has_load : public  std::disjunction<has_member_load<ToTest, WrapperArchive>, has_func_load<ToTest, WrapperArchive>> {};
			template<typename ToTest, typename WrapperArchive>
			class has_save : public  std::disjunction<has_member_save<ToTest, WrapperArchive>, has_func_save<ToTest, WrapperArchive>> {};

			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_member_save : public std::conjunction<has_member_save<ToTest, WrapperArchive>,
				std::negation<use_cereal_save<ToTest, CerealArchive> >> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_member_save_v = use_wrapper_member_save<ToTest, WrapperArchive, CerealArchive>::value;

			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_func_save : public std::conjunction<has_func_save<ToTest, WrapperArchive>,
				std::negation<std::disjunction<use_cereal_save<ToTest, CerealArchive>, use_wrapper_member_save<ToTest, WrapperArchive, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_func_save_v = use_wrapper_func_save<ToTest, WrapperArchive, CerealArchive>::value;

			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_member_serialize_output : public std::conjunction<has_member_serialize<ToTest, WrapperArchive>, 
				std::negation<std::disjunction<has_save<ToTest, WrapperArchive>, has_load<ToTest, WrapperArchive>, use_cereal_save<ToTest, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_member_serialize_output_v = use_wrapper_member_serialize_output<ToTest, WrapperArchive, CerealArchive>::value;
		
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_func_serialize_output : public std::conjunction<has_func_serialize<ToTest, WrapperArchive>,
				std::negation<std::disjunction<use_wrapper_member_serialize_output<ToTest, WrapperArchive, CerealArchive>,
				has_load<ToTest, WrapperArchive>,has_save<ToTest, WrapperArchive> , use_cereal_save<ToTest, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_func_serialize_output_v = use_wrapper_func_serialize_output<ToTest, WrapperArchive, CerealArchive>::value;

			//TODO: name correctly serialize as save!
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_member_save_with_serialize : public std::conjunction<has_member_serialize<ToTest, WrapperArchive>, has_load<ToTest, WrapperArchive>,
				std::negation<std::disjunction<has_save<ToTest, WrapperArchive>,use_cereal_save<ToTest, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_member_save_with_serialize_v = use_wrapper_member_save_with_serialize<ToTest, WrapperArchive, CerealArchive>::value;

			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_func_save_with_serialize : public std::conjunction<has_func_serialize<ToTest, WrapperArchive>,	has_load<ToTest, WrapperArchive>,
				std::negation<std::disjunction<has_save<ToTest, WrapperArchive>, use_cereal_save<ToTest, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_func_save_with_serialize_v = use_wrapper_func_save_with_serialize<ToTest, WrapperArchive, CerealArchive>::value;

			template<typename ToTest, typename CerealArchive>
			class use_cereal_load_member : public cereal_has_load_member<ToTest, CerealArchive> {};
			template<typename ToTest, typename CerealArchive>
			constexpr bool use_cereal_load_member_v = use_cereal_load_member<ToTest, CerealArchive>::value;

			template<typename ToTest, typename CerealArchive>
			class use_cereal_load_func : public std::conjunction<cereal_has_load_func<ToTest, CerealArchive>, std::negation<use_cereal_load_member<ToTest, CerealArchive>>> {};
			template<typename ToTest, typename CerealArchive>
			constexpr bool use_cereal_load_func_v = use_cereal_load_func<ToTest, CerealArchive>::value;
			
			template<typename ToTest, typename CerealArchive>
			class use_cereal_load : public std::disjunction<use_cereal_load_member<ToTest, CerealArchive>, use_cereal_load_func<ToTest, CerealArchive>> {};
			template<typename ToTest, typename CerealArchive>
			constexpr bool use_cereal_load_v = use_cereal_load<ToTest, CerealArchive>::value;
			//***
			
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_member_load : public std::conjunction<has_member_load<ToTest, WrapperArchive>,
				std::negation<use_cereal_load<ToTest, CerealArchive> >> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_member_load_v = use_wrapper_member_load<ToTest, WrapperArchive, CerealArchive>::value;

			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_func_load : public std::conjunction<has_func_load<ToTest, WrapperArchive>,
				std::negation<std::disjunction<use_cereal_load<ToTest, CerealArchive>, use_wrapper_member_load<ToTest, WrapperArchive, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_func_load_v = use_wrapper_func_load<ToTest, WrapperArchive, CerealArchive>::value;

			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_member_serialize_input : public std::conjunction<has_member_serialize<ToTest, WrapperArchive>,
				std::negation<std::disjunction<has_save<ToTest, WrapperArchive>, has_load<ToTest, WrapperArchive>, use_cereal_load<ToTest, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_member_serialize_input_v = use_wrapper_member_serialize_input<ToTest, WrapperArchive, CerealArchive>::value;

			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_func_serialize_input : public std::conjunction<has_func_serialize<ToTest, WrapperArchive>,
				std::negation<std::disjunction<use_wrapper_member_serialize_input<ToTest, WrapperArchive, CerealArchive>,
				has_load<ToTest, WrapperArchive>, has_save<ToTest, WrapperArchive>, use_cereal_load<ToTest, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_func_serialize_input_v = use_wrapper_func_serialize_input<ToTest, WrapperArchive, CerealArchive>::value;

			//TODO: Name correctly serialize as load
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_member_load_with_serialize : public std::conjunction<has_member_serialize<ToTest, WrapperArchive>, has_save<ToTest, WrapperArchive>,
				std::negation<std::disjunction<has_load<ToTest, WrapperArchive>, use_cereal_load<ToTest, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_member_load_with_serialize_v = use_wrapper_member_load_with_serialize<ToTest, WrapperArchive, CerealArchive>::value;

			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			class use_wrapper_func_load_with_serialize : public std::conjunction<has_func_serialize<ToTest, WrapperArchive>, has_save<ToTest, WrapperArchive>,
				std::negation<std::disjunction<has_load<ToTest, WrapperArchive>, use_cereal_load<ToTest, CerealArchive> >>> {};
			template<typename ToTest, typename WrapperArchive, typename CerealArchive>
			constexpr bool use_wrapper_func_load_with_serialize_v = use_wrapper_func_load_with_serialize<ToTest, WrapperArchive, CerealArchive>::value;
		
			/*******************************************************/
		}
		
	
		template<typename T,typename WrappingArchive>
		class CerealAdapter
		{
		public:
			//If T is an Array: keep the type
			//If T is a l value: store a reference (T&)
			//If T is a r value: copy the value (T) (if used as intended this is an unlikly case but sometimes happens)
			using type = typename std::conditional<std::is_array<typename std::remove_reference<T>::type>::value,
				typename std::remove_cv<T>::type,
				typename std::conditional<std::is_lvalue_reference<T>::value,
				T, typename std::decay<T>::type>::type>::type;
			
		private:
			T&& Data;
		public:
			explicit CerealAdapter(T&& value) : Data(value) {};
			//ALLOW_DEFAULT_COPY_AND_ASSIGN(CerealAdapter)
			ALLOW_DEFAULT_MOVE_AND_ASSIGN(CerealAdapter)

			//For Output Archives
			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_member_serialize_output_v<type,WrappingArchive,Archive>> serialize(Archive &ar)
			{
				Data.serialize(static_cast<WrappingArchive&>( ar ));
			}
			
			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_func_serialize_output_v<type, WrappingArchive, Archive>> serialize(Archive &ar)
			{
				serialize(Data, static_cast<WrappingArchive&> (ar ));
			}

			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_member_save_with_serialize_v<type, WrappingArchive, Archive>>  save(Archive &ar)
			{
				Data.serialize(static_cast<WrappingArchive&>( ar ));
			}

			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_func_save_with_serialize_v<type, WrappingArchive, Archive>> save(Archive &ar)
			{
				serialize(Data, static_cast<WrappingArchive&>( ar ));
			}

			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_member_save_v<type, WrappingArchive, Archive>> save(Archive &ar) const
			{
				//auto myar = WrappingArchive{ ar };
				Data.save(static_cast<WrappingArchive&>(ar));
			}

			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_func_save_v<type, WrappingArchive, Archive>> save(Archive &ar) const
			{
				save(Data, static_cast<WrappingArchive&>(ar ));
			}

			// For Input Archives
			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_member_serialize_input_v<type, WrappingArchive, Archive>> serialize(Archive &ar)
			{
				Data.serialize(static_cast<WrappingArchive&>(ar ));
			}

			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_func_serialize_input_v<type, WrappingArchive, Archive>> serialize(Archive &ar)
			{
				serialize(Data, static_cast<WrappingArchive&>(ar));
			}

			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_member_load_with_serialize_v<type, WrappingArchive, Archive>>  load(Archive &ar)
			{
				Data.serialize(static_cast<WrappingArchive&>(ar));
			}

			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_func_load_with_serialize_v<type, WrappingArchive, Archive>> load(Archive &ar)
			{
				serialize(Data, static_cast<WrappingArchive&>(ar ));
			}

			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_member_load_v<type, WrappingArchive, Archive>> load(Archive &ar) const
			{
				Data.load(static_cast<WrappingArchive&>(ar ));
			}
			template <typename Archive>
			inline std::enable_if_t<traits::use_wrapper_func_load_v<type, WrappingArchive, Archive>> load(Archive &ar) const
			{
				load(Data, static_cast<WrappingArchive&>(ar));
			}

		};

	}
}