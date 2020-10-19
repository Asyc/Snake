#include "window.hpp"

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

Window::Window(int width, int height, const std::string_view& title) {
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_FALSE);
    m_WindowHandle = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_WindowHandle, this);
}

Window::Window(Window&& rhs) noexcept {
    m_WindowHandle = rhs.m_WindowHandle;
    rhs.m_WindowHandle = nullptr;
    glfwSetWindowUserPointer(m_WindowHandle, this);
}

Window::~Window() {
    glfwDestroyWindow(m_WindowHandle);
}

Window& Window::operator=(Window&& rhs) noexcept {
    if (m_WindowHandle == nullptr) return *this;

    m_WindowHandle = rhs.m_WindowHandle;
    rhs.m_WindowHandle = nullptr;
    glfwSetWindowUserPointer(m_WindowHandle, this);

    return *this;
}

void Window::setKeyCallback(KeyCallback callback) {
    m_KeyCallback = std::move(callback);
    glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* handle, int key, int scancode, int action, int mods) {
        auto* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(handle));
        window->m_KeyCallback(key, scancode, action, mods);
    });
}

void Window::setTitle(const std::string_view& title) const {
    glfwSetWindowTitle(m_WindowHandle, title.data());
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_WindowHandle);
}

std::pair<uint32_t, uint32_t> Window::getSize() const {
    int width, height;
    glfwGetWindowSize(m_WindowHandle, &width, &height);
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
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
