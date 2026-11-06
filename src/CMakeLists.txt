cmake_minimum_required(VERSION 3.16)
project(robot CXX)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(a_lib STATIC
    src/forw.cpp
    src/wave.cpp
    src/rotate.cpp
    src/control.cpp
)

target_include_directories(a_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(a_exe src/main.cpp)
target_link_libraries(a_exe PRIVATE a_lib)
