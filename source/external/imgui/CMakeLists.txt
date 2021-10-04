cmake_minimum_required(VERSION 3.19.1)
project(imgui)

file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${IMGUI_SOURCES})