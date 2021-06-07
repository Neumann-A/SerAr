///---------------------------------------------------------------------------------------------------
// file:		ArchiveVisitor.h
//
// summary: 	Declares the archive visitor class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander Neumann
// date: 07.01.2017

#ifndef INC_ArchiveVisitor_H
#define INC_ArchiveVisitor_H
///---------------------------------------------------------------------------------------------------
#pragma once

// Visitor pattern is stupid because all derived classes of archive must be known! 
// Makes basiclly use of overload resolution with the derived classes -> hell to maintain -> No go

class IArchiveVisitor
{
public:
    template <typename Archive>
    void dispatch(Archive&& ar)
    {
        ar(type);
    }
};

template<typename Type>
class ArchiveVisitor : public IArchiveVisitor
{
    template <typename Archive>
    void dispatch(Archive&& ar)
    {
        ar(type);
    }
};
#endif	// INC_ArchiveVisitor_H
// end of ArchiveVisitor.h
///---------------------------------------------------------------------------------------------------
