///---------------------------------------------------------------------------------------------------
// file:        InputArchive.h
//
// summary:     Declares the input archive class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander Neumann
// date: 07.01.2017

#ifndef INC_InputArchive_H
#define INC_InputArchive_H
///---------------------------------------------------------------------------------------------------
#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4814) // constexpr is not implicit const Warning
#endif

#include <MyCEL/basics/BasicMacros.h>
#include <SerAr/Core/ArchiveHelper.h>

namespace Archives
{
    using namespace SerAr;
    template <typename OptionClass, typename Archive>
    class InputArchive_Options
    {
    public:
        using ArchiveType = Archive;
        using ArchiveOptions = OptionClass;

        inline ArchiveOptions& self() noexcept
        {
            return *static_cast<ArchiveOptions * const>(this);
        }
    };

    class ISerializeable; //Forward declaration

    class IInputArchive
    {
    protected:
        constexpr IInputArchive() = default;
        ~IInputArchive() = default;
        DISALLOW_COPY_AND_ASSIGN(IInputArchive)
    public:
        ALLOW_DEFAULT_MOVE_AND_ASSIGN(IInputArchive)
        virtual IInputArchive& unstore(ISerializeable&) = delete;
    };

    template<typename Archive>
    class InputArchive //: private IInputArchive
    {
    public:
        using ArchiveType = Archive;
    private:

        inline ArchiveType& self() noexcept
        {
            return *static_cast<ArchiveType * const>(this);
        }

        // Serializes data
        template <typename T>
        inline void work(T&& head)
        {
            static_assert(!std::is_const_v<T>, "Cannot load into a const value T!");
            //static_assert(!std::is_lvalue_reference<T>::value, "Passed rvalue reference for loading from Input Archive! \n (Impossible since load can not write a value into a temporary)");
            self().beforework(head);
            self().dowork(head);
            self().afterwork(head);
        }

        // Recursion to unwind parameter list (has to be an overload for ADL lookup)
        template <typename T, typename ... Other>
        inline void work(T&& head, Other&& ... tail)
        {
            work(std::forward<T>(head));
            work(std::forward<Other>(tail)...);
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
        
        //Archive Member Load. Archive knows how to load the type
        //If this is called with Type T, T must have been already constructed!
        //So either the Archive knows how to load T or the class itself knows how to do it.
        //The Archive never needs to know how to construct T itself!!!!
        template <typename T> requires (UseArchiveMemberLoad<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            self().load(std::forward<T>(value));
            return self();
        }
        template <typename T> requires (UseArchiveFunctionLoad<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            load(self(), std::forward<T>(value));
            return self();
        }
        //Member Load. Member loads itself
        template <typename T> requires (UseTypeMemberLoad<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            value.load(self());
            return self();
        }
        //There is an external load function which knows how to load T from the Archive
        template <typename T> requires (UseTypeFunctionLoad<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            load(std::forward<T>(value), self());
            return self();
        }


        //There is an member serialization function which knows how to load T from the Archive
        template <typename T> requires (UseTypeMemberSerialize<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            value.serialize(self());
            return self();
        }

        //There is an external serilization function which knows how to load T from the Archive
        template <typename T> requires (UseTypeFunctionSerialize<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            serialize(std::forward<T>(value), self());
            return self();
        }

        template <typename T> requires (UseArchiveMemberSerialize<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            self().serialize(std::forward<T>(value));
            return self();
        }
        //There is an external serilization function which knows how to load T from the Archive
        template <typename T> requires (UseArchiveFunctionSerialize<T,ArchiveType>)
        inline ArchiveType& dowork(T&& value) {
            serialize(self(), value);
            return self();
        }

    protected:
        constexpr InputArchive(ArchiveType* const) noexcept {}
        DISALLOW_COPY_AND_ASSIGN(InputArchive)
    public:
        ALLOW_DEFAULT_MOVE_AND_ASSIGN(InputArchive)
        // Serializes all passed in data (only interface)
        template <typename ... Types>
        inline ArchiveType& operator()(Types&& ... args)
        {
            self().work(std::forward<Types>(args)...);
            return self();
        }
    };

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif	// INC_InputArchive_H
// end of InputArchive.h
///---------------------------------------------------------------------------------------------------
