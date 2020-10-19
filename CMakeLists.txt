cmake_minimum_required(VERSION 3.1)

project(Snake)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES
        src/main.cpp
        src/window/window.cpp src/window/window.hpp
        src/context/render_context.cpp src/context/render_context.hpp
        src/window/swapchain.cpp src/window/swapchain.hpp
        src/render/pipeline/pipeline.cpp src/render/pipeline/pipeline.hpp
        src/render/pipeline/pipeline_builder.cpp src/render/pipeline/pipeline_builder.hpp src/render/command.cpp src/render/command.hpp src/render/buffer.cpp src/render/buffer.hpp src/game/snake.cpp src/game/snake.hpp src/game/application.cpp src/game/application.hpp)

add_subdirectory(libs/glfw)

# Compiling GLSL -> SPIR-V shaders

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "assets/shaders/glsl/*.vert"
        "assets/shaders/glsl/*.frag"
        )

foreach (FILE ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${FILE} NAME)
    set(TARGET "${FILE_NAME}.spv")

    add_custom_command(
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders
            OUTPUT ${TARGET}
            COMMAND glslangvalidator -V glsl/${FILE_NAME} -o ${TARGET}
            DEPENDS ${FILE}
    )
    list(APPEND BINARY_SHADERS ${TARGET})
endforeach ()

add_custom_target(Shaders DEPENDS ${BINARY_SHADERS})
add_executable(Snake ${SOURCES})

add_dependencies(Snake Shaders)
target_include_directories(Snake PRIVATE src libs/glm)
target_link_libraries(Snake PUBLIC Vulkan::Vulkan glfw)