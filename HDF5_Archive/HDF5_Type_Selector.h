#pragma once

#include <cstdint>
#include <complex>

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
			if constexpr (std::is_arithmetic_v<T> || stdext::is_string_v<T> || stdext::is_complex_v<T>)
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
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::int8_t>, hid_t> getType(const T&)	{
			return H5T_NATIVE_INT8;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::int16_t>, hid_t> getType(const T&) {
			return H5T_NATIVE_INT16;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::int32_t>, hid_t> getType(const T&) {
			return H5T_NATIVE_INT32;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::int64_t>, hid_t> getType(const T&) {
			return H5T_NATIVE_INT64;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint8_t>, hid_t> getType(const T&) {
			return H5T_NATIVE_UINT8;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint16_t>, hid_t> getType(const T&) {
			return H5T_NATIVE_UINT16;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint32_t>, hid_t> getType(const T&) {
			return H5T_NATIVE_UINT32;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint64_t>, hid_t> getType(const T&) {
			return H5T_NATIVE_UINT64;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, float>, hid_t> getType(const T&) {
			return H5T_NATIVE_FLOAT;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, double>, hid_t> getType(const T&) {
			return H5T_NATIVE_DOUBLE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, long double>, hid_t> getType(const T&) {
			return H5T_NATIVE_LDOUBLE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<stdext::is_complex_v<T>, hid_t> getType(const T& val) {
			const std::array<std::size_t, 1> size{ { 2 } };
			return H5Tarray_create(getType(val.real()), 1, size.data());
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, const char*> || std::is_same_v<T, char*> || std::is_same_v<T, char>, hid_t> getType(const T&) {
			return H5T_C_S1;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, bool>, hid_t> getType(const T&) {
			return H5T_NATIVE_B8;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<stdext::is_string_v<T>, hid_t> getType(const T&) {
			auto hdf5typeid = H5Tcopy(H5T_C_S1);
			//H5Tset_size(hdf5typeid, val.size());
			H5Tset_size(hdf5typeid, H5T_VARIABLE);
			return hdf5typeid;
		};
	};
	template<>
	struct DatatypeSelector<HDF5_Datatype::LittleEndian>
	{
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::int8_t>, hid_t> getType(const T&) {
			return H5T_STD_I8LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::int16_t>, hid_t> getType(const T&) {
			return H5T_STD_I16LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::int32_t>, hid_t> getType(const T&) {
			return H5T_STD_I32LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::int64_t>, hid_t> getType(const T&) {
			return H5T_STD_I64LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint8_t>, hid_t> getType(const T&) {
			return H5T_STD_U8LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint16_t>, hid_t> getType(const T&) {
			return H5T_STD_U16LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint32_t>, hid_t> getType(const T&) {
			return H5T_STD_U32LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint64_t>, hid_t> getType(const T&) {
			return H5T_STD_U64LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, float>, hid_t> getType(const T&) {
			return H5T_IEEE_F32LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, double>, hid_t> getType(const T&) {
			return H5T_IEEE_F64LE;
		};
		//template<typename T>
		//inline static constexpr std::enable_if_t<std::is_same_v<T, long double>, hid_t> getType(const T&) {
		//	return H5T_NATIVE_LDOUBLE;
		//};
		template<typename T>
		inline static constexpr std::enable_if_t<stdext::is_complex_v<T>, hid_t> getType(const T& val) {
			const std::array<std::size_t, 1> size{ { 2 } };
			return H5Tarray_create(getType(val.real()), 1, size.data());
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, const char*> || std::is_same_v<T, char*> || std::is_same_v<T, char>, hid_t> getType(const T&) {
			return H5T_C_S1;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<std::is_same_v<T, bool>, hid_t> getType(const T&) {
			return H5T_STD_B8LE;
		};
		template<typename T>
		inline static constexpr std::enable_if_t<stdext::is_string_v<T>, hid_t> getType(const T&) {
			auto hdf5typeid = H5Tcopy(H5T_C_S1);
			H5Tset_size(hdf5typeid, H5T_VARIABLE);
			return hdf5typeid;
		};
	};
	template<>
	struct DatatypeSelector<HDF5_Datatype::BigEndian>
	{
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, std::int8_t>, hid_t> getType(const T&) {
				return H5T_STD_I8BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, std::int16_t>, hid_t> getType(const T&) {
				return H5T_STD_I16BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, std::int32_t>, hid_t> getType(const T&) {
				return H5T_STD_I32BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, std::int64_t>, hid_t> getType(const T&) {
				return H5T_STD_I64BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint8_t>, hid_t> getType(const T&) {
				return H5T_STD_U8BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint16_t>, hid_t> getType(const T&) {
				return H5T_STD_U16BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint32_t>, hid_t> getType(const T&) {
				return H5T_STD_U32BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, std::uint64_t>, hid_t> getType(const T&) {
				return H5T_STD_U64BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, float>, hid_t> getType(const T&) {
				return H5T_IEEE_F32BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, double>, hid_t> getType(const T&) {
				return H5T_IEEE_F64BE;
			};
			//template<typename T>
			//inline static constexpr std::enable_if_t<std::is_same_v<T, long double>, hid_t> getType(const T&) {
			//	return H5T_NATIVE_LDOUBLE;
			//};
			template<typename T>
			inline static constexpr std::enable_if_t<stdext::is_complex_v<T>, hid_t> getType(const T& val) {
				const std::array<std::size_t, 1> size{ { 2 } };
				return H5Tarray_create(getType(val.real()), 1, size.data());
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, const char*> || std::is_same_v<T, char*> || std::is_same_v<T, char>, hid_t> getType(const T&) {
				return H5T_C_S1;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<std::is_same_v<T, bool>, hid_t> getType(const T&) {
				return H5T_STD_B8BE;
			};
			template<typename T>
			inline static constexpr std::enable_if_t<stdext::is_string_v<T>, hid_t> getType(const T&) {
				auto hdf5typeid = H5Tcopy(H5T_C_S1);
				H5Tset_size(hdf5typeid, H5T_VARIABLE);
				return hdf5typeid;
			}; 			
	};
	struct DatatypeRuntimeSelector
	{
		template<typename T>
		inline static constexpr auto getType(const HDF5_Datatype& type, const T& val)
		{
			switch (type)
			{
			case HDF5_Datatype::Native:
				return DatatypeSelector<HDF5_Datatype::Native>::getType(val);
			case HDF5_Datatype::LittleEndian:
				return DatatypeSelector<HDF5_Datatype::LittleEndian>::getType(val);
			case HDF5_Datatype::BigEndian:
				return DatatypeSelector<HDF5_Datatype::BigEndian>::getType(val);
			default:
				return DatatypeSelector<HDF5_Datatype::Native>::getType(val);
			}
		}
	};

	template<class T>
	using get_HDF5_datatyp_t = decltype(DatatypeSelector<HDF5_Datatype::Native>::getType(std::declval<std::decay_t<T&>>()));

	template<typename T>
	struct has_HDF5_datatype : stdext::is_detected_exact<hid_t, get_HDF5_datatyp_t, T> {};

	// Maybe copying in the selector is better than this if statement
	inline bool isTypeImmutable(const hid_t& dtype) 
	{
		if (dtype == H5T_IEEE_F32BE)
			return true;
		else if (dtype == H5T_IEEE_F32LE)
			return true;
		else if (dtype == H5T_IEEE_F64BE)
			return true;
		else if (dtype == H5T_IEEE_F64LE)
			return true;
		else if (dtype == H5T_STD_I8BE)
			return true;
		else if (dtype == H5T_STD_I8LE)
			return true;
		else if (dtype == H5T_STD_I16BE)
			return true;
		else if (dtype == H5T_STD_I16LE)
			return true;
		else if (dtype == H5T_STD_I32BE)
			return true;
		else if (dtype == H5T_STD_I32LE)
			return true;
		else if (dtype == H5T_STD_I64BE)
			return true;
		else if (dtype == H5T_STD_I64LE)
			return true;
		else if (dtype == H5T_STD_U8BE)
			return true;
		else if (dtype == H5T_STD_U8LE)
			return true;
		else if (dtype == H5T_STD_U16BE)
			return true;
		else if (dtype == H5T_STD_U16LE)
			return true;
		else if (dtype == H5T_STD_U32BE)
			return true;
		else if (dtype == H5T_STD_U32LE)
			return true;
		else if (dtype == H5T_STD_U64BE)
			return true;
		else if (dtype == H5T_STD_U64LE)
			return true;
		else if (dtype == H5T_STD_B8BE)
			return true;
		else if (dtype == H5T_STD_B8LE)
			return true;
		else if (dtype == H5T_STD_B16BE)
			return true;
		else if (dtype == H5T_STD_B16LE)
			return true;
		else if (dtype == H5T_STD_B32BE)
			return true;
		else if (dtype == H5T_STD_B32LE)
			return true;
		else if (dtype == H5T_STD_B64BE)
			return true;
		else if (dtype == H5T_STD_B64LE)
			return true;
		else if (dtype == H5T_STD_REF_OBJ)
			return true;
		else if (dtype == H5T_STD_REF_DSETREG)
			return true;
		else if (dtype == H5T_C_S1)
			return true;
		else if (dtype == H5T_NATIVE_CHAR)
			return true;
		else if (dtype == H5T_NATIVE_SCHAR)
			return true;
		else if (dtype == H5T_NATIVE_UCHAR)
			return true;
		else if (dtype == H5T_NATIVE_SHORT)
			return true;
		else if (dtype == H5T_NATIVE_USHORT)
			return true;
		else if (dtype == H5T_NATIVE_INT)
			return true;
		else if (dtype == H5T_NATIVE_UINT)
			return true;
		else if (dtype == H5T_NATIVE_LONG)
			return true;
		else if (dtype == H5T_NATIVE_ULONG)
			return true;
		else if (dtype == H5T_NATIVE_LLONG)
			return true;
		else if (dtype == H5T_NATIVE_ULLONG)
			return true;
		else if (dtype == H5T_NATIVE_FLOAT)
			return true;
		else if (dtype == H5T_NATIVE_DOUBLE)
			return true;
		else if (dtype == H5T_NATIVE_LDOUBLE)
			return true;
		else if (dtype == H5T_NATIVE_B8)
			return true;
		else if (dtype == H5T_NATIVE_B16)
			return true;
		else if (dtype == H5T_NATIVE_B32)
			return true;
		else if (dtype == H5T_NATIVE_B64)
			return true;
		else if (dtype == H5T_NATIVE_OPAQUE)
			return true;
		else if (dtype == H5T_NATIVE_HADDR)
			return true;
		else if (dtype == H5T_NATIVE_HSIZE)
			return true;
		else if (dtype == H5T_NATIVE_HSSIZE)
			return true;
		else if (dtype == H5T_NATIVE_HERR)
			return true;
		else if (dtype == H5T_NATIVE_HBOOL)
			return true;
		else if (dtype == H5T_NATIVE_INT8)
			return true;
		else if (dtype == H5T_NATIVE_UINT8)
			return true;
		else if (dtype == H5T_NATIVE_INT16)
			return true;
		else if (dtype == H5T_NATIVE_UINT16)
			return true;
		else if (dtype == H5T_NATIVE_INT32)
			return true;
		else if (dtype == H5T_NATIVE_UINT32)
			return true;
		else if (dtype == H5T_NATIVE_INT64)
			return true;
		else if (dtype == H5T_NATIVE_UINT64)
			return true;
		else
			return false; //TODO add more types!

	};
	
}