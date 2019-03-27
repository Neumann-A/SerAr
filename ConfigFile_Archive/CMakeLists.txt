cmake_minimum_required (VERSION 3.4)
project (ConfigFile_Archive CXX)

include_directories(${Basic_Library_INCLUDE_DIRS})
include_directories(${Archive_SOURCE_DIR})

set(CPP_FILES ConfigFile_Archive.cpp)
set(H_FILES ConfigFile_Archive.h)
add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)

target_include_directories(${PROJECT_NAME} PUBLIC ${Basic_Library_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${Archive_SOURCE_DIR} ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC BaseArchive)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
