cmake_minimum_required (VERSION 3.11)
project (HDF5_Archive CXX)

find_package(HDF5 CONFIG COMPONENTS C REQUIRED)

if(NOT HDF5_FOUND)
  message(FATAL_ERROR "HDF5 not found!")
endif()

file(GLOB CPP_FILES *.cpp)
file(GLOB H_FILES *.h)
add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)
# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME}
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${PROJECT_NAME} PUBLIC ${Basic_Library_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${Archive_SOURCE_DIR} ${PROJECT_SOURCE_DIR})
					  
target_link_libraries(${PROJECT_NAME} PRIVATE Basic_Library)

if(HDF5_USE_STATIC_LIBRARIES)
	target_link_libraries(${PROJECT_NAME} PRIVATE hdf5::hdf5-static)
else()
	target_link_libraries(${PROJECT_NAME} PRIVATE hdf5::hdf5-shared)
endif()
	
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
