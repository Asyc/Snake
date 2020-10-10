#include "window.hpp"

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

Window::Window(int width, int height, const std::string_view& title) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_FALSE);
    m_WindowHandle = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
}

Window::Window(Window&& rhs) noexcept {
    m_WindowHandle = rhs.m_WindowHandle;
    rhs.m_WindowHandle = nullptr;
}

Window::~Window() {
    glfwDestroyWindow(m_WindowHandle);
}

Window& Window::operator=(Window&& rhs) noexcept {
    if (m_WindowHandle == nullptr) return *this;

    m_WindowHandle = rhs.m_WindowHandle;
    rhs.m_WindowHandle = nullptr;

    return *this;
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_WindowHandle);
}

GLFWwindow* Window::getWindowHandle() const {
    return m_WindowHandle;
}

void Window::initLibrary() {
    glfwInit();
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::terminateLibrary() {
    glfwTerminate();
}
