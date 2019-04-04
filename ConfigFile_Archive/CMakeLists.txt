cmake_minimum_required (VERSION 3.4)
project (ConfigFile_Archive CXX)

set(CPP_FILES ConfigFile_Archive.cpp)
set(H_FILES ConfigFile_Archive.h)

add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)

#target_include_directories(${PROJECT_NAME} PUBLIC ${Basic_Library_INCLUDE_DIRS})
#target_include_directories(${PROJECT_NAME} PRIVATE ${Archive_SOURCE_DIR} ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC BaseArchive)
target_link_libraries(${PROJECT_NAME} PUBLIC Basic_Library)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
