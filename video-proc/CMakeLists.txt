cmake_minimum_required(VERSION 3.16)
project(video_proc)

set(CMAKE_CXX_STANDARD 14)

#file(GLOB SOURCES
#    *.h
#    *.cpp
#)

add_executable(video_proc main.cpp ${SOURCES})

find_package(OpenCV REQUIRED)
include_directories(video_proc ${OpenCV_INCLUDE_DIRS})
target_link_libraries(video_proc ${OpenCV_LIBS})