cmake_minimum_required (VERSION 3.6 FATAL_ERROR)
project (BaseArchive CXX)

file(GLOB CPP_FILES *.cpp)
file(GLOB BASEARCHIVE_H_FILES *.h)

add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${Basic_Library_INCLUDE_DIRS})

# Required on Unix OS family to be able to be linked into shared libraries.
#set_target_properties(${PROJECT_NAME}
#                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
					  
#target_link_libraries(${PROJECT_NAME} Basic_Library)


#set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
#                                ${Basic_Library_INCLUDE_DIRS}
#    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

#install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
