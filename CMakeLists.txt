cmake_minimum_required(VERSION 4.0)
project(vhsfilter2)

set(CMAKE_CXX_STANDARD 20)
find_package(OpenCV REQUIRED)
add_executable(vhsfilter2 main.cpp)
target_link_libraries(vhsfilter2  opencv_highgui opencv_core)
