
cmake_minimum_required(VERSION 3.5)
project(FireRedDecompTool)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.h)
qt5_add_resources(RESOURCE_SOURCES resources/resources.qrc)
list(APPEND SOURCE_FILES ${RESOURCE_SOURCES})

add_executable(FireRedDecompTool WIN32 ${SOURCE_FILES} ${HEADER_FILES} src/appicon.rc)
target_include_directories(FireRedDecompTool PRIVATE include)
target_link_libraries(FireRedDecompTool Qt5::Widgets)
