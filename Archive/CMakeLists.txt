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
#target_sources(${PROJECT_NAME} INTERFACE ${H_FILES})
target_include_directories(${PROJECT_NAME} INTERFACE ${Basic_Library_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
