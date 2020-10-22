#pragma once

#include <ostream>
#include <istream>

#include <cereal\archives\portable_binary.hpp>

#include "NamedValue.h"
#include "OutputArchive.h"
#include "InputArchive.h"
#include "CerealAdapter.h"


//Wrapper for cereal supported output formats
//TODO::Deal with special cases!

namespace Archives
{
	class PortableBinaryOutArchive : public Archives::OutputArchive<PortableBinaryOutArchive>, public cereal::PortableBinaryOutputArchive
	{
	private:
		using WrapperArchive = PortableBinaryOutArchive;
		using WrapperBase = Archives::OutputArchive<WrapperArchive>;
		using Archive = cereal::PortableBinaryOutputArchive;
		using BaseArchive = cereal::OutputArchive<Archive>;

		DISALLOW_COPY_AND_ASSIGN(PortableBinaryOutArchive)
	public:
		PortableBinaryOutArchive(std::ostream& stream)
			: WrapperBase(this), Archive(stream) {};

		ALLOW_DEFAULT_MOVE_AND_ASSIGN(PortableBinaryOutArchive)

			template<typename T>
		inline std::enable_if_t<!helpers::traits::use_cereal_save_v<T, Archive>> save(const NamedValue<T>& value)
		{
			static_cast<Archive&>(*this)(cereal::make_nvp(value.getName(), helpers::CerealAdapter<T, WrapperArchive>{value.getValue()}));
		};

		// Output Endpoint; cereal knows how to write the type!
		template <typename T>
		inline std::enable_if_t<helpers::traits::use_cereal_save_v<T, Archive>> save(const NamedValue<T>& value)
		{
			static_cast<Archive&>(*this)(cereal::make_nvp(value.getName(), value.getValue()));
		}

		template<typename T>
		inline std::enable_if_t<!helpers::traits::use_cereal_save_v<T, Archive>> save(const T& value)
		{
			static_cast<Archive&>(*this)(helpers::CerealAdapter<T, WrapperArchive>{std::forward<T&&>(value)});
		};

		// Output Endpoint; cereal knows how to write the type!
		template <typename T>
		inline std::enable_if_t<helpers::traits::use_cereal_save_v<T, Archive>> save(const T& value)
		{
			static_cast<Archive&>(*this)(value);
		}

		template <typename ... Types>
		inline WrapperArchive& operator()(Types && ... args)
		{
			return static_cast< WrapperBase& >(*this)(std::forward<Types&&>(args)...);
		}

	};

	class PortableBinaryInArchive : public InputArchive<PortableBinaryInArchive>, public cereal::PortableBinaryInputArchive
	{
	private:
		using WrapperArchive = PortableBinaryInArchive;
		using WrapperBase = Archives::InputArchive<WrapperArchive>;
		using Archive = cereal::PortableBinaryInputArchive;
		using BaseArchive = cereal::InputArchive<Archive>;

		DISALLOW_COPY_AND_ASSIGN(PortableBinaryInArchive)
	public:
		PortableBinaryInArchive(std::istream& stream) : WrapperBase(this), Archive(stream) {};

		ALLOW_DEFAULT_MOVE_AND_ASSIGN(PortableBinaryInArchive)

		template<typename T>
		inline std::enable_if_t<!helpers::traits::use_cereal_load_v<T, Archive>> load(const NamedValue<T>& value)
		{
			static_cast<Archive&>(*this)(cereal::make_nvp(value.getName(), helpers::CerealAdapter<T, WrapperArchive>{value.getValue()}));
		};

		// Input Endpoint; cereal knows how to load the type!
		template <typename T>
		inline std::enable_if_t<helpers::traits::use_cereal_load_v<T, Archive>> load(const NamedValue<T>& value)
		{
			static_cast<Archive&>(*this)(cereal::make_nvp(value.getName(), value.getValue()));
		}

		template<typename T>
		inline std::enable_if_t<!helpers::traits::use_cereal_load_v<T, Archive>> load(const T& value)
		{
			static_cast<Archive&>(*this)(helpers::CerealAdapter<T, WrapperArchive>{std::forward<T&&>(value)});
		};

		// Input Endpoint; cereal knows how to load the type!
		template <typename T>
		inline std::enable_if_t<helpers::traits::use_cereal_load_v<T, Archive>> load(const T& value)
		{
			static_cast<Archive&>(*this)(value);
		}

		template <typename ... Types>
		inline WrapperArchive& operator()(Types && ... args)
		{
			return static_cast< WrapperBase& >(*this)(std::forward<Types&&>(args)...);
		}
	};
}
