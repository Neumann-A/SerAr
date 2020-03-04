cmake_minimum_required (VERSION 3.16)
project(Archive VERSION 1.0.0 LANGUAGES CXX)
set(PARENT_PROJECT_NAME ${PROJECT_NAME} CACHE INTERNAL "" FORCE)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/ArchiveOptions.cmake)

set(BASEARCHIVE_INCLUDES    Archive/dummy.cpp
                            Archive/ArchiveHelper.h
                            Archive/ArchiveVisitor.h
                            Archive/BaseArchiveType.h
                            Archive/InputArchive.h
                            Archive/LoadConstructor.h
                            Archive/NamedValue.h
                            Archive/NamedValueWithDesc.h
                            Archive/OutputArchive.h
                            Archive/Serializeable.h
                            Archive/TestIOArchive.h
                            Archive/AllArchiveIncludes.h)

add_subdirectory(Archive)

set(Archive_LIBS)

if(${Archive_MATLAB})
 add_subdirectory(MATLAB_Archive)
 list(APPEND Archive_LIBS MATLAB_Archive)
 list(APPEND Archive_DEFINITIONS ARCHIVE_HAS_MATLAB)
endif()

if(${Archive_ConfigFile})
 add_subdirectory(ConfigFile_Archive)
 list(APPEND Archive_LIBS ConfigFile_Archive)
 list(APPEND Archive_DEFINITIONS ARCHIVE_HAS_ConfigFile)
endif()

if(${Archive_HDF5})
 if(${Archive_MATLAB}) 
  if(NOT Archive_USE_STATIC_HDF5)
   message(STATUS "HDF5 and MATLAB Archives will be used! Can only link static HDF5!")
   set(Archive_USE_STATIC_HDF5 ON)
  endif()
 endif()
 add_subdirectory(HDF5_Archive)
 list(APPEND Archive_LIBS HDF5_Archive)
 list(APPEND Archive_DEFINITIONS ARCHIVE_HAS_HDF5)
endif()

if(${Archive_CerealWrappers})
 add_subdirectory(CerealWrappers)
 list(APPEND Archive_LIBS CerealWrappers)
 list(APPEND Archive_DEFINITIONS ARCHIVE_HAS_CEREAL)
endif()

if(${Archive_ProgramOptions_Wrapper})
 add_subdirectory(ProgramOptions_Wrapper)
 list(APPEND Archive_LIBS ProgramOptions_Wrapper)
 list(APPEND Archive_DEFINITIONS ARCHIVE_HAS_BOOST_PO)
 
endif()

# Bundle of all build Archives
add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE ${Archive_LIBS})
target_compile_definitions(${PROJECT_NAME} INTERFACE ${Archive_DEFINITIONS})
target_include_directories(${PROJECT_NAME} INTERFACE   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                                       $<INSTALL_INTERFACE:include/${PROJECT_NAME}> )

#Dummy Target for Visual Studio Folders View
add_library(${PROJECT_NAME}_Base STATIC EXCLUDE_FROM_ALL ${BASEARCHIVE_INCLUDES})
set_target_properties (${PROJECT_NAME}_Base PROPERTIES FOLDER Archive)
target_compile_features(${PROJECT_NAME}_Base PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME}_Base PUBLIC BaseArchive)

install(TARGETS ${PROJECT_NAME} BaseArchive ${Archive_LIBS}
        EXPORT ${PROJECT_NAME}-exports
        LIBRARY 
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE 
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT ${PROJECT_NAME}-exports
        FILE
            ${PROJECT_NAME}Targets.cmake
        NAMESPACE
            ${PROJECT_NAME}::
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    ${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION 
    ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
install(FILES 
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION 
            ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} 
)