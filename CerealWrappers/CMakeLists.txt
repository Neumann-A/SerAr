cmake_minimum_required (VERSION 3.8 FATAL_ERROR)
project (CerealWrappers CXX)

find_package(Cereal)

FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

#file(GLOB CPP_FILES *.cpp)
set(H_FILES "BinaryArchiveWrapper.h;CerealAdapter.h;JSONArchiveWrapper.h;PortableBinaryArchiveWrapper.h;XMLArchiveWrapper.h")

PREPEND(${H_FILES} ${CMAKE_CURRENT_SOURCE_DIR} ${H_FILES_NEW})

add_library(${PROJECT_NAME} INTERFACE)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

#set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)
# Required on Unix OS family to be able to be linked into shared libraries.
#set_target_properties(${PROJECT_NAME}
#                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(${PROJECT_NAME} INTERFACE ${H_FILES_NEW})
#target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} INTERFACE ${Basic_Library_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} INTERFACE ${CEREAL_INCLUDE_DIRS})			  
target_link_libraries(${PROJECT_NAME} INTERFACE Basic_Library)

add_custom_target(${PROJECT_NAME}_Archive SOURCES ${H_FILES})
set_target_properties (${PROJECT_NAME}_Archive PROPERTIES FOLDER Archive)	

#install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
