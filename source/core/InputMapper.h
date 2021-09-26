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
        std::vector<EasyDelegate::TDelegate<void(EActionKey)>> vListeners;
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
        void CreateAction(std::string srActionName, Args &&...args)
        {
            //static_assert(std::is_same_v<Args, EActionKey>&&..., "Type that you send to CreateAction is not supported.");
            std::array<EActionKey, sizeof...(Args)> aKeys = { { args... } };
            for(auto key : aKeys)
            {
                m_mInputDescriptor.emplace(std::make_pair(key, srActionName));
            }
        }

        template<class ...Args>
        void BindAction(std::string srActionName, EKeyState eState, Args &&...args)
        {
            auto it = m_mInputActions.find(srActionName);
            if(it != m_mInputActions.end())
            {
                auto range = m_mInputActions.equal_range(srActionName);
                for(auto range_it = range.first; range_it != range.second; ++range_it)
                {
                    if(range_it->second.eState == eState)
                    {
                        range_it->second.vListeners.emplace_back(EasyDelegate::TDelegate<void(EActionKey)>(std::forward<Args>(args)...));
                        return;
                    }
                }
            }
            m_mInputActions.emplace(srActionName, MakeBindAction(eState, EasyDelegate::TDelegate<void(EActionKey)>(std::forward<Args>(args)...)));
        }

        template<class ...Args>
        void BindAxis(std::string srAxisName, Args &&...args)
        {
            auto it = m_mInputAxis.find(srAxisName);
            if(it != m_mInputAxis.end())
            {
                auto range = m_mInputAxis.equal_range(srAxisName);
                for(auto range_it = range.first; range_it != range.second; ++range_it)
                {
                    range_it->second.vListeners.emplace_back(EasyDelegate::TDelegate<void(float, float)>(std::forward<Args>(args)...));
                }
            }
            m_mInputAxis.emplace(srAxisName, MakeBindAxis(EasyDelegate::TDelegate<void(float, float)>(std::forward<Args>(args)...)));
        }

        void Update(float fDeltaTime);
    private:
        void KeyBoardInput(int key, int scancode, int action, int mods);
        void MouseMovementInput(double xpos, double ypos, double xmax, double ymax);
        void MouseWheelInput(double xpos, double ypos);

        void HandleActions(std::string srActionName, EActionKey eKey, const EKeyState& eKeyState);
        void HandleAxis(std::string srAxisName,  glm::vec2 fValue);

        FInputAction MakeBindAction(EKeyState eState, EasyDelegate::TDelegate<void(EActionKey)>&& dCallback);
        FInputAxis MakeBindAxis(EasyDelegate::TDelegate<void(float, float)>&& dCallback);

        std::multimap<EActionKey, std::string> m_mInputDescriptor;

        std::multimap<std::string, FInputAction> m_mInputActions;
        std::multimap<std::string, FInputAxis> m_mInputAxis;

        std::map<EActionKey, EKeyState> m_mKeyStates;
        std::map<EActionKey, glm::vec2> m_mAxisStates;

        glm::vec2 fPosOld{0.f, 0.f};
        float m_fDeltaTime{0};
    };
}