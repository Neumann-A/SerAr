cmake_minimum_required(VERSION 3.17)

set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE CACHE STRING "CMake message log level (default: VERBOSE)")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(CMakeCS REQUIRED)

include(options.cmake)

project("Project.SerAr.cmakecs")

feature_summary(WHAT ALL)