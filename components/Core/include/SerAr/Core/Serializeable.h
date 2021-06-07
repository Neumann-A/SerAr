///---------------------------------------------------------------------------------------------------
// file:		Serializeable.h
//
// summary: 	Declares the serializeable class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander Neumann
// date: 07.01.2017

#ifndef INC_Serializeable_H
#define INC_Serializeable_H
///---------------------------------------------------------------------------------------------------
#pragma once
#include <cassert>

#include <SerAr/Core/InputArchive.h>
#include <SerAr/Core/OutputArchive.h>

#define IMPLEMENT_INDEXABLE_CLASS(SomeClass) \
    static int& GetClassIndexStatic()\
    {\
        static int index = -1; \
        return index; \
    }\
    virtual int& GetClassIndex()\
    {\
        assert(typeid(*this) == typeid(SomeClass)); \
        return GetClassIndexStatic(); \
    }


namespace Archives
{
    class ISerializationHelper
    {
    public:
        virtual ISerializationHelper& deduceType() = delete;
    };

    template<typename Archive, typename Type>
    class SerializationHelper
    {

    };

    class ISerializeable
    {
    public:
        IMPLEMENT_INDEXABLE_CLASS(ISerializeable)

//		virtual void serialize(IInputArchive&) = 0;				//Runtime loading
//		virtual void serialize(IOutputArchive&) = 0;			//Runtime saving

        template <typename Archive>
        void serialize(OutputArchive<Archive>&& ar)
        {
            //This serialize functions is called by the Archive! So here we know the Archive type!
        }
        template <typename Archive>
        void serialize(InputArchive<Archive>&& ar)
        {
            //This serialize functions is called by the Archive! So here we know the Archive type!
        }
    };

    //Define abstract concept of a Serializeable Type 
    template<typename Type>
    class Serializeable : public ISerializeable
    {
        // Since the used Archive is not known at this point the class cannot use a static assertion
        // to verify if the given type can be serialized!
        // Here double dispatch with a vistor pattern in the archive class is used to determine all 
        // type at runtime 
    public:
        IMPLEMENT_INDEXABLE_CLASS(Serializeable)

        using SerializeableType = Type;
        //using ArchiveType = Archive;

        //Runtime loading
        void serialize(IInputArchive& inArchive) final
        {

        };
        //Runtime saving
        void serialize(IOutputArchive& outArchive) final
        {

        };
    };

    //Define abstract concept of loadable serializable type
    template<typename Type>
    class Serializeable_load
    {
    public:
        using SerializeableType = Type;

    };

    //Define abstract concept of a saveable serializeable type
    template<typename Type>
    class Serializeable_save
    {
    public:
        using SerializeableType = Type;

    };
}


#undef IMPLEMENT_INDEXABLE_CLASS(SomeClass)

#endif	// INC_Serializeable_H
// end of Serializeable.h
///---------------------------------------------------------------------------------------------------
