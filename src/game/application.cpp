#include "application.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "render/pipeline/pipeline_builder.hpp"

constexpr size_t HORIZONTAL_TILES = 50, VERTICAL_TILES = 50;

struct Vertex {
    glm::ivec2 pos;
};

Application* Application::s_Application = nullptr;

Application::Application() : m_Window(1920, 1920, "Snake | FPS: Not Calculated"), m_Context(m_Window, "Snake", {0, 1, 0}),
                             m_Pipeline([&]() {
            PipelineBuilder<1, 1, 0, 2, 0> builder(m_Context, ShaderResource("standard"));
            builder.setBinding(0, sizeof(Vertex))
                    .setAttribute(0, 0, vk::Format::eR32G32Sint, 0)
                    .setPushConstantRange(vk::ShaderStageFlagBits::eFragment, 0, sizeof(glm::vec4))
                    .setPushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(glm::vec4), sizeof(glm::mat4) * 2);
            return builder.build();
        }()), m_Snake(HORIZONTAL_TILES, VERTICAL_TILES), m_SnakeBuffer([&](){
            VertexBuffer buffer(m_Context, sizeof(Vertex) * 4, 6);
            std::array<Vertex, 4> vertices = {
                    Vertex{glm::ivec2(0, 1)},
                    Vertex{glm::ivec2(0, 0)},
                    Vertex{glm::ivec2(1, 0)},
                    Vertex{glm::ivec2(1, 1)}
            };
            std::array<uint32_t, 6> indices = {0, 1, 2, 0, 3, 2};

            memcpy(buffer.map(), vertices.data(), sizeof(Vertex) * vertices.size());
            buffer.unmap();

            memcpy(buffer.mapIndexBuffer(), indices.data(), sizeof(uint32_t) * indices.size());
            buffer.unmapIndexBuffer();

            return std::move(buffer);
        }()), m_Active(false) {
    createProjection();
    s_Application = this;
    m_Window.setKeyCallback([](int key, int scancode, int action, int){
        if (action != GLFW_PRESS) return;
        auto& snake = Application::s_Application->m_Snake;
        auto direction = snake.getDirection();
        switch (key) {
            case GLFW_KEY_UP:
                if (snake.getDirection() != Snake::Direction::DOWN) snake.switchDirection(Snake::Direction::UP);
            case GLFW_KEY_DOWN:
                if (snake.getDirection() != Snake::Direction::UP) snake.switchDirection(Snake::Direction::DOWN);
                break;
            case GLFW_KEY_LEFT:
                if (snake.getDirection() != Snake::Direction::RIGHT) snake.switchDirection(Snake::Direction::LEFT);
                break;
            case GLFW_KEY_RIGHT:
                if (snake.getDirection() != Snake::Direction::LEFT) snake.switchDirection(Snake::Direction::RIGHT);
                break;
            default:
                break;
        }
        if (direction != snake.getDirection()) {
            snake.tick();
            Application::s_Application->m_LastTick = std::chrono::high_resolution_clock::now();
            Application::s_Application->m_Active = true;
        }
    });
}

void Application::enterGameLoop() {
    double total = 0.0f;
    size_t frames = 0;

    auto last = std::chrono::high_resolution_clock::now();
    m_Snake.switchDirection(Snake::Direction::UP);
    while (!m_Window.shouldClose()) {
        Window::pollEvents();
        update();

        auto start = std::chrono::high_resolution_clock::now();

        auto& image = m_Context.getSwapchain().nextImage();

        render(CommandBuffer{m_Context, *image.m_CommandBuffer});

        m_Context.getSwapchain().presentImage();

        frames++;
        auto now = std::chrono::high_resolution_clock::now();

        total += std::chrono::duration_cast<std::chrono::nanoseconds>(now - start).count() / 1000000.0f;
        frames++;
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last).count() >= 1) {
            last = now;
            std::cout << "Average Frame Time(MS): " << std::fixed << std::setprecision(3)
                      << (total / static_cast<float>(frames)) << '\n';
            m_Window.setTitle("Snake | FPS: " + std::to_string(frames));
            frames = 0;
            total = 0;
        }
    }

    m_Context.getDevice().waitIdle();
}

void Application::update() {
    if (!m_Active) return;
    auto now = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastTick).count() >= 80) {
        m_Snake.tick();
        m_LastTick = now;
    }
    if (m_Snake.doesCollide()) {
        m_Active = false;
        m_Snake.reset();
    }
}

void Application::render(const CommandBuffer& buffer) {
    buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline.getPipeline());
    buffer.setViewportScissor();

    glm::vec4 color(0.0f, 1.0f, 0.0f, 1.0f);
    buffer->pushConstants(m_Pipeline.getLayout(), vk::ShaderStageFlagBits::eFragment, 0, sizeof(glm::vec4), glm::value_ptr(color));
    buffer->pushConstants(m_Pipeline.getLayout(), vk::ShaderStageFlagBits::eVertex, sizeof(glm::vec4), sizeof(glm::mat4), glm::value_ptr(m_ProjectionMatrix));
    auto renderBody = [&](const glm::ivec2& pos) {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0));
        buffer->pushConstants(m_Pipeline.getLayout(), vk::ShaderStageFlagBits::eVertex, sizeof(glm::vec4) + sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(modelMatrix));


        buffer.bindVertexBuffer(m_SnakeBuffer);
        buffer.drawVertexBuffer(m_SnakeBuffer);
    };

    renderBody(m_Snake.getHead());
    std::for_each(m_Snake.getBody().begin(), m_Snake.getBody().end(), [&renderBody](const glm::ivec2& pos){renderBody(pos);});
}

void Application::createProjection() {
    auto size = m_Window.getSize();
    float aspect = static_cast<float>(size.first) / static_cast<float>(size.second);
    m_ProjectionMatrix = glm::ortho(aspect * static_cast<float>(VERTICAL_TILES) / -2,
                                      aspect * static_cast<float>(HORIZONTAL_TILES) / 2,
                                      static_cast<float>(VERTICAL_TILES) / 2, static_cast<float>(VERTICAL_TILES) / -2);
}

Application* Application::getApplication() {
    return s_Application;
}
