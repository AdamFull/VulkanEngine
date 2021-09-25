#pragma once

namespace Engine
{
    class InputMapper;

    class CameraController
    {
    public:
        void Initialize(std::unique_ptr<InputMapper>& mapper);

        void BindView(EasyDelegate::TDelegate<void(glm::vec3, glm::vec3)>&& viewRot);

    private:
        void MoveForward();
        void MoveBackward();
        void MoveLeft();
        void MoveRight();
        void MoveUp();
        void MoveDown();
        void MouseRotationX(float fX);
        void MouseRotationY(float fY);

        glm::vec3 m_vecRotation;
        glm::vec3 m_vecPosition{0, 0, 0};

        EasyDelegate::TDelegate<void(glm::vec3, glm::vec3)> ViewRotation;

        float m_fMoveSpeed{3.f};
        float m_fLookSpeed{1.5f};
    };
}