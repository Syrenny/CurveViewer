cmake_minimum_required(VERSION 3.16)
project(CurveViewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Gui Widgets Core)

file(GLOB_RECURSE CPP_FILES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/utils/*.cpp
)

file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/utils/*.h
)

file(GLOB_RECURSE UI_FILES
        ${CMAKE_SOURCE_DIR}/src/*.ui
)

qt_add_executable(${PROJECT_NAME}
        ${CPP_FILES}
        ${HEADER_FILES}
        ${UI_FILES}
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/utils/
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Qt6::Gui
        Qt6::Widgets
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)