cmake_minimum_required(VERSION 3.16)
project(eigen_task CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 REQUIRED)

add_executable(eigen_task src/main.cpp)
target_link_libraries(eigen_task PRIVATE Eigen3::Eigen)
