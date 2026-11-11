cmake_minimum_required(VERSION 3.8)
project(inspection_robot)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(inspection_interfaces REQUIRED)

add_executable(status_publisher src/status_publisher.cpp)
ament_target_dependencies(status_publisher rclcpp inspection_interfaces)

add_executable(status_monitor src/status_monitor.cpp)
ament_target_dependencies(status_monitor rclcpp inspection_interfaces)

install(TARGETS status_publisher status_monitor
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
