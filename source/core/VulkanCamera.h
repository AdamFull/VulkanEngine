#pragma once

namespace Engine
{
    class VulkanCamera
    {
    public:
        void Update(GLFWwindow *window, float fDeltaTime);
        glm::mat4 View();
        glm::mat4 Perspective(float aspectRatio);
        glm::vec3 m_vecPosition{5.0f, 40.0f, 5.0f};
        float m_fSpeed{5.0f};
        float m_fPitch{};

    private:
        float m_fRoll{};
        float m_fYaw{3.14};
        float m_fFovDegrees{90.0f};
        float m_fNear{0.1f};
        float m_fFar{1000.0f};
    };
}