///---------------------------------------------------------------------------------------------------
// file:		HDF5_FwdDecl.h
//
// summary: 	Contains the HDF5 forward declartions
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander
// date: 26.08.2017

#ifndef INC_HDF5_FwdDecl_H
#define INC_HDF5_FwdDecl_H
///---------------------------------------------------------------------------------------------------
#pragma once


namespace HDF5_Wrapper
{
	//Forward Declarations
	class HDF5_FileWrapper;
	class HDF5_GroupWrapper;
	class HDF5_DatasetWrapper;
	class HDF5_DataspaceWrapper;
	class HDF5_AttributeWrapper;
	class HDF5_DatatypeWrapper;
	class HDF5_LocationWrapper;
	template<typename T>
	class HDF5_GeneralType;
	template<typename T>
	struct HDF5_OpenCreateCloseWrapper;

	struct HDF5_DummyOptions {};
	struct HDF5_GeneralOptions;
	struct HDF5_FileOptions;
	struct HDF5_GroupOptions;
	struct HDF5_DatasetOptions;
	//Extra Options without linkage
	struct HDF5_DataspaceOptions;
	struct HDF5_DatatypeOptions;


	inline bool isTypeImmutable(const hid_t& dtype);
}
#endif	// INC_HDF5_FwdDecl_H
// end of HDF5_FwdDecl.h
///---------------------------------------------------------------------------------------------------
