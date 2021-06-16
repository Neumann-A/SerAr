///---------------------------------------------------------------------------------------------------
// file:        ArchiveHelper.h
//
// summary:     Declares the archive helper class
//
// Copyright (c) 2017 Alexander Neumann.
//
// author: Alexander Neumann
// date: 07.01.2017

#ifndef INC_ArchiveHelper_H
#define INC_ArchiveHelper_H
///---------------------------------------------------------------------------------------------------
#pragma once

#include <type_traits>
#include <MyCEL/stdext/std_extensions.h>

namespace Archives {
    template<typename T>
    class InputArchive;
    template<typename T>
    class OutputArchive;
}

namespace SerAr
{
    template<typename Archive>
    concept IsInputArchive = std::is_base_of_v<::Archives::InputArchive<Archive>, Archive>;
    template<typename Archive>
    concept IsOutputArchive = std::is_base_of_v<::Archives::OutputArchive<Archive>, Archive>;
    template<typename Archive>
    concept IsArchive = IsInputArchive<Archive> || IsOutputArchive<Archive>;

    namespace details {
        template<typename T, typename U>
        concept MemberSaveable = requires(T&& t, U&& u) {
            t.save(u);
        };
        template<typename T, typename U>
        concept FunctionSaveable = requires(T&& t, U&& u) {
            save(t, u);
        };
        template<typename T, typename U>
        concept MemberLoadable = requires(T&& t, U&& u) {
            t.load(u);
        };
        template<typename T, typename U>
        concept FunctionLoadable = requires(T&& t, U&& u) {
            load(t, u);
        };
        template<typename T, typename U>
        concept MemberSerializeable = requires(T&& t, U&& u) {
            t.serialize(u);
        };
        template<typename T, typename U>
        concept FunctionSerializeable = requires(T&& t, U&& u) {
            serialize(t, u);
        };
        template<typename T, typename U>
        concept MemberPrologue = requires(T&& t, U&& u) {
            t.prologue(u);
        };
        template<typename T, typename U>
        concept FunctionPrologue = requires(T&& t, U&& u) {
            prologue(t, u);
        };
        template<typename T, typename U>
        concept MemberEpilogue = requires(T&& t, U&& u) {
            t.epilogue(u);
        };
        template<typename T, typename U>
        concept FunctionEpilogue = requires(T&& t, U&& u) {
            epilogue(t, u);
        };
    }
    // Saveable concepts
    template<typename Type, typename Ar>
    concept TypeMemberSaveable = details::MemberSaveable<Type, Ar> && IsOutputArchive<Ar>;
    template<typename Type, typename Ar>
    concept TypeFunctionSaveable = details::FunctionSaveable<Type, Ar> && IsOutputArchive<Ar>;
    template<typename Type, typename Ar>
    concept ArchiveMemberSaveable = details::MemberSaveable<Ar, Type> && IsOutputArchive<Ar>;
    template<typename Type, typename Ar>
    concept ArchiveFunctionSaveable = details::FunctionSaveable<Ar, Type> && IsOutputArchive<Ar>;
    template<typename Type, typename Ar>
    concept IsSaveable = (TypeMemberSaveable<Type,Ar> || TypeFunctionSaveable<Type,Ar> || ArchiveMemberSaveable<Type,Ar> || ArchiveFunctionSaveable<Type,Ar>);

    // Use Save concepts
    template<typename Type, typename Ar>
    concept UseArchiveMemberSave = ArchiveMemberSaveable<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseArchiveFunctionSave = !UseArchiveMemberSave<Type,Ar> && ArchiveFunctionSaveable<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseArchiveSave = (UseArchiveMemberSave<Type, Ar> || UseArchiveFunctionSave<Type, Ar>);
    template<typename Type, typename Ar>
    concept UseTypeMemberSave = !UseArchiveSave<Type,Ar> && TypeMemberSaveable<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseTypeFunctionSave = !UseArchiveSave<Type,Ar> && TypeFunctionSaveable<Type,Ar>;

    // Loadable concepts
    template<typename Type, typename Ar>
    concept TypeMemberLoadable = details::MemberLoadable<Type, Ar> && IsInputArchive<Ar>;
    template<typename Type, typename Ar>
    concept TypeFunctionLoadable = details::FunctionLoadable<Type, Ar> && IsInputArchive<Ar>;
    template<typename Type, typename Ar>
    concept ArchiveMemberLoadable = details::MemberLoadable<Ar, Type> && IsInputArchive<Ar>;
    template<typename Type, typename Ar>
    concept ArchiveFunctionLoadable = details::FunctionLoadable<Ar, Type> && IsInputArchive<Ar>;
    template<typename Type, typename Ar>
    concept IsLoadable = (TypeMemberLoadable<Type,Ar> || TypeFunctionLoadable<Type,Ar> || ArchiveMemberLoadable<Type,Ar> || ArchiveFunctionLoadable<Type,Ar>);
    // Use Load concepts
    template<typename Type, typename Ar>
    concept UseArchiveMemberLoad = ArchiveMemberLoadable<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseArchiveFunctionLoad = !UseArchiveMemberLoad<Type,Ar> && ArchiveFunctionLoadable<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseArchiveLoad = (UseArchiveMemberLoad<Type, Ar> || UseArchiveFunctionLoad<Type, Ar>);
    template<typename Type, typename Ar>
    concept UseTypeMemberLoad = !UseArchiveLoad<Type,Ar> && TypeMemberLoadable<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseTypeFunctionLoad = !UseArchiveLoad<Type,Ar> && TypeFunctionLoadable<Type,Ar>;


    // Serializeable concept
    template<typename Type, typename Ar>
    concept TypeMemberSerializeable = details::MemberSerializeable<Type, Ar> && IsArchive<Ar>;
    template<typename Type, typename Ar>
    concept TypeFunctionSerializeable = details::FunctionSerializeable<Type, Ar> && IsArchive<Ar>;
    template<typename Type, typename Ar>
    concept ArchiveMemberSerializeable = details::MemberSerializeable<Ar, Type> && IsArchive<Ar>;
    template<typename Type, typename Ar>
    concept ArchiveFunctionSerializeable = details::FunctionSerializeable<Ar, Type> && IsArchive<Ar>;
    template<typename Type, typename Ar>
    concept IsSerializeable = (TypeMemberSerializeable<Type,Ar> || TypeFunctionSerializeable<Type,Ar> || ArchiveMemberSerializeable<Type,Ar> || ArchiveFunctionSerializeable<Type,Ar>);
    // Use Serialize concepts
    template<typename Type, typename Ar>
    concept UseArchiveMemberSerialize = ArchiveMemberSerializeable<Type,Ar> && !(UseArchiveLoad<Type, Ar>|| UseArchiveSave<Type, Ar>);
    template<typename Type, typename Ar>
    concept UseArchiveFunctionSerialize = !UseArchiveMemberSerialize<Type,Ar> && ArchiveFunctionSerializeable<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseArchiveSerialize = (UseArchiveMemberSerialize<Type, Ar> || UseArchiveFunctionSerialize<Type, Ar>);
    template<typename Type, typename Ar>
    concept UseArchive = (UseArchiveSerialize<Type, Ar> || UseArchiveLoad<Type, Ar> || UseArchiveSave<Type, Ar>);

    template<typename Type, typename Ar>
    concept UseTypeMemberSerialize = !UseArchive<Type,Ar> && TypeMemberSerializeable<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseTypeFunctionSerialize = !UseArchive<Type,Ar> && TypeFunctionSerializeable<Type,Ar>;

    // Prologue concept
    template<typename Type, typename Ar>
    concept TypeMemberPrologue = IsArchive<Ar> && details::MemberPrologue<Type, Ar>;
    template<typename Type, typename Ar>
    concept TypeFunctionPrologue = IsArchive<Ar> && details::FunctionPrologue<Type, Ar>;
    template<typename Type, typename Ar>
    concept ArchiveMemberPrologue = IsArchive<Ar> && details::MemberPrologue<Ar, Type>;
    template<typename Type, typename Ar>
    concept ArchiveFunctionPrologue = IsArchive<Ar> && details::FunctionPrologue<Ar, Type>;
    template<typename Type, typename Ar>
    concept HasPrologue = TypeMemberPrologue<Type,Ar> || TypeFunctionPrologue<Type,Ar> || ArchiveMemberPrologue<Type,Ar> || ArchiveFunctionPrologue<Type,Ar>;
    // Use Prologue concepts
    template<typename Type, typename Ar>
    concept UseArchiveMemberPrologue = ArchiveMemberPrologue<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseArchiveFunctionPrologue = !UseArchiveMemberPrologue<Type,Ar> && ArchiveFunctionPrologue<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseTypeMemberPrologue = !UseArchiveFunctionPrologue<Type,Ar> && TypeMemberPrologue<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseTypeFunctionPrologue = !UseArchiveFunctionPrologue<Type,Ar> && TypeFunctionPrologue<Type,Ar>;
    // Epilogue concept
    template<typename Type, typename Ar>
    concept TypeMemberEpilogue = IsArchive<Ar> && details::MemberEpilogue<Type, Ar>;
    template<typename Type, typename Ar>
    concept TypeFunctionEpilogue = IsArchive<Ar> && details::FunctionEpilogue<Type, Ar>;
    template<typename Type, typename Ar>
    concept ArchiveMemberEpilogue = IsArchive<Ar> && details::MemberEpilogue<Ar, Type>;
    template<typename Type, typename Ar>
    concept ArchiveFunctionEpilogue = IsArchive<Ar> && details::FunctionEpilogue<Ar, Type>;
    template<typename Type, typename Ar>
    concept HasEpilogue = TypeMemberEpilogue<Type,Ar> || TypeFunctionEpilogue<Type,Ar> || ArchiveMemberEpilogue<Type,Ar> || ArchiveFunctionEpilogue<Type,Ar>;
    // Use Epilogue concepts
    template<typename Type, typename Ar>
    concept UseArchiveMemberEpilogue = ArchiveMemberEpilogue<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseArchiveFunctionEpilogue = !UseArchiveMemberEpilogue<Type,Ar> && ArchiveFunctionEpilogue<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseTypeMemberEpilogue = !UseArchiveFunctionEpilogue<Type,Ar> && TypeMemberEpilogue<Type,Ar>;
    template<typename Type, typename Ar>
    concept UseTypeFunctionEpilogue = !UseArchiveFunctionEpilogue<Type,Ar> && TypeFunctionEpilogue<Type,Ar>;

    //
    template<typename Type, typename Ar>
    concept IsTypeSaveable = (TypeMemberSerializeable<Type, Ar> || TypeFunctionSerializeable<Type, Ar> || TypeMemberSaveable<Type, Ar> || TypeFunctionSaveable<Type, Ar>);
    template<typename Type, typename Ar>
    concept IsTypeLoadable = (TypeMemberSerializeable<Type, Ar> || TypeFunctionSerializeable<Type, Ar> || TypeMemberLoadable<Type, Ar> || TypeFunctionLoadable<Type, Ar>);
}

namespace Archives
{
    namespace traits
    {
        //Function type for member save
        template<class T, typename ...Args>
        using member_save_t = decltype(std::declval<T&>().save(std::declval<Args&>()...));

        //Function type for save function
        template<class T, typename ...Args>
        using func_save_t = decltype(save(std::declval<T&>(), std::declval<Args&>()...));

        //Function type for member load
        template<class T, typename ...Args>
        using member_load_t = decltype(std::declval<T&>().load(std::declval<Args&>()...));
        //Function type for load function
        template<class T, typename ...Args>
        using func_load_t = decltype(load(std::declval<T&>(), std::declval<Args&>()...));

        //Function type for member serialize
        template<class T, typename ...Args>
        using member_serialize_t = decltype(std::declval<T&>().serialize(std::declval<Args&>()...));
        //Function type for serialize function
        template<class T, typename ...Args>
        using func_serialize_t = decltype(serialize(std::declval<T&>(), std::declval<Args&>()...));

        //Function type for member prologue
        template<class T, typename ...Args>
        using prologue_member_t = decltype(std::declval<T&>().prologue(std::declval<Args&>()...));
        //Function type for member epilogue
        template<class T, typename ...Args>
        using epilogue_member_t = decltype(std::declval<T&>().epilogue(std::declval<Args&>()...));

        //Function type for function prologue
        template<typename ...Args>
        using prologue_func_t = decltype(prologue(std::declval<Args&>()...));
        //Function type for function epilogue
        template<typename ...Args>
        using epilogue_func_t = decltype(epilogue(std::declval<Args&>()...));

        /**************************************Helpers*********************************************************/

        //Checks if Archive has a prologue member function
        template<typename ToTest, typename ArchiveType>
        class has_prologue_member : public stdext::is_detected<prologue_member_t, ArchiveType, ToTest> {};
        //Checks if Archive has a epilogue member function
        template<typename ToTest, typename ArchiveType>
        class has_epilogue_member : public stdext::is_detected<epilogue_member_t, ArchiveType, ToTest> {};
        
        //Checks if Archive has a prologue function
        template<typename ToTest, typename ArchiveType>
        class has_prologue_func : public stdext::is_detected<prologue_member_t, ToTest, ArchiveType> {};
        //Checks if Archive has a epilogue function
        template<typename ToTest, typename ArchiveType>
        class has_epilogue_func : public stdext::is_detected<epilogue_member_t, ToTest, ArchiveType> {};

        template<typename ToTest, typename ArchiveType>
        class use_prologue_member : public has_prologue_member<ToTest, ArchiveType> {};
        template<typename ToTest, typename ArchiveType>
        class use_epilogue_member : public has_epilogue_member<ToTest, ArchiveType> {};

        template<typename ToTest, typename ArchiveType>
        class use_prologue_func : public std::conjunction<has_prologue_func<ToTest, ArchiveType>, std::negation<has_prologue_member<ToTest, ArchiveType>>> {};
        template<typename ToTest, typename ArchiveType>
        class use_epilogue_func : public std::conjunction<has_prologue_func<ToTest, ArchiveType>, std::negation<has_epilogue_member<ToTest, ArchiveType>>> {};

        template<typename ToTest, typename ArchiveType>
        class no_prologue : public std::negation<std::disjunction<has_prologue_func<ToTest, ArchiveType>, has_prologue_member<ToTest, ArchiveType>>> {};
        template<typename ToTest, typename ArchiveType>
        class no_epilogue : public std::negation<std::disjunction<has_prologue_func<ToTest, ArchiveType>, has_epilogue_member<ToTest, ArchiveType>>> {};

        //Checks if ToTest has a save function for itself
        template<typename ToTest, typename ArchiveType>
        class has_member_save : public stdext::is_detected<member_save_t, ToTest, ArchiveType> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool has_member_save_v = has_member_save<ToTest, ArchiveType>::value;

        //Checks if there is a save function for ToTest
        template<typename ToTest, typename ArchiveType>
        class has_func_save : public stdext::is_detected<func_save_t, ToTest, ArchiveType> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool has_func_save_v = has_func_save<ToTest, ArchiveType>::value;

        //Checks if the Archive has a save function for ToTest
        template<typename ArchiveType, typename ToTest>
        class archive_has_member_save : public stdext::is_detected<member_save_t, ArchiveType, ToTest> {};
        template<typename ArchiveType, typename ToTest>
        static constexpr bool archive_has_member_save_v = archive_has_member_save<ArchiveType, ToTest>::value;

        //Checks if the ToTest has a load function for itself
        template<typename ToTest, typename ArchiveType>
        class has_member_load : public stdext::is_detected<member_load_t, ToTest, ArchiveType> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool has_member_load_v = has_member_load<ToTest, ArchiveType>::value;
        //Checks if there is a load function for ToTest
        template<typename ToTest, typename ArchiveType>
        class has_func_load : public stdext::is_detected<func_load_t, ToTest, ArchiveType> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool has_func_load_v = has_func_load<ToTest, ArchiveType>::value;

        //Checks if the Archive has a load function for the type
        template<typename ArchiveType, typename ToTest>
        class archive_has_member_load : public stdext::is_detected<member_load_t, ArchiveType, ToTest> {};
        template<typename ArchiveType, typename ToTest>
        static constexpr bool archive_has_member_load_v = archive_has_member_load<ArchiveType, ToTest>::value;

        //Checks if ToTest has a serialization function for itself
        template<typename ToTest, typename ArchiveType>
        class has_member_serialize : public stdext::is_detected<member_serialize_t, ToTest, ArchiveType> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool has_member_serialize_v = has_member_serialize<ToTest, ArchiveType>::value;
        //Checks if there is a serialize function for ToTest
        template<typename ToTest, typename ArchiveType>
        class has_func_serialize : public stdext::is_detected<func_serialize_t, ToTest, ArchiveType> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool has_func_serialize_v = has_func_serialize<ToTest, ArchiveType>::value;

        //Counts the save functions
        template<typename ToTest, typename ArchiveType>
        static constexpr std::uint16_t count_save = archive_has_member_save_v<ArchiveType, ToTest> + has_member_save_v<ToTest, ArchiveType> + has_func_save_v<ToTest, ArchiveType> + has_member_serialize_v<ToTest, ArchiveType> + has_func_serialize_v<ToTest, ArchiveType>;

        //Counts the load functions
        template<typename ToTest, typename ArchiveType>
        static constexpr std::uint16_t count_load = archive_has_member_load_v<ArchiveType, ToTest> + has_member_load_v<ToTest, ArchiveType> + has_func_load_v<ToTest, ArchiveType> + has_member_serialize_v<ToTest, ArchiveType> + has_func_serialize_v<ToTest, ArchiveType>;

        /****************************************Helpers for selection logic*********************************************************/
        //Helper Booleans to determine which save function to use!
        template<typename ToTest, typename ArchiveType>
        class use_archive_member_save : public archive_has_member_save<ArchiveType, ToTest> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_archive_member_save_v = use_archive_member_save<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class use_member_save : public std::conjunction< std::negation<use_archive_member_save<ToTest, ArchiveType>>, has_member_save<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_member_save_v = use_member_save<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class use_func_save : public std::conjunction<std::negation<use_member_save<ToTest, ArchiveType>> , has_func_save<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_func_save_v = use_func_save<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class use_member_serialize_save : public std::conjunction<std::negation<use_func_save<ToTest, ArchiveType>>, has_member_serialize<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_member_serialize_save_v = use_member_serialize_save<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class use_func_serialize_save : public std::conjunction<std::negation<use_member_serialize_save<ToTest, ArchiveType>> , has_func_serialize<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_func_serialize_save_v = use_func_serialize_save<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class has_type_save : public std::disjunction<has_member_save<ToTest, ArchiveType>,
            has_func_save< ToTest, ArchiveType>, has_member_serialize<ToTest, ArchiveType>,
            has_func_serialize<ToTest, ArchiveType> > {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool has_type_save_v = has_type_save<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class no_type_save : public std::negation<has_type_save<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool no_type_save_v = no_type_save<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class uses_type_save : public std::conjunction< has_type_save<ToTest, ArchiveType>, std::negation<use_archive_member_save<ToTest, ArchiveType>>>{};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool uses_type_save_v = uses_type_save<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class not_any_save : public std::conjunction< std::negation<use_archive_member_save<ToTest, ArchiveType> >, no_type_save<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool not_any_save_v = not_any_save<ToTest, ArchiveType>::value;

        //Helper Booleans to determine which load function to use!
        template<typename ToTest, typename ArchiveType>
        class use_archive_member_load : public archive_has_member_load<ArchiveType, ToTest> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_archive_member_load_v = use_archive_member_load<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class use_member_load : public std::conjunction<std::negation<use_archive_member_load<ToTest, ArchiveType>>, has_member_load<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_member_load_v = use_member_load<ToTest, ArchiveType>::value;
        
        template<typename ToTest, typename ArchiveType>
        class use_func_load : public std::conjunction<std::negation<use_member_load<ToTest, ArchiveType>>, has_func_load<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_func_load_v = use_func_load<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class use_member_serialize_load : public std::conjunction<std::negation<use_func_load<ToTest, ArchiveType>>, has_member_serialize<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_member_serialize_load_v = use_member_serialize_load<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class use_func_serialize_load : public std::conjunction<std::negation<use_member_serialize_load<ToTest, ArchiveType>>, has_func_serialize<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool use_func_serialize_load_v = use_func_serialize_load<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class no_type_load : public std::negation<std::disjunction<has_func_serialize<ToTest, ArchiveType>,
                                                                   has_member_serialize<ToTest, ArchiveType>,
                                                                   has_func_load< ToTest, ArchiveType>,
                                                                   has_member_load<ToTest, ArchiveType>>> {};

        template<typename ToTest, typename ArchiveType>
        static constexpr bool no_type_load_v = no_type_load<ToTest, ArchiveType>::value;

        template<typename ToTest, typename ArchiveType>
        class not_any_load : public std::conjunction<std::negation<use_archive_member_load<ToTest, ArchiveType>>, no_type_load<ToTest, ArchiveType>> {};
        template<typename ToTest, typename ArchiveType>
        static constexpr bool not_any_load_v = not_any_load<ToTest, ArchiveType>::value;

    }
}

#endif  // INC_ArchiveHelper_H
// end of ArchiveHelper.h
///---------------------------------------------------------------------------------------------------
