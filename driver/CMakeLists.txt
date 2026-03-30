cmake_minimum_required(VERSION 3.16)
project(RReaperTool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) 

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

add_executable(RReaperTool main.cpp mainwindow.cpp mainwindow.h
    mainwindow.ui)
target_link_libraries(RReaperTool Qt6::Widgets Qt6::SerialPort)
