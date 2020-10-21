#include "snake.hpp"

#include <algorithm>
#include <random>

#include "application.hpp"

Snake::Snake(size_t width, size_t height) : m_Head(0, 0),
                                            m_Direction(Direction::NONE), m_Width(width), m_Height(height), m_Target(4, 2) {
    m_BodySet.reserve(width * height);
    reset();

}

void Snake::tick() {
    if (!m_DirectionQueue.empty()) {
        m_Direction = m_DirectionQueue.front();
        m_DirectionQueue.pop();
    }

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

    m_Body.push_front(prev);
    m_BodySet.insert(prev);

    if (m_Head == m_Target) {
        setTarget();
    } else if (!m_Body.empty()) {
        m_BodySet.erase(m_Body.back());
        m_Body.pop_back();
    }
}

void Snake::switchDirection(Snake::Direction direction) {
    if (m_DirectionQueue.size() >= 10) m_DirectionQueue.back() = direction;
    else m_DirectionQueue.push(direction);
}

bool Snake::doesCollide() const {
    if (std::abs(m_Head.x) == Application::HORIZONTAL_TILES / 2 || std::abs(m_Head.y) == Application::VERTICAL_TILES / 2) {
        return true;
    }

    return m_BodySet.find(m_Head) != m_BodySet.end();
}

const glm::ivec2& Snake::getTarget() const {
    return m_Target;
}

Snake::Direction Snake::getDirection() const {
    return m_DirectionQueue.empty() ? m_Direction : m_DirectionQueue.front();
}

const glm::ivec2& Snake::getHead() const {
    return m_Head;
}

const std::list<glm::ivec2>& Snake::getBody() const {
    return m_Body;
}

void Snake::reset() {
    m_Direction = Direction::NONE;
    m_DirectionQueue = std::queue<Direction>();
    m_Head = glm::ivec2(0, 0);
    m_Body.clear();
    m_BodySet.clear();
    m_Body.push_front(glm::ivec2(0, -1));
    m_BodySet.insert(m_Body.front());
    m_Target = glm::ivec2(2, 0);
}

void Snake::setTarget() {
    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_real_distribution<double> hDist(Application::HORIZONTAL_TILES / -2.0f, Application::HORIZONTAL_TILES / 2.0f);
    static std::uniform_real_distribution<double> vDist(Application::VERTICAL_TILES / -2.0f, Application::VERTICAL_TILES / 2.0f);

    auto it = m_BodySet.end();
    glm::ivec2 generated;

    do {
        generated = glm::ivec2(hDist(mt), vDist(mt));
        it = m_BodySet.find(generated);
    } while (it != m_BodySet.end());

    m_Target = generated;
}
