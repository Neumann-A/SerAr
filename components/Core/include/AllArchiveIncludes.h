///---------------------------------------------------------------------------------------------------
// file:		Archive\AllArchiveIncludes.h
//
// summary: 	Declares all archive includes 
//
// Copyright (c) 2019 Alexander Neumann.
//
// author: Neumann
// date: 10.05.2019
#pragma once
#ifndef INC_AllArchiveIncludes_H
#define INC_AllArchiveIncludes_H
///---------------------------------------------------------------------------------------------------

#ifdef SERAR_HAS_MATLAB
#include <SerAr/MATLAB/Matlab_Archive.h>
#else
namespace Archives {
	class MatlabOptions;
	class MatlabOutputArchive;
	class MatlabInputArchive;
}
#endif
#ifdef SERAR_HAS_HDF5
#include <SerAr/HDF5/HDF5_Archive.h>
#else
namespace Archives {
	class HDF5_OutputArchive;
	class HDF5_InputArchive;
}
#endif

#endif	// INC_AllArchiveIncludes_H
// end of Archive\AllArchiveIncludes.h
///---------------------------------------------------------------------------------------------------

