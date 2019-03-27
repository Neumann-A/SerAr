cmake_minimum_required (VERSION 3.8)
project (ProgramOptions_Wrapper CXX)
#Wrapper for direkt serialization using boost program options. 

set(CPP_FILES ProgramOptions_Wrapper.cpp)
set(H_FILES ProgramOptions_Wrapper.h)
add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${Basic_Library_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${Archive_SOURCE_DIR} ${PROJECT_SOURCE_DIR})

# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)