cmake_minimum_required (VERSION 3.6 FATAL_ERROR)
project (CerealWrappers)

find_package(cereal)
#find_path(EIGEN_INCLUDE NAMES "signature_of_eigen3_matrix_library")
include_directories(${Basic_Library_INCLUDE_DIRS})

file(GLOB CPP_FILES *.cpp)
file(GLOB H_FILES *.h)
add_library(${PROJECT_NAME} INTERFACE ${CPP_FILES} ${H_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)
# Required on Unix OS family to be able to be linked into shared libraries.
#set_target_properties(${PROJECT_NAME}
#                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
					  
#target_link_libraries(${PROJECT_NAME} Basic_Library)


set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
								 ${cereal_INCLUDE_DIRS}
                                 ${Basic_Library_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

#install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
