#pragma once

namespace Engine
{
    class InputMapper;
    class CameraBase;

    class CameraController
    {
    public:
        void Initialize(std::unique_ptr<InputMapper>& mapper);
        void AttachCamera(std::unique_ptr<CameraBase>& camera);

    private:
        void MoveForward();
        void MoveBackward();
        void MoveLeft();
        void MoveRight();
        void MoveUp();
        void MoveDown();
        void MouseRotation(float fX, float fY);

        glm::vec3 m_vecRotation{0.f, 0.f, 0.f};
        glm::vec3 m_vecPosition{0.f, 0.f, 1.0};

        EasyDelegate::TDelegate<void(glm::vec3, glm::vec3)> ViewRotation;

        float m_fMoveSpeed{3.f};
        float m_fLookSpeed{500.f};
    };
}