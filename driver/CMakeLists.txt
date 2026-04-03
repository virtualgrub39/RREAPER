# Copyright (C) 2026 Mikołaj Trafisz
# 
# This file is part of RREAPER.
# RREAPER is free software: you may redistribute it and/or modify it
# under the terms of the GNU General Public License, version 3, or any later version.
# 
# RREAPER comes with no warranty; see the GNU GPL for details.
# 

cmake_minimum_required(VERSION 3.16)
project(RReaperTool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) 

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

add_executable(RReaperTool main.cpp mainwindow.cpp mainwindow.h
    mainwindow.ui)
target_link_libraries(RReaperTool Qt6::Widgets Qt6::SerialPort)
