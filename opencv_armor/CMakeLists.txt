cmake_minimum_required(VERSION 3.16)
project(opencv_armor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(armor src/main.cpp)
target_include_directories(armor PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(armor PRIVATE ${OpenCV_LIBS})
