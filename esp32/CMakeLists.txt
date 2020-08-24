# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(openrowingcomputer)

set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} ${CMAKE_SOURCE_DIR}/components/epd2in9b")
set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} ${CMAKE_SOURCE_DIR}/components/motion_driver_6.12")
