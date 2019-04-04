cmake_minimum_required (VERSION 3.11)
project(Archive CXX)

#set(INCLUDE_DIRS ${Basic_Library_INCLUDE_DIRS})
#set(ADDITONAL_SRCS "")
#set(Archive_LIBS "")

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
                            Archive/TestIOArchive.h)

add_subdirectory(Archive)

set(Archive_LIBS)

if(${Archive_MATLAB})
 add_subdirectory(MATLAB_Archive)
 set(Archive_LIBS ${Archive_LIBS} MATLAB_Archive)
endif()

if(${Archive_ConfigFile})
 add_subdirectory(ConfigFile_Archive)
 set(Archive_LIBS ${Archive_LIBS} ConfigFile_Archive)
endif()

if(${Archive_HDF5})
 if(${Archive_MATLAB}) 
  set(HDF5_USE_STATIC_LIBRARIES ON)
 endif()
 add_subdirectory(HDF5_Archive)
 set(Archive_LIBS ${Archive_LIBS} HDF5_Archive)
endif()

if(${Archive_CerealWrappers})
 add_subdirectory(CerealWrappers)
 set(Archive_LIBS ${Archive_LIBS} CerealWrappers)
endif()

if(${Archive_ProgramOptions_Wrapper})
 add_subdirectory(ProgramOptions_Wrapper)
 set(Archive_LIBS ${Archive_LIBS} ProgramOptions_Wrapper)
endif()

# Bundle of all build Archives
add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE ${Archive_LIBS})


#  HACK: have the files showing in the IDE
#set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)
#add_custom_target(${PROJECT_NAME}_Base SOURCES ${BASEARCHIVE_INCLUDES})
#Dummy Target for Visual Studio Folders View
add_library(${PROJECT_NAME}_Base STATIC EXCLUDE_FROM_ALL ${BASEARCHIVE_INCLUDES})
#set_target_properties(${PROJECT_NAME}_Base PROPERTIES LINKER_LANGUAGE Cxx) 
set_target_properties (${PROJECT_NAME}_Base PROPERTIES FOLDER Archive)
target_compile_features(${PROJECT_NAME}_Base PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME}_Base PUBLIC BaseArchive)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
