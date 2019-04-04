cmake_minimum_required (VERSION 3.11)
project (HDF5_Archive CXX)

find_package(HDF5 CONFIG COMPONENTS C REQUIRED)

if(NOT HDF5_FOUND)
  message(FATAL_ERROR "HDF5 not found!")
endif()

set(CPP_FILES   HDF5_Archive.cpp
                HDF5_Type_Selector.cpp
                HDF5_Wrappers.cpp)

set(H_FILES     HDF5_Archive.h
                HDF5_FwdDecl.h
                HDF5_Type_Selector.h
                HDF5_Wrappers.h)

add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)

target_link_libraries(${PROJECT_NAME} PUBLIC Basic_Library)
target_link_libraries(${PROJECT_NAME} PUBLIC BaseArchive)

if(HDF5_USE_STATIC_LIBRARIES)
    target_link_libraries(${PROJECT_NAME} PRIVATE hdf5::hdf5-static)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE hdf5::hdf5-shared)
endif()
    
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
