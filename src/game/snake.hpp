#ifndef SNAKE_SNAKE_HPP
#define SNAKE_SNAKE_HPP

#include <cstdint>
#include <list>
#include <unordered_set>
#include <queue>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
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
    void setTarget();

    [[nodiscard]] bool doesCollide() const;

    [[nodiscard]] Direction getDirection() const;
    [[nodiscard]] const glm::ivec2& getTarget() const;
    [[nodiscard]] const glm::ivec2& getHead() const;
    [[nodiscard]] const std::list<glm::ivec2>& getBody() const;
private:
    glm::ivec2 m_Head, m_Target;
    std::list<glm::ivec2> m_Body;
    std::unordered_set<glm::ivec2> m_BodySet;
    std::queue<Direction> m_DirectionQueue;

    Direction m_Direction;

    size_t m_Width, m_Height;
};


#endif //SNAKE_SNAKE_HPP
