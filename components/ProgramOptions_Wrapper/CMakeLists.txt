cmake_minimum_required (VERSION 3.8)
project (ProgramOptions_Wrapper CXX)
#Wrapper for direct serialization using boost program options. 

set(CPP_FILES ProgramOptions_Wrapper.cpp PO_Description_Wrapper.cpp)
set(H_FILES ProgramOptions_Wrapper.h PO_Description_Wrapper.h)

add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})
set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)

target_link_libraries(${PROJECT_NAME} PUBLIC Basic_Library)
target_link_libraries(${PROJECT_NAME} PUBLIC BaseArchive)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)