cmake_minimum_required(VERSION 3.10)

project(plg2 VERSION 0.1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED Widgets)
find_package(Qt5Widgets)

FILE(GLOB SRC_FILES "*.cpp")
FILE(GLOB HEAD_FILES "*.h")
FILE(GLOB UI_FILES "*.ui")

source_group("Head Files" FILES ${HEAD_FILES} )
source_group("Form Files" FILES ${UI_FILES} )
source_group("Src Files" FILES ${SRC_FILES} )

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/plugins)
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_FILES} ${HEAD_FILES} ${UI_FILES} )

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
