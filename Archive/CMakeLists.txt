cmake_minimum_required (VERSION 3.6 FATAL_ERROR)
project(BaseArchive VERSION 1.0.0 LANGUAGES CXX)
include(GNUInstallDirs)

set(H_FILES ArchiveHelper.h
            ArchiveVisitor.h
            BaseArchiveType.h
            InputArchive.h
            LoadConstructor.h
            NamedValue.h
            OutputArchive.h
            Serializeable.h
            TestIOArchive.h)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE Basic_Library)
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                                     $<INSTALL_INTERFACE:include/${PARENT_PROJECT_NAME}>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

install(FILES 
            ${H_FILES}
        DESTINATION 
            ${CMAKE_INSTALL_INCLUDEDIR}/${PARENT_PROJECT_NAME}/${PROJECT_NAME}
)