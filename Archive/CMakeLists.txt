cmake_minimum_required (VERSION 3.6 FATAL_ERROR)
project(BaseArchive CXX)

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
target_include_directories(${PROJECT_NAME} INTERFACE "${PROJECT_SOURCE_DIR}/..")
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
