#include "core/Application.hpp"

int main() {
    Application *application = new Application();
    application->run();

    delete application;
    application = nullptr;
    
    return 0;
}