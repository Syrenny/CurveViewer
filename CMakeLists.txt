cmake_minimum_required(VERSION 3.16)
project(CurveViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Gui Widgets)

add_executable(CurveViewer
    main.cpp
    MainWindow.cpp
    MainWindow.h
    CurveWidget.cpp
    CurveWidget.h
    utils/CurveLoader.cpp
    utils/CurveLoader.h
)

target_link_libraries(CurveViewer PRIVATE Qt6::Gui Qt6::Widgets)
