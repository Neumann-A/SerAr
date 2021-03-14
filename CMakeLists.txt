cmake_minimum_required(VERSION 3.19)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMakeJSON_FOUND)
    find_package(CMakeJSON REQUIRED)
endif()

project("SerAr.json")

feature_summary(WHAT ALL)