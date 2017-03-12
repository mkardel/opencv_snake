cmake_minimum_required(VERSION 2.8)
project( OpenCV_snake )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( OpenCV 3.0.0 REQUIRED )

add_executable( snake snake.cpp )
target_link_libraries( snake ${OpenCV_LIBS} )
