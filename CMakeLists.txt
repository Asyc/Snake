cmake_minimum_required(VERSION 3.1)

project(Snake)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES src/main.cpp src/window/window.cpp src/window/window.hpp src/context/render_context.cpp src/context/render_context.hpp src/window/swapchain.cpp src/window/swapchain.hpp)

add_subdirectory(libs/glfw)
add_executable(Snake ${SOURCES})

target_include_directories(Snake PRIVATE src)
target_link_libraries(Snake PUBLIC Vulkan::Vulkan glfw)