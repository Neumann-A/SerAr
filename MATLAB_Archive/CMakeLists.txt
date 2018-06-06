cmake_minimum_required (VERSION 3.4)
project (MATLAB_Archive CXX)

#Important: To find MATLAB correctly it has to be written as Matlab in FindPackage!
#(Do not ask me why!)
find_package(Matlab COMPONENTS MX_LIBRARY MAT_LIBRARY REQUIRED)

include_directories(${Basic_Library_INCLUDE_DIRS})
include_directories(${Archive_SOURCE_DIR})
include_directories(${Matlab_INCLUDE_DIRS})

if(NOT Matlab_FOUND)
  message(FATAL_ERROR "MATLAB not found!")
endif()

file(GLOB CPP_FILES *.cpp)
file(GLOB H_FILES *.h)
add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)
# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
					  
					  
set(Matlab_LIBS ${Matlab_MAT_LIBRARY} ${Matlab_MX_LIBRARY})

if(UNIX)
#I do not know why we need this extra dependency?
set(Matlab_LIBS ${Matlab_LIBS} ${Matlab_ROOT_DIR}/bin/glnxa64/libmwfl.so)
endif()

#link_libraries(${Matlab_ROOT_DIR}/bin/glnxa64)			  
target_link_libraries(${PROJECT_NAME} ${Matlab_LIBS} Basic_Library)


#For checking if Find_package really found Matlab
#message(STATUS "In Matlab Project: ${Matlab_MAT_LIBRARY} ${Matlab_MX_LIBRARY}" )

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
								 ${Matlab_INCLUDE_DIRS}
								 ${Archive_SOURCE_DIR}
                                 ${Basic_Library_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
