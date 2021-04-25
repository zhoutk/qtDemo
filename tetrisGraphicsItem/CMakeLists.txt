cmake_minimum_required(VERSION 3.10)

project(tetris LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") 
	add_compile_options(/wd26451 /wd26812)
	# set(EnclibName enccore)
elseif(UNIX)
	# set(EnclibName libenccore.so)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

message("Looking for Qt...")
find_package(Qt5 REQUIRED Widgets)
if (${Qt5_FOUND})
    message("Found Qt " ${Qt5_VERSION})
else()
    message("Couldn't find Qt")
endif()

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)
#find_package(Qt5Network)

FILE(GLOB_RECURSE SRC_FILES "./src/*.cpp")
FILE(GLOB_RECURSE HEAD_FILES "./src/*.h")
FILE(GLOB_RECURSE UI_FILES "./src/*.ui") 

#qt5_add_resources(QRC_FTP_FILES "./src/ftp.qrc")

source_group("Head Files" FILES ${HEAD_FILES} )
source_group("Form Files" FILES ${UI_FILES} )
# source_group("Src Files" FILES ${SRC_FILES} )

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEAD_FILES} ${UI_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui)
