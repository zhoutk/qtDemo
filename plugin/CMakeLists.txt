cmake_minimum_required(VERSION 3.10)

project(demo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("Looking for Qt...")
find_package(Qt5 REQUIRED Widgets)
find_package(Qt5Widgets)
if (${Qt5_FOUND})
    message("Found Qt " ${Qt5_VERSION})
else()
    message("Couldn't find Qt")
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        plugindemoplugin.h
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} )

add_subdirectory(sub1)
add_subdirectory(sub2)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
