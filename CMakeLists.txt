cmake_minimum_required(VERSION 2.8)
project( imgProcess )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( imgProcess main.cpp IMG.cpp)
target_link_libraries( imgProcess ${OpenCV_LIBS} )
        