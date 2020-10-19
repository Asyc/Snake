#ifndef SNAKE_SNAKE_HPP
#define SNAKE_SNAKE_HPP

#include <cstdint>
#include <vector>

#include <glm/vec2.hpp>

class Snake {
public:
    enum class Direction {
        NONE, UP, DOWN, LEFT, RIGHT
    };

    Snake(size_t width, size_t height);

    void tick();
    void switchDirection(Direction direction);
    void reset();

    [[nodiscard]] bool doesCollide() const;

    [[nodiscard]] Direction getDirection() const;
    [[nodiscard]] const glm::ivec2& getHead() const;
    [[nodiscard]] const std::vector<glm::ivec2>& getBody() const;
private:
    glm::ivec2 m_Head, m_Target;
    std::vector<glm::ivec2> m_Body;
    Direction m_Direction;

    size_t m_Width, m_Height;
};


#endif //SNAKE_SNAKE_HPP
