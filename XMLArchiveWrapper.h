#pragma once

#include <ostream>
#include <istream>

#include <cereal\archives\xml.hpp>

#include "NamedValue.h"
#include "OutputArchive.h"
#include "InputArchive.h"
#include "CerealAdapter.h"

//Wrapper for cereal supported output formats
//TODO::Deal with special cases!
// Currently:: Forward every Type to cereal! This also means that Type has to follow cereals rule for serialization! 
// Maybe there is a more sophisticated way with an adapter type to make it work how our Archive type works!

namespace Archives
{
	class XMLOutArchive : public Archives::OutputArchive<XMLOutArchive>, public cereal::XMLOutputArchive
	{
	private:
		using WrapperArchive = XMLOutArchive;
		using WrapperBase = Archives::OutputArchive<WrapperArchive>;
		using Archive = cereal::XMLOutputArchive;
		using BaseArchive = cereal::OutputArchive<Archive>;

		DISALLOW_COPY_AND_ASSIGN(XMLOutArchive)
	public:
		XMLOutArchive(std::ostream& stream, const Archive::Options& options = Archive::Options::Default())
			: WrapperBase(this), Archive(stream, options) {};

		ALLOW_DEFAULT_MOVE_AND_ASSIGN(XMLOutArchive)

			template<typename T>
		inline std::enable_if_t<!helpers::traits::use_cereal_save_v<T, Archive>> save(const NamedValue<T>& value)
		{
			static_cast<Archive&>(*this)(cereal::make_nvp(value.getName(), helpers::CerealAdapter<T, WrapperArchive>{value.getValue()}));
		};

		// Output Endpoin; cereal knows how to write the type!
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

		// Output Endpoin; cereal knows how to write the type!
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

	class XMLInArchive : public InputArchive<XMLInArchive>, public cereal::XMLInputArchive
	{
	private:
		using WrapperArchive = XMLInArchive;
		using WrapperBase = Archives::InputArchive<WrapperArchive>;
		using Archive = cereal::XMLInputArchive;
		using BaseArchive = cereal::InputArchive<Archive>;

		DISALLOW_COPY_AND_ASSIGN(XMLInArchive)
	public:
		XMLInArchive(std::istream& stream) : WrapperBase(this), Archive(stream) {};

		ALLOW_DEFAULT_MOVE_AND_ASSIGN(XMLInArchive)

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