cmake_minimum_required (VERSION 3.17)

OPTION(SerAr_WITH_STATIC_HDF5 "Link HDF5 statically" ON)
add_feature_info(HDF5_STATIC SerAr_WITH_STATIC_HDF5 "Link HDF5 statically. Required if MATLAB support is also build.")
if(SerAr_WITH_STATIC_HDF5)
 set(SerAr_HDF5_USE_STATIC_LIBRARIES ON)
endif()

if(SerAr_WITH_MATLAB AND NOT SerAr_WITH_STATIC_HDF5)
    message(FATAL_ERROR "[SerAr]: Building HDF5 and MATLAB components requires static HDF5 linkage due to version conflicts with the version in MATLAB.")
endif()

project("HDF5.cmakecs")
if(SerAr_WITH_HDF5 AND SerAr_WITH_STATIC_HDF5 AND NOT TARGET hdf5::hdf5-static)
    message(FATAL_ERROR "Requested static hdf5 linkage but target hdf5::hdf5-static is not available")
endif()

# cmake_minimum_required (VERSION 3.11)
# project (HDF5_Archive CXX)

# find_package(HDF5 CONFIG COMPONENTS C REQUIRED)

# if(NOT HDF5_FOUND)
#   message(FATAL_ERROR "HDF5 not found!")
# endif()

# set(CPP_FILES   HDF5_Archive.cpp
#                 HDF5_Type_Selector.cpp
#                 HDF5_Wrappers.cpp)

# set(H_FILES     HDF5_Archive.h
#                 HDF5_FwdDecl.h
#                 HDF5_Type_Selector.h
#                 HDF5_Wrappers.h)

# add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})

# target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)

# target_link_libraries(${PROJECT_NAME} PUBLIC Basic_Library)
# target_link_libraries(${PROJECT_NAME} PUBLIC BaseArchive)

# if(Archive_USE_STATIC_HDF5)
#     target_link_libraries(${PROJECT_NAME} PRIVATE hdf5::hdf5-static)
# else()
#     target_link_libraries(${PROJECT_NAME} PRIVATE hdf5::hdf5-shared)
# endif()
    
# install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
