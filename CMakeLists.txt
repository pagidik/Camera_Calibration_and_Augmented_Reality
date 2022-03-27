cmake_minimum_required(VERSION 2.8)
project(CV_PROJECT_4)

find_package(OpenCV REQUIRED)

find_package(aruco REQUIRED)
# find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(CV_PROJECT_4 aruco.cpp)

target_link_libraries(CV_PROJECT_4 ${OpenCV_LIBS})
target_link_libraries(CV_PROJECT_4 aruco)
# target_link_libraries(CV_PROJECT_4 ${OpenGL_LIBRARY})


