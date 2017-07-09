cmake_minimum_required (VERSION 3.4)
project (HDF5_Archive)

find_package(HDF5 COMPONENTS CXX HL REQUIRED)
#set(HDF5_USE_STATIC_LIBRARIES ON)
#find_package(HDF5 REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})
include_directories(${Basic_Library_INCLUDE_DIRS})
include_directories(${Archive_SOURCE_DIR})

if(NOT HDF5_FOUND)
  message(FATAL_ERROR "HDF5 not found!")
endif()

file(GLOB CPP_FILES *.cpp)
file(GLOB H_FILES *.h)
add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)
# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
					  
target_link_libraries(${PROJECT_NAME} Basic_Library ${HDF5_LIBRARIES})
#message(STATUS "HDF5 libraries:" ${HDF5_LIBRARIES})
#message(STATUS "HDF5 libraries:" ${HDF5_CXX_LIBRARY})
#message(STATUS "HDF5 libraries:" ${HDF5_CXX_HL_LIBRARY})

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
								 ${HDF5_INCLUDE_DIRS}
                                 ${Basic_Library_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
