///---------------------------------------------------------------------------------------------------
// file:		OutputArchive.h
//
// summary: 	Declares the output archive class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander Neumann
// date: 07.01.2017

#ifndef INC_OutputArchive_H
#define INC_OutputArchive_H
///---------------------------------------------------------------------------------------------------
#pragma once

#include <MyCEL/basics/BasicMacros.h>
#include <SerAr/Core/ArchiveHelper.h>
#include <cassert>

namespace Archives
{
    using namespace SerAr;
    class ISerializeable; //Forward declaration

    //TODO:: Define general Options!
    template <typename OptionClass, typename Archive>
    class OutputArchive_Options
    {
    public:
        using ArchiveType = Archive;
        using ArchiveOptions = OptionClass;

        inline ArchiveOptions& self() noexcept
        {
            return *static_cast<ArchiveOptions * const>(this);
        }
    };
    
    class IOutputArchive
    {
    protected:
        constexpr IOutputArchive() = default;
        virtual ~IOutputArchive() = default;
        DISALLOW_COPY_AND_ASSIGN(IOutputArchive)
    public:
        ALLOW_DEFAULT_MOVE_AND_ASSIGN(IOutputArchive)
        virtual IOutputArchive& store(ISerializeable&) = delete;
    };

    template <typename Archive>
    class OutputArchive //: private IOutputArchive
    {
    public:
        using ArchiveType = Archive;
        
        //const OutputArchive_Options& getArchiveOptions()
        //{
        //	return mOptions;
        //}

    private:
        inline ArchiveType& self() noexcept
        {
            return *static_cast<ArchiveType * const>(this);
        }

        // Does all the work for only element
        template <typename T>
        inline void worksplitter(T&& head)
        {			
            //std::cout << "Called: " << __FUNCTION__  << "\n" << " with Type: " << typeid(head).name() << std::endl;
            // Would be nice if we could forward here!
            self().beforework(head);
            self().dowork(head);
            self().afterwork(head);
        }

        // Compile Time Recursion to unwind parameter list (has to be an overload for ADL lookup)
        template <typename T, typename ... Other>
        inline void worksplitter(T&& head, Other&& ... tail)
        {
            worksplitter(std::forward<T>(head));
            worksplitter(std::forward<Other>(tail)...);
        }

        template <typename T> requires (UseArchiveMemberPrologue<T,ArchiveType>)
        inline void beforework(T&& val) { self().prologue(std::forward<T>(val)); }
        template <typename T> requires (UseArchiveFunctionPrologue<T,ArchiveType>)
        inline void beforework(T&& val) { prologue(self(),std::forward<T>(val)); }
        template <typename T> requires (UseTypeMemberPrologue<T,ArchiveType>)
        inline void beforework(T&& val) { val.prologue(self()); }
        template <typename T> requires (UseTypeFunctionPrologue<T,ArchiveType>)
        inline void beforework(T&& val) { prologue(std::forward<T>(val), self()); }
        template <typename T> requires (!HasPrologue<T,ArchiveType>)
        inline void beforework(T&&) { }

        template <typename T> requires (UseArchiveMemberEpilogue<T,ArchiveType>)
        inline void afterwork(T&& val) { self().epilogue(std::forward<T>(val)); }
        template <typename T> requires (UseArchiveFunctionEpilogue<T,ArchiveType>)
        inline void afterwork(T&& val) { epilogue(self(),std::forward<T>(val)); }
        template <typename T> requires (UseTypeMemberEpilogue<T,ArchiveType>)
        inline void afterwork(T&& val) { val.epilogue(self()); }
        template <typename T> requires (UseTypeFunctionEpilogue<T,ArchiveType>)
        inline void afterwork(T&& val) { epilogue(std::forward<T>(val), self()); }
        template <typename T> requires (!HasEpilogue<T,ArchiveType>)
        inline void afterwork(T&&) { }

        //Archive Member Save. Archive knows how to save the type
        //If this is called with Type T, T must have been already constructed!
        //So either the Archive knows how to save T or the class itself knows how to do it.
        //The Archive never needs to know how to construct T itself!!!!
        template <typename T> requires (UseArchiveMemberSave<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            self().save(std::forward<T>(value));
            return self();
        }
        template <typename T> requires (UseArchiveFunctionSave<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            save(self(), std::forward<T>(value));
            return self();
        }
        //Member Save. Member saves itself
        template <typename T> requires (UseTypeMemberSave<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            const_cast<T&>(value).save(self());
            return self();
        }
        //There is an external save function which knows how to save T from the Archive
        template <typename T> requires (UseTypeFunctionSave<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            save(std::forward<T>(value), self());
            return self();
        }

        //There is an member serialization function which knows how to save T from the Archive
        template <typename T> requires (UseTypeMemberSerialize<std::remove_cvref_t<T>,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            const_cast<std::remove_cvref_t<T>&>(value).serialize(self());
            return self();
        }

        //There is an external serilization function which knows how to save T from the Archive
        template <typename T> requires (UseTypeFunctionSerialize<std::remove_cvref_t<T>,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            serialize(const_cast<std::remove_cvref_t<T>&>(value), self());
            return self();
        }

        template <typename T> requires (UseArchiveMemberSerialize<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            self().serialize(std::forward<T>(value));
            return self();
        }
        //There is an external serilization function which knows how to save T from the Archive
        template <typename T> requires (UseArchiveFunctionSerialize<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            serialize(self(), value);
            return self();
        }

//        //We do not have a clue how to save/serialize T....
//        template <typename T = void> requires (! (IsSaveable<T,ArchiveType> || IsSerializeable<T,ArchiveType>))
//        inline ArchiveType& dowork(T&&)
//        {
//            assert(false);
//            //Game Over
//            static_assert(std::is_same_v<std::remove_const_t<T>,const T>, "Type cannot be saved to Archive. No implementation has been defined for it!");
//#ifdef _MSC_VER
//#ifdef __llvm__
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wlanguage-extension-token"
//#endif
//            //static_assert(!(IsSaveable<T,ArchiveType> || IsSerializeable<T,ArchiveType>), __FUNCSIG__);
//#ifdef __llvm__
//#pragma clang diagnostic pop
//#endif
//#else
//            static_assert(! (IsSaveable<T,ArchiveType> || IsSerializeable<T,ArchiveType>));
//#endif
//            return self();
//        }

    protected:
        constexpr OutputArchive(ArchiveType * const) noexcept {}
        
        //CRTP class should not be assigned and copied
        DISALLOW_COPY_AND_ASSIGN(OutputArchive)
    public:
        ALLOW_DEFAULT_MOVE_AND_ASSIGN(OutputArchive)
        // Serializes all passed in data (only interface)
        template <typename ... Types>
        inline ArchiveType& operator()(Types&& ... args)
        {
            self().worksplitter(std::forward<Types>(args)...);
            return self();
        }
    };
}

#endif	// INC_OutputArchive_H
// end of OutputArchive.h
///---------------------------------------------------------------------------------------------------
