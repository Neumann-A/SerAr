cmake_minimum_required (VERSION 3.19)

set(CMAKE_AUTOMOC ON)
set(SerAr_BUILD_TESTING ON)
# Until CMake fixes their AUTOMOC implementation. 
#set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#             PROPERTY Qt5Core_VERSION_MAJOR "5")
#set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#             PROPERTY Qt5Core_VERSION_MINOR "15")
project("QtUI.json")
cmake_print_variables(QT_VERSION_MAJOR QT_VERSION_MINOR Qt5Core_VERSION_MAJOR Qt5Core_VERSION_MINOR)
