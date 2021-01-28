#设置cmake版本号
cmake_minimum_required(VERSION 3.10)

project(plg1 VERSION 0.1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#打开全局moc
set(CMAKE_AUTOMOC ON)
#打开全局uic
set(CMAKE_AUTOUIC ON)
#打开全局rcc
# set(CMAKE_AUTORCC ON)

message("Looking for Qt...")
# Qt modules (https://doc.qt.io/qt-5/qtmodules.html) you're using in your application
find_package(Qt5 REQUIRED Widgets)
# find_package(Qt5 COMPONENTS Widgets REQUIRED)

if (${Qt5_FOUND})
    message("Found Qt " ${Qt5_VERSION})
else()
    message("Couldn't find Qt")
endif()

#查找需要的Qt库文件，最好每一个库都要写，Qt也会根据依赖关系自动添加
find_package(Qt5Widgets)

FILE(GLOB SRC_FILES "*.cpp")
FILE(GLOB HEAD_FILES "*.h")
FILE(GLOB UI_FILES "*.ui") 

source_group("Head Files" FILES ${HEAD_FILES} )
source_group("Form Files" FILES ${UI_FILES} )
source_group("Src Files" FILES ${SRC_FILES} )

add_definitions(-DNAVIGATION_LIBRARY)

#创建工程文件
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/plugins)
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_FILES} ${HEAD_FILES} ${UI_FILES} )

#添加Qt5依赖项
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

