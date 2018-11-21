cmake_minimum_required (VERSION 3.11)
project(Archive CXX)

set(INCLUDE_DIRS ${Basic_Library_INCLUDE_DIRS})
file(GLOB BASEARCHIVE_INCLUDES Archive/*.h)
set(ADDITONAL_SRCS "")
set(Archive_LIBS "")

if(${Archive_MATLAB})
add_subdirectory(MATLAB_Archive)
#set(INCLUDE_DIRS ${INCLUDE_DIRS} ${MATLAB_Archive_INCLUDE_DIRS})
#set(ADDITONAL_SRCS ${ADDITONAL_SRCS} $<TARGET_OBJECTS:Archive_MATLAB>)
set(Archive_LIBS ${Archive_LIBS} MATLAB_Archive)
endif()

if(${Archive_ConfigFile})
add_subdirectory(ConfigFile_Archive)
#set(INCLUDE_DIRS ${INCLUDE_DIRS} ${ConfigFile_Archive_INCLUDE_DIRS})
#set(ADDITONAL_SRCS ${ADDITONAL_SRCS} $<TARGET_OBJECTS:Archive_ConfigFile>)
set(Archive_LIBS ${Archive_LIBS} ConfigFile_Archive)
endif()

if(${Archive_HDF5})
 if(${Archive_MATLAB}) 
 # If MATLAB archive is allowed we can only statically link our HDF5 archive because of possible version mismatches between our HDF5 versions and MATLABs
  set(HDF5_USE_STATIC_LIBRARIES ON)
 endif()
 add_subdirectory(HDF5_Archive)
 set(INCLUDE_DIRS ${INCLUDE_DIRS} ${HDF5_Archive_INCLUDE_DIRS})
#set(ADDITONAL_SRCS ${ADDITONAL_SRCS} $<TARGET_OBJECTS:Archive_HDF5>)
set(Archive_LIBS ${Archive_LIBS} HDF5_Archive)
endif()

if(${Archive_CerealWrappers})
#header only!
add_subdirectory(CerealWrappers)
#set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CerealWrappers_INCLUDE_DIRS})
set(Archive_LIBS ${Archive_LIBS} CerealWrappers)
endif()

if(${Archive_ProgramOptions_Wrapper})
#header only!
add_subdirectory(ProgramOptions_Wrapper)
#set(INCLUDE_DIRS ${INCLUDE_DIRS} ${ProgramOptions_Wrapper_INCLUDE_DIRS})
set(Archive_LIBS ${Archive_LIBS} ProgramOptions_Wrapper)
endif()

file(GLOB CPP_FILES Archive/*.cpp)

#add_library(${PROJECT_NAME} STATIC ${BASEARCHIVE_INCLUDES})
#set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ${INCLUDE_DIRS})
#  HACK: have the files showing in the IDE
#set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)
add_custom_target(${PROJECT_NAME}_Base SOURCES ${BASEARCHIVE_INCLUDES})

set_target_properties (${PROJECT_NAME}_Base PROPERTIES FOLDER Archive)	

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_include_directories(${PROJECT_NAME} INTERFACE ${Basic_Library_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} INTERFACE Basic_Library ${Archive_LIBS})

message(STATUS "Archive Libs: ${Archive_LIBS}")

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
