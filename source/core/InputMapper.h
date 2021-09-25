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
        std::vector<EasyDelegate::TDelegate<void(float, float)>> vListeners;
    };

    class InputMapper
    {
    public:
        InputMapper();

        InputMapper(const InputMapper&) = delete;
        void operator=(const InputMapper&) = delete;
        InputMapper(InputMapper&&) = delete;
        InputMapper& operator=(InputMapper&&) = delete;

        template<class ...Args>
        void BindAction(EActionKey eKey, EKeyState eState, Args &&...args)
        {
            auto it = m_mInputActions.find(eKey);
            if(it != m_mInputActions.end())
            {
                auto range = m_mInputActions.equal_range(eKey);
                for(auto range_it = range.first; range_it != range.second; ++range_it)
                {
                    if(range_it->second.eState == eState)
                    {
                        range_it->second.vListeners.emplace_back(EasyDelegate::TDelegate<void()>(std::forward<Args>(args)...));
                        return;
                    }
                }
            }
            m_mInputActions.emplace(eKey, MakeBindAction(eState, EasyDelegate::TDelegate<void()>(std::forward<Args>(args)...)));
        }

        template<class ...Args>
        void BindAxis(EActionKey eActionKey, Args &&...args)
        {
            auto it = m_mInputAxis.find(eActionKey);
            if(it != m_mInputAxis.end())
            {
                auto range = m_mInputAxis.equal_range(eActionKey);
                for(auto range_it = range.first; range_it != range.second; ++range_it)
                {
                    range_it->second.vListeners.emplace_back(EasyDelegate::TDelegate<void(float, float)>(std::forward<Args>(args)...));
                }
            }
            m_mInputAxis.emplace(eActionKey, MakeBindAxis(EasyDelegate::TDelegate<void(float, float)>(std::forward<Args>(args)...)));
        }

        void Update(float fDeltaTime);
    private:
        void KeyBoardInput(int key, int scancode, int action, int mods);
        void MouseInput(double xpos, double ypos, double xmax, double ymax);

        FInputAction MakeBindAction(EKeyState eState, EasyDelegate::TDelegate<void()>&& dCallback);
        FInputAxis MakeBindAxis(EasyDelegate::TDelegate<void(float, float)>&& dCallback);

        void HandleActions(const EActionKey& eActionKey, const EKeyState& eKeyState);
        void HandleAxis(const EActionKey& eActionKey,  glm::vec2 fValue);

        std::multimap<EActionKey, FInputAction> m_mInputActions;
        std::multimap<EActionKey, FInputAxis> m_mInputAxis;

        std::map<EActionKey, EKeyState> m_mKeyStates;
        std::map<EActionKey, glm::vec2> m_mAxisStates;

         glm::vec2 fPosOld{0.f, 0.f};
        float m_fDeltaTime{0};
    };
}