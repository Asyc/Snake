#ifndef SNAKE_WINDOW_HPP
#define SNAKE_WINDOW_HPP

#include <functional>
#include <string_view>
#include <tuple>

struct GLFWwindow;

class Window {
public:
    using KeyCallback = std::function<void(int key, int scancode, int action, int mods)>;
    using ResizeCallback = std::function<void(int width, int height)>;

    Window(int width, int height, const std::string_view& title);

    Window(const Window& rhs) = delete;

    Window(Window&& rhs) noexcept;

    ~Window();

    Window& operator=(const Window& rhs) = delete;

    Window& operator=(Window&& rhs) noexcept;

    void setTitle(const std::string_view& title) const;

    void setKeyCallback(KeyCallback callback);
    void setResizeCallback(ResizeCallback callback);

    [[nodiscard]] bool shouldClose() const;

    [[nodiscard]] std::pair<uint32_t, uint32_t> getSize() const;

    [[nodiscard]] GLFWwindow* getWindowHandle() const;

    static void initLibrary();

    static void pollEvents();

    static void terminateLibrary();

private:
    KeyCallback m_KeyCallback;
    ResizeCallback m_ResizeCallback;
    GLFWwindow* m_WindowHandle;
};


#endif //SNAKE_WINDOW_HPP
