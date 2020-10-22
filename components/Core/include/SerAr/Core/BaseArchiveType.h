///---------------------------------------------------------------------------------------------------
// file:		BaseArchiveType.h
//
// summary: 	Declares the base archive type class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander Neumann
// date: 07.01.2017

#ifndef INC_BaseArchiveType_H
#define INC_BaseArchiveType_H
///---------------------------------------------------------------------------------------------------
#pragma once

#include <SerAr/Core/Serializeable.h>

namespace Archives
{

	class ArchiveBase
	{
	protected:
		ArchiveBase() = default;
	public:
		virtual ~ArchiveBase() = default;

		virtual ArchiveBase& store(ISerializeable&) = delete;
		virtual ArchiveBase& unstore(ISerializeable&) = delete;
	};

}

#endif	// INC_BaseArchiveType_H
// end of BaseArchiveType.h
///---------------------------------------------------------------------------------------------------
