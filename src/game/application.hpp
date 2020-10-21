#ifndef SNAKE_APPLICATION_HPP
#define SNAKE_APPLICATION_HPP

#include <chrono>

#include <glm/mat4x4.hpp>

#include "context/render_context.hpp"
#include "game/snake.hpp"
#include "render/buffer.hpp"
#include "render/pipeline/pipeline.hpp"
#include "window/window.hpp"

class Application {
public:
    constexpr static size_t HORIZONTAL_TILES = 50, VERTICAL_TILES = 50;

    void enterGameLoop();
    static Application* getApplication();
private:
    Application();
    Window m_Window;
    RenderContext m_Context;

    Pipeline m_Pipeline;

    glm::mat4 m_ProjectionMatrix;

    Snake m_Snake;
    VertexBuffer m_SnakeBuffer;

    bool m_Active;
    std::chrono::high_resolution_clock::time_point m_LastTick;

    std::queue<Snake::Direction> m_KeyQueue;

    void update();
    void render(const CommandBuffer& target);
    void createProjection();

    friend void run();

    static Application* s_Application;
};


#endif //SNAKE_APPLICATION_HPP
