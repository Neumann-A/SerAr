#pragma once

#include <cstdint>

#include <type_traits>

#include <hdf5.h>
#include <hdf5_hl.h>

#include "stdext/std_extensions.h"

namespace HDF5_Wrapper
{

	template<typename T, typename = void>
	struct DataspaceTypeSelector
	{
		//A scalar dataspace, H5S_SCALAR, has a single element, though that element may be of a complex datatype, such as a compound or array datatype.
		//A simple dataspace, H5S_SIMPLE, consists of a regular array of elements.
		//A null dataspace, H5S_NULL, has no data elements.
		static constexpr inline H5S_class_t value()
		{
			if constexpr (std::is_arithmetic_v<T> || stdext::is_string_v<T>)
				return  H5S_SCALAR;
			else if constexpr (stdext::is_container_v<T>)
				return  H5S_SIMPLE;
			else if constexpr (stdext::is_eigen_type_v<T> || stdext::is_eigen_tensor_v<T>)
				return  H5S_SIMPLE;
			else
				return H5S_NO_CLASS;
		};
	};

	enum class HDF5_Datatype { Native, LittleEndian, BigEndian };

	template<HDF5_Datatype types = HDF5_Datatype::Native>
	struct DatatypeSelector {};

	template<>
	struct DatatypeSelector<HDF5_Datatype::Native>
	{
		template<typename T>
		inline static constexpr auto getType()
		{
			if constexpr(std::is_same_v<T, std::int8_t>) {
				return H5T_NATIVE_INT8;
			}
			else if constexpr(std::is_same_v<T, std::int16_t>) {
				return H5T_NATIVE_INT16;
			}
			else if constexpr(std::is_same_v<T, std::int32_t>) {
				return H5T_NATIVE_INT32;
			}
			else if constexpr(std::is_same_v<T, std::int64_t>) {
				return H5T_NATIVE_INT64;
			}
			else if constexpr(std::is_same_v<T, std::uint8_t>) {
				return H5T_NATIVE_UINT8;
			}
			else if constexpr(std::is_same_v<T, std::uint16_t>) {
				return H5T_NATIVE_UINT16;
			}
			else if constexpr(std::is_same_v<T, std::uint32_t>) {
				return H5T_NATIVE_UINT32;
			}
			else if constexpr(std::is_same_v<T, std::uint64_t>) {
				return H5T_NATIVE_UINT64;
			}
			else if constexpr(std::is_same_v<T, float>) {
				return H5T_NATIVE_FLOAT;
			}
			else if constexpr(std::is_same_v<T, double>) {
				return H5T_NATIVE_DOUBLE;
			}
			else if constexpr(std::is_same_v<T, long double>) {
				return H5T_NATIVE_LDOUBLE;
			}
			else if constexpr(std::is_same_v<T, const char*> || std::is_same_v<T, char*> || std::is_same_v<T, char>) {
				return H5T_C_S1;
			}
			else if constexpr(std::is_same_v<T, bool>) {
				return H5T_NATIVE_B8;
			}
			else if constexpr(stdext::is_string_v<T>) {
				return H5Tcreate(H5T_STRING, H5T_VARIABLE);
			}
			else {
				static_assert(!std::is_same_v<T, void>, "Type definied!");
			}
		}
	};
	template<>
	struct DatatypeSelector<HDF5_Datatype::LittleEndian>
	{
		template<typename T>
		inline static constexpr auto getType()
		{
			if constexpr(std::is_same_v<T, std::int8_t>) {
				return H5T_STD_I8LE;
			}
			else if constexpr(std::is_same_v<T, std::int16_t>) {
				return H5T_STD_I16LE;
			}
			else if constexpr(std::is_same_v<T, std::int32_t>) {
				return H5T_STD_I32LE;
			}
			else if constexpr(std::is_same_v<T, std::int64_t>) {
				return H5T_STD_I64LE;
			}
			else if constexpr(std::is_same_v<T, std::uint8_t>) {
				return H5T_STD_U8LE;
			}
			else if constexpr(std::is_same_v<T, std::uint16_t>) {
				return H5T_STD_U16LE;
			}
			else if constexpr(std::is_same_v<T, std::uint32_t>) {
				return H5T_STD_U32LE;
			}
			else if constexpr(std::is_same_v<T, std::uint64_t>) {
				return H5T_STD_U64LE;
			}
			else if constexpr(std::is_same_v<T, float>) {
				return H5T_IEEE_F32LE;
			}
			else if constexpr(std::is_same_v<T, double>) {
				return H5T_IEEE_F64LE;
			}
			//else if constexpr(std::is_same_v<T, long double>) {
			//	return H5T_NATIVE_LDOUBLE;
			//}
			else if constexpr(std::is_same_v<T, const char*> || std::is_same_v<T, char*> || std::is_same_v<T, char>) {
				return H5T_C_S1;
			}
			else if constexpr(std::is_same_v<T, bool>) {
				return H5T_STD_B8LE;
			}
			else if constexpr(stdext::is_string_v<T>) {
				return H5Tcreate(H5T_STRING, H5T_VARIABLE);
			}
			else {
				static_assert(!std::is_same_v<T, void>, "Type not definied!");
			}
		}
	};
	template<>
	struct DatatypeSelector<HDF5_Datatype::BigEndian>
	{
		template<typename T>
		inline static constexpr auto getType()
		{
			if constexpr(std::is_same_v<T, std::int8_t>) {
				return H5T_STD_I8BE;
			}
			else if constexpr(std::is_same_v<T, std::int16_t>) {
				return H5T_STD_I16BE;
			}
			else if constexpr(std::is_same_v<T, std::int32_t>) {
				return H5T_STD_I32BE;
			}
			else if constexpr(std::is_same_v<T, std::int64_t>) {
				return H5T_STD_I64BE;
			}
			else if constexpr(std::is_same_v<T, std::uint8_t>) {
				return H5T_STD_U8BE;
			}
			else if constexpr(std::is_same_v<T, std::uint16_t>) {
				return H5T_STD_U16BE;
			}
			else if constexpr(std::is_same_v<T, std::uint32_t>) {
				return H5T_STD_U32BE;
			}
			else if constexpr(std::is_same_v<T, std::uint64_t>) {
				return H5T_STD_U64BE;
			}
			else if constexpr(std::is_same_v<T, float>) {
				return H5T_IEEE_F32BE;
			}
			else if constexpr(std::is_same_v<T, double>) {
				return H5T_IEEE_F64BE;
			}
			//else if constexpr(std::is_same_v<T, long doubBE>) {
			//	return H5T_NATIVE_LDOUBBE;
			//}
			else if constexpr(std::is_same_v<T, const char*> || std::is_same_v<T, char*> || std::is_same_v<T, char>) {
				return H5T_C_S1;
			}
			else if constexpr(std::is_same_v<T, bool>) {
				return H5T_STD_B8BE;
			}
			else if constexpr(stdext::is_string_v<T>) {
				return H5Tcreate(H5T_STRING, H5T_VARIABLE);
			}
			else {
				static_assert(!std::is_same_v<T, void>, "Type definied!");
			}
		}
	};
	struct DatatypeRuntimeSelector
	{
		template<typename T>
		inline static constexpr auto getType(const HDF5_Datatype& type)
		{
			switch (type)
			{
			case HDF5_Datatype::Native:
				return DatatypeSelector<HDF5_Datatype::Native>::getType<T>();
			case HDF5_Datatype::LittleEndian:
				return DatatypeSelector<HDF5_Datatype::LittleEndian>::getType<T>();
			case HDF5_Datatype::BigEndian:
				return DatatypeSelector<HDF5_Datatype::BigEndian>::getType<T>();
			}
		}

		template<typename T>
		inline static constexpr auto getType(const HDF5_Datatype& type, const T&)
		{
			return getType<std::decay_t<T>>(type);
		}
	};
}