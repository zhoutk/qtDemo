cmake_minimum_required(VERSION 3.10)

project(demo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        plugindemoplugin.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(demo
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(demo SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(demo
            ${PROJECT_SOURCES}
        )
    endif()
endif()

add_subdirectory(sub1)
add_subdirectory(sub2)

target_link_libraries(demo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
