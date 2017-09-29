cmake_minimum_required (VERSION 3.6 FATAL_ERROR)
project (CerealWrappers)

find_package(Cereal)
#find_path(EIGEN_INCLUDE NAMES "signature_of_eigen3_matrix_library")


#file(GLOB CPP_FILES *.cpp)
set(H_FILES "BinaryArchiveWrapper.h;CerealAdapter.h;JSONArchiveWrapper.h;PortableBinaryArchiveWrapper.h;XMLArchiveWrapper.h")
add_library(${PROJECT_NAME} INTERFACE)
#set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)
# Required on Unix OS family to be able to be linked into shared libraries.
#set_target_properties(${PROJECT_NAME}
#                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_sources(${PROJECT_NAME} INTERFACE ${H_FILES})
target_include_directories(${PROJECT_NAME} INTERFACE ${Basic_Library_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} INTERFACE ${CEREAL_INCLUDE_DIRS})			  
target_link_libraries(${PROJECT_NAME} INTERFACE Basic_Library)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
								 ${CEREAL_INCLUDE_DIRS}
                                 ${Basic_Library_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

#install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
