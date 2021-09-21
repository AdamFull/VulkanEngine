#pragma once
#include "KeycodeConfig.h"

namespace Engine
{
    class WindowHandle;

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
        std::pair<EActionKey, float> pAxisFirst;
        std::pair<EActionKey, float> pAxisSecond;

        EasyDelegate::TDelegate<void(float)> dListener;
    };

    class InputMapper
    {
    public:
        InputMapper(std::unique_ptr<WindowHandle>& winHandle);
        void BindAction(EActionKey eKey, EKeyState eState, EasyDelegate::TDelegate<void()>&& dCallback);
        void CreateAxis(std::string srAxisName, EActionKey eKeyFirst, float fFMax, EActionKey eKeySecond, float fSMax);
        void BindAxis(std::string srAxisName, EasyDelegate::TDelegate<void(float)>&& dCallback);
        void Update(float fDeltaTime);
    private:
        void KeyBoardInput(int key, int scancode, int action, int mods);
        void MouseInput(double xpos, double ypos);

        FInputAction MakeBindAction(EKeyState eState, EasyDelegate::TDelegate<void()>&& dCallback);
        FInputAxis MakeBindAxis(EActionKey eKeyFirst, float fFMax, EActionKey eKeySecond, float fSMax);

        void HandleActions(const EActionKey& eActionKey, const EKeyState& eKeyState);
        void HandleAxis(const EActionKey& eActionKey, const EKeyState& eKeyState);

        std::multimap<EActionKey, FInputAction> m_mInputActions;
        std::multimap<std::string, FInputAxis> m_mInputAxis;

        std::map<EActionKey, EKeyState> m_mKeyStates;
        std::map<EActionKey, float> m_mAxisStates;
    };
}