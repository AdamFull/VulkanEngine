#pragma once
#include "WindowHandle.h"

namespace Engine
{
    class Application
    {
    public:
        template<class ...Args>
        void CreateWindow(Args ...args)
        {
            m_pWinHandle = std::make_unique<WindowHandle>(std::forward<Args>(args)...);
        }
        
        void run();
    private:
        std::unique_ptr<WindowHandle> m_pWinHandle;
    };
}