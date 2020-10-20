#include <iostream>

#include "game/application.hpp"

void run() {
    Application application;
    application.enterGameLoop();
}

int main() {

#ifdef NDEBUG
    try {
        Window::initLibrary();
        run();
    } catch (const std::exception& ex) {
        Window::terminateLibrary();
        std::cerr << ex.what();
        return -1;
    }
#else
    Window::initLibrary();
    run();
#endif
    Window::terminateLibrary();
    return 0;
}