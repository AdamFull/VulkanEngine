#include "core/Application.hpp"

constexpr int iWidth = 800;
constexpr int iHeight = 600;
constexpr const char* srAppName = "Vulkan window";

Engine::Application app;

int main()
{
    app.CreateWindow(iWidth, iHeight, srAppName);
    app.run();
}