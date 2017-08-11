#include "HDF5_Type_Selector.h"

namespace HDF5_Wrapper
{
	//bool isTypeImmutable(const hid_t& dtype)
	//{
	//	if (dtype == H5T_IEEE_F32BE)
	//		return true;
	//	else if (dtype == H5T_IEEE_F32LE)
	//		return true;
	//	else if (dtype == H5T_IEEE_F64BE)
	//		return true;
	//	else if (dtype == H5T_IEEE_F64LE)
	//		return true;
	//	else if (dtype == H5T_STD_I8BE)
	//		return true;
	//	else if (dtype == H5T_STD_I8LE)
	//		return true;
	//	else if (dtype == H5T_STD_I16BE)
	//		return true;
	//	else if (dtype == H5T_STD_I16LE)
	//		return true;
	//	else if (dtype == H5T_STD_I32BE)
	//		return true;
	//	else if (dtype == H5T_STD_I32LE)
	//		return true;
	//	else if (dtype == H5T_STD_I64BE)
	//		return true;
	//	else if (dtype == H5T_STD_I64LE)
	//		return true;
	//	else if (dtype == H5T_STD_U8BE)
	//		return true;
	//	else if (dtype == H5T_STD_U8LE)
	//		return true;
	//	else if (dtype == H5T_STD_U16BE)
	//		return true;
	//	else if (dtype == H5T_STD_U16LE)
	//		return true;
	//	else if (dtype == H5T_STD_U32BE)
	//		return true;
	//	else if (dtype == H5T_STD_U32LE)
	//		return true;
	//	else if (dtype == H5T_STD_U64BE)
	//		return true;
	//	else if (dtype == H5T_STD_U64LE)
	//		return true;
	//	else if (dtype == H5T_STD_B8BE)
	//		return true;
	//	else if (dtype == H5T_STD_B8LE)
	//		return true;
	//	else if (dtype == H5T_STD_B16BE)
	//		return true;
	//	else if (dtype == H5T_STD_B16LE)
	//		return true;
	//	else if (dtype == H5T_STD_B32BE)
	//		return true;
	//	else if (dtype == H5T_STD_B32LE)
	//		return true;
	//	else if (dtype == H5T_STD_B64BE)
	//		return true;
	//	else if (dtype == H5T_STD_B64LE)
	//		return true;
	//	else if (dtype == H5T_STD_REF_OBJ)
	//		return true;
	//	else if (dtype == H5T_STD_REF_DSETREG)
	//		return true;
	//	else if (dtype == H5T_C_S1)
	//		return true;
	//	else if (dtype == H5T_NATIVE_CHAR)
	//		return true;
	//	else if (dtype == H5T_NATIVE_SCHAR)
	//		return true;
	//	else if (dtype == H5T_NATIVE_UCHAR)
	//		return true;
	//	else if (dtype == H5T_NATIVE_SHORT)
	//		return true;
	//	else if (dtype == H5T_NATIVE_USHORT)
	//		return true;
	//	else if (dtype == H5T_NATIVE_INT)
	//		return true;
	//	else if (dtype == H5T_NATIVE_UINT)
	//		return true;
	//	else if (dtype == H5T_NATIVE_LONG)
	//		return true;
	//	else if (dtype == H5T_NATIVE_ULONG)
	//		return true;
	//	else if (dtype == H5T_NATIVE_LLONG)
	//		return true;
	//	else if (dtype == H5T_NATIVE_ULLONG)
	//		return true;
	//	else if (dtype == H5T_NATIVE_FLOAT)
	//		return true;
	//	else if (dtype == H5T_NATIVE_DOUBLE)
	//		return true;
	//	else if (dtype == H5T_NATIVE_LDOUBLE)
	//		return true;
	//	else if (dtype == H5T_NATIVE_B8)
	//		return true;
	//	else if (dtype == H5T_NATIVE_B16)
	//		return true;
	//	else if (dtype == H5T_NATIVE_B32)
	//		return true;
	//	else if (dtype == H5T_NATIVE_B64)
	//		return true;
	//	else if (dtype == H5T_NATIVE_OPAQUE)
	//		return true;
	//	else if (dtype == H5T_NATIVE_HADDR)
	//		return true;
	//	else if (dtype == H5T_NATIVE_HSIZE)
	//		return true;
	//	else if (dtype == H5T_NATIVE_HSSIZE)
	//		return true;
	//	else if (dtype == H5T_NATIVE_HERR)
	//		return true;
	//	else if (dtype == H5T_NATIVE_HBOOL)
	//		return true;
	//	else if (dtype == H5T_NATIVE_INT8)
	//		return true;
	//	else if (dtype == H5T_NATIVE_UINT8)
	//		return true;
	//	else if (dtype == H5T_NATIVE_INT16)
	//		return true;
	//	else if (dtype == H5T_NATIVE_UINT16)
	//		return true;
	//	else if (dtype == H5T_NATIVE_INT32)
	//		return true;
	//	else if (dtype == H5T_NATIVE_UINT32)
	//		return true;
	//	else if (dtype == H5T_NATIVE_INT64)
	//		return true;
	//	else if (dtype == H5T_NATIVE_UINT64)
	//		return true;
	//	else
	//		return false; //TODO add more types!

	//}
}