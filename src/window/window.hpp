#ifndef SNAKE_WINDOW_HPP
#define SNAKE_WINDOW_HPP

#include <string_view>

struct GLFWwindow;

class Window {
public:
    Window(int width, int height, const std::string_view& title);
    Window(const Window& rhs) = delete;
    Window(Window&& rhs) noexcept;
    ~Window();

    Window& operator=(const Window& rhs) = delete;
    Window& operator=(Window&& rhs) noexcept;

    [[nodiscard]] bool shouldClose() const;
    [[nodiscard]] GLFWwindow* getWindowHandle() const;

    static void initLibrary();
    static void pollEvents();
    static void terminateLibrary();
private:
    GLFWwindow* m_WindowHandle;
};


#endif //SNAKE_WINDOW_HPP
