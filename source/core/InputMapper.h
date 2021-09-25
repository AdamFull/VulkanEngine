#pragma once
#include "KeycodeConfig.h"

namespace Engine
{
    enum class EKeyState
    {
        ePress,         // Just pressed
        ePressed,       // Pressed and not released
        eRelease,       // Just released
        eReleased       // Flying
    };

    struct FInputAction
    {
        EKeyState eState;
        std::vector<EasyDelegate::TDelegate<void()>> vListeners;
    };

    struct FInputAxis
    {
        std::vector<EasyDelegate::TDelegate<void(float)>> vListeners;
    };

    class InputMapper
    {
    public:
        InputMapper();

        InputMapper(const InputMapper&) = delete;
        void operator=(const InputMapper&) = delete;
        InputMapper(InputMapper&&) = delete;
        InputMapper& operator=(InputMapper&&) = delete;

        void BindAction(EActionKey eKey, EKeyState eState, EasyDelegate::TDelegate<void()>&& dCallback);
        void BindAxis(EActionKey eActionKey, EasyDelegate::TDelegate<void(float)>&& dCallback);
        void Update(float fDeltaTime);
    private:
        void KeyBoardInput(int key, int scancode, int action, int mods);
        void MouseInput(double xpos, double ypos, double xmax, double ymax);

        FInputAction MakeBindAction(EKeyState eState, EasyDelegate::TDelegate<void()>&& dCallback);
        FInputAxis MakeBindAxis(EasyDelegate::TDelegate<void(float)>&& dCallback);

        void HandleActions(const EActionKey& eActionKey, const EKeyState& eKeyState);
        void HandleAxis(const EActionKey& eActionKey, const EKeyState& eKeyState);

        std::multimap<EActionKey, FInputAction> m_mInputActions;
        std::multimap<EActionKey, FInputAxis> m_mInputAxis;

        std::map<EActionKey, EKeyState> m_mKeyStates;
        std::map<EActionKey, float> m_mAxisStates;

        float fPosXOld{0}, fPosYOld{0}, fSpeedXOld{0}, fSpeedYOld{0};
        float fDeltaTime{0};
    };
}