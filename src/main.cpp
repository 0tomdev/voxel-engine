#include <iostream>

#include "Application.hpp"

int main() {
    Application& app = Application::get();
    if (!app.init()) return 1;
    app.run();
    app.shutDown();

    return 0;
}