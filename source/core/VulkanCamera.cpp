#include "Vulkan/VulkanCamera.h"

namespace Engine
{
    void VulkanCamera::Update(GLFWwindow *window, float fDeltaTime)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            fDeltaTime *= 4;
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            m_fSpeed += fDeltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            m_fSpeed = std::max(0.1f, m_fSpeed - fDeltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            m_vecPosition.y += fDeltaTime * m_fSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            m_vecPosition.y -= fDeltaTime * m_fSpeed;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            m_fPitch += fDeltaTime;
            m_fPitch = std::min<float>(m_fPitch, 0.98f * 0.5f * static_cast<float>(3.14));
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            m_fPitch -= fDeltaTime;
            m_fPitch = std::max<float>(m_fPitch, -0.98f * 0.5f * static_cast<float>(3.14));
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            m_fYaw += fDeltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            m_fYaw -= fDeltaTime;
        }

        glm::vec3 lateralMotion{};
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            lateralMotion.z -= fDeltaTime * m_fSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            lateralMotion.z += fDeltaTime * m_fSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            lateralMotion.x -= fDeltaTime * m_fSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            lateralMotion.x += fDeltaTime * m_fSpeed;
        }
        m_vecPosition.x += -lateralMotion.z * std::sin(m_fYaw) + lateralMotion.x * std::cos(m_fYaw);
        m_vecPosition.z += lateralMotion.x * std::sin(m_fYaw) + lateralMotion.z * std::cos(m_fYaw);
    }

    glm::mat4 VulkanCamera::View()
    {
        auto viewing = glm::mat4(1.0);
        viewing[3] = glm::vec4(-m_vecPosition, 1.0f);
        auto r1 = glm::rotate(glm::mat4(1.0), m_fPitch, glm::vec3(1.0, 0.0, 0.0));
        auto r2 = glm::rotate(glm::mat4(1.0), m_fYaw, glm::vec3(0.0, 1.0, 0.0));
        return r1 * r2 * viewing;
    }

    glm::mat4 VulkanCamera::Perspective(float aspectRatio)
    {
        return glm::perspective(glm::radians(m_fFovDegrees), aspectRatio, m_fNear, m_fFar);
    }
}