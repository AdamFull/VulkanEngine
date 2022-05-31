#include "WindowHandle.h"
#include "WinCallbacks.h"

using namespace Engine::Core::Window;

namespace Engine
{
    namespace Core
    {
        namespace Window
        {
            void to_json(nlohmann::json &json, const FWindowCreateInfo &type)
            {
                json = nlohmann::json
                {
                    {"name", type.name},
                    {"width", type.width},
                    {"height", type.height},
                    {"fullscreen", type.fullscreen},
                    {"windowed", type.windowed},
                    {"backend", {type.backend}}
                };
            }

            void from_json(const nlohmann::json &json, FWindowCreateInfo &type)
            {
                ParseArgument(json, type.name, "name", true);
                ParseArgument(json, type.width, "width", true);
                ParseArgument(json, type.height, "height", true);
                ParseArgument(json, type.fullscreen, "fullscreen");
                ParseArgument(json, type.windowed, "windowed");
                ParseArgument(json, type.backend, "backend", true);
            }
        }
    }
}

template<>
scope_ptr<CWindowHandle> utl::singleton<CWindowHandle>::_instance{nullptr};

int32_t CWindowHandle::m_iWidth{800};
int32_t CWindowHandle::m_iHeight{600};
bool CWindowHandle::m_bWasResized{false};

CWindowHandle::CWindowHandle()
{
}

CWindowHandle::~CWindowHandle()
{
    close();
}

void CWindowHandle::create(FWindowCreateInfo createInfo)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto* pPrimaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(pPrimaryMonitor);
    if(createInfo.fullscreen)
    {
        createInfo.width = mode->width;
        createInfo.height = mode->height;
    }

    resizeWindow(createInfo.width, createInfo.height);
    frameBufferUpdated();

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    if(createInfo.windowed)
        pWindow = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.name.c_str(), nullptr, nullptr);
    else
        pWindow = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.name.c_str(), pPrimaryMonitor, nullptr);

    glfwSetWindowUserPointer(pWindow, this);
    glfwSetInputMode(pWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
    if(createInfo.fullscreen)
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowSizeCallback(pWindow, &WinCallbacks::WinSizeChangeCallback);
    glfwSetWindowFocusCallback(pWindow, &WinCallbacks::WinInputFocusChangeCallback);
    glfwSetCursorEnterCallback(pWindow, &WinCallbacks::WinInputCursorEnterCallback);
    glfwSetCursorPosCallback(pWindow, &WinCallbacks::WinInputCursorPositionCallback);
    glfwSetMouseButtonCallback(pWindow, &WinCallbacks::WinInputMouseButtonCallback);
    glfwSetScrollCallback(pWindow, &WinCallbacks::WinInputScrollCallback);
    glfwSetKeyCallback(pWindow, &WinCallbacks::WinInputKeyCallback);
    glfwSetCharCallback(pWindow, &WinCallbacks::WinInputCharCallback);
    glfwSetMonitorCallback(&WinCallbacks::WinInputMonitorCallback);

    WinCallbacks::SubscribeSizeChange(this, &CWindowHandle::resizeWindow);
}

void CWindowHandle::createWindowSurface(vk::Instance &instance, const void* pAllocator, VkSurfaceKHR &surface)
{
    VkSurfaceKHR rawSurface;
    if (glfwCreateWindowSurface(instance, pWindow, (const VkAllocationCallbacks*)pAllocator, &rawSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!");
    }

    surface = rawSurface;
}

void CWindowHandle::wait()
{
    while (m_iWidth == 0 || m_iHeight == 0)
    {
        glfwGetFramebufferSize(pWindow, &m_iWidth, &m_iHeight);
        glfwWaitEvents();
    }
}

void CWindowHandle::resizeWindow(int iWidth, int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_bWasResized = true;
}

void CWindowHandle::close()
{
    glfwDestroyWindow(pWindow);
    glfwTerminate();
}
