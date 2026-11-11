cmake_minimum_required(VERSION 3.16)
project(opencv_image_tool CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(image_tool src/main.cpp)
target_include_directories(image_tool PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(image_tool PRIVATE ${OpenCV_LIBS})
