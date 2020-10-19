#include "snake.hpp"

#include <algorithm>

Snake::Snake(size_t width, size_t height) : m_Head(0, 0),
                                            m_Direction(Direction::NONE), m_Width(width), m_Height(height) {
    m_Body.reserve(width * height);
}

void Snake::tick() {
    glm::ivec2 prev = m_Head;
    switch (m_Direction) {
        case Direction::UP:
            m_Head.y++;
            break;
        case Direction::DOWN:
            m_Head.y--;
            break;
        case Direction::LEFT:
            m_Head.x--;
            break;
        case Direction::RIGHT:
            m_Head.x++;
            break;
        default:
            break;
    }

    if (m_Head.x == prev.x && m_Head.y == prev.y && !m_Body.empty()) {
        m_Body[m_Body.size() - 1] = prev;
    }
}

void Snake::switchDirection(Snake::Direction direction) {
    m_Direction = direction;
}

bool Snake::doesCollide() const {
    //if (m_Head.x == 0 || m_Head.x == m_Width - 1 || m_Head.y == 0 || m_Head.y == m_Height - 1) {
    //    return true;
    //}

    for (const auto& pos : m_Body) {
        if (pos == m_Head) return true;
    }

    return std::any_of(m_Body.begin(), m_Body.end(), [&](const glm::ivec2& pos) { return m_Head == pos; });
}

Snake::Direction Snake::getDirection() const {
    return m_Direction;
}

const glm::ivec2& Snake::getHead() const {
    return m_Head;
}

const std::vector<glm::ivec2>& Snake::getBody() const {
    return m_Body;
}

void Snake::reset() {

}