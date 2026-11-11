cmake_minimum_required(VERSION 3.8)
project(nav_bringup)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(nav2_common REQUIRED)
find_package(nav2_bringup REQUIRED)

# ---- 把 launch / 参数 / rviz / 行为树 / 地图全部装进 share/ ----
install(DIRECTORY
  src/nav_bringup/launch
  src/nav_bringup/config
  src/nav_bringup/rviz
  src/nav_bringup/behavior_trees
  src/nav_bringup/maps
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
