cmake_minimum_required (VERSION 3.6 FATAL_ERROR)
project (BaseArchive CXX)

file(GLOB CPP_FILES *.cpp)
file(GLOB BASEARCHIVE_H_FILES *.h)

add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${Basic_Library_INCLUDE_DIRS})

#install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
