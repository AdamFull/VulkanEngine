#include "Application.hpp"

namespace Engine
{
    void Application::run()
    {
        while (!m_pWinHandle->ShouldClose()) 
        {
            m_pWinHandle->PollEvents();
        }
    }
}