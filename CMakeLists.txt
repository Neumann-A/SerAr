cmake_minimum_required (VERSION 3.4)
project (Archive)

set(INCLUDE_DIRS ${Basic_Library_INCLUDE_DIRS})
file(GLOB BASEARCHIVE_INCLUDES Archive/*.h)
set(ADDITONAL_SRCS "")
set(Archive_LIBS "")

if(${Archive_MATLAB})
add_subdirectory(MATLAB_Archive)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${MATLAB_Archive_INCLUDE_DIRS})
#set(ADDITONAL_SRCS ${ADDITONAL_SRCS} $<TARGET_OBJECTS:Archive_MATLAB>)
set(Archive_LIBS ${Archive_LIBS} MATLAB_Archive)
endif()

if(${Archive_ConfigFile})
add_subdirectory(ConfigFile_Archive)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${ConfigFile_Archive_INCLUDE_DIRS})
#set(ADDITONAL_SRCS ${ADDITONAL_SRCS} $<TARGET_OBJECTS:Archive_ConfigFile>)
set(Archive_LIBS ${Archive_LIBS} ConfigFile_Archive)
endif()

if(${Archive_HDF5})
add_subdirectory(HDF5_Archive)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${HDF5_Archive_INCLUDE_DIRS})
#set(ADDITONAL_SRCS ${ADDITONAL_SRCS} $<TARGET_OBJECTS:Archive_HDF5>)
set(Archive_LIBS ${Archive_LIBS} HDF5_Archive)
endif()

if(${Archive_CerealWrappers})
#header only!
add_subdirectory(CerealWrappers)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${CerealWrappers_INCLUDE_DIRS})
set(Archive_LIBS ${Archive_LIBS} CerealWrappers)
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
target_include_directories(${PROJECT_NAME} INTERFACE ${Basic_Library_INCLUDE_DIRS})		  
target_link_libraries(${PROJECT_NAME} INTERFACE Basic_Library ${Archive_LIBS})
message(STATUS "Archive Libs: ${Archive_LIBS}")

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
								 ${INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
