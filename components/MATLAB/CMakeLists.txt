cmake_minimum_required (VERSION 3.17)

project("MATLAB.json")

# cmake_minimum_required (VERSION 3.4)
# project (MATLAB_Archive CXX)

# #Important: To find MATLAB correctly it has to be written as Matlab in FindPackage!
# #(Do not ask me why!)
# find_package(Matlab COMPONENTS MX_LIBRARY MAT_LIBRARY REQUIRED)

# if(NOT Matlab_FOUND)
#   message(FATAL_ERROR "MATLAB not found!")
# endif()

# set(CPP_FILES Matlab_Archive.cpp)
# set(H_FILES Matlab_Archive.h)

# add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${H_FILES})

# target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
# target_compile_definitions(${PROJECT_NAME} PUBLIC "UINT64_T=unsigned long long")
# target_compile_definitions(${PROJECT_NAME} PUBLIC "INT64_T=long long")

# set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER Archive)

# target_include_directories(${PROJECT_NAME} PUBLIC ${Basic_Library_INCLUDE_DIRS} ${Matlab_INCLUDE_DIRS})

# set(Matlab_LIBS ${Matlab_MAT_LIBRARY} ${Matlab_MX_LIBRARY})

# if(UNIX)
# #I do not know why we need this extra dependency?
# set(Matlab_LIBS ${Matlab_LIBS} ${Matlab_ROOT_DIR}/bin/glnxa64/libmwfl.so)
# endif()
 
# target_link_libraries(${PROJECT_NAME} PRIVATE ${Matlab_LIBS} Basic_Library)
# target_link_libraries(${PROJECT_NAME} PUBLIC BaseArchive)

# #Add correct Matlab PATH to PATH
# set($ENV{PATH} "${Matlab_ROOT_DIR}/bin/win64;$ENV{PATH}")

# install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/lib)
# install(FILES ${Matlab_LIBS} DESTINATION ${CMAKE_BINARY_DIR}/lib)