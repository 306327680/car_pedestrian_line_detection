cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ground_based_rgbd_people_detector)
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

add_executable (car_detction main.cpp)
target_link_libraries (car_detction ${OpenCV_LIBRARIES})
