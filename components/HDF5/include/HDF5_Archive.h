///---------------------------------------------------------------------------------------------------
// file:		HDF5_Archive.h
//
// summary: 	Declares the hdf 5 archive class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander
// date: 08.03.2017

#ifndef INC_HDF5_Archive_H
#define INC_HDF5_Archive_H
///---------------------------------------------------------------------------------------------------
#pragma once

#include <cassert>

#include <type_traits>

#include <filesystem>

#include <algorithm>
#include <functional>
#include <numeric>

#include <vector>
#include <array>
#include <utility>
#include <map>
#include <iosfwd>
#include <string>
#include <string_view>
#include <regex>
#include <complex>
#include <exception>
#include <memory>
#include <stack>
#include <hdf5.h>

#include <MyCEL/basics/BasicMacros.h>
#include <MyCEL/basics/BasicIncludes.h>
#include <MyCEL/stdext/std_extensions.h>

#include <SerAr/Core/NamedValue.h>
#include <SerAr/Core/NamedEnumVariant.hpp>
#include <SerAr/Core/InputArchive.h>
#include <SerAr/Core/OutputArchive.h>

#include "HDF5_Wrappers.h"
#include "HDF5_ArchiveHelpers.hpp"
#include "HDF5_InputArchive.hpp"
#include "HDF5_OutputArchive.hpp"

#endif	// INC_HDF5_Archive_H

///---------------------------------------------------------------------------------------------------
