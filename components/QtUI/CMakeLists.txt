cmake_minimum_required (VERSION 3.19)

set(CMAKE_AUTOMOC ON)

## Automatic Qt version selection
if(NOT DEFINED SERAR_QT_VERSION_MAJOR OR SERAR_QT_VERSION_MAJOR STREQUAL "auto")
    find_package(Qt6Widgets QUIET)
    if(NOT Qt6Widgets_FOUND)
        find_package(Qt5Widgets REQUIRED COMPONENTS Widgets)
        if(Qt5Widgets_FOUND)
            set(SERAR_QT_VERSION_MAJOR 5)
        else()
            set(SERAR_QT_VERSION_MAJOR 6)
        endif()
    else()
        set(SERAR_QT_VERSION_MAJOR 6)
    endif()
endif()


project("QtUI.json")

