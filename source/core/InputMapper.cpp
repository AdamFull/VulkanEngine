#include "InputMapper.h"
#include "WindowHandle.h"
#include "EngineMath.hpp"

namespace Engine
{
    InputMapper::InputMapper()
    {
        WindowHandle::KeyCodeCallback.attach(this, &InputMapper::KeyBoardInput);
        WindowHandle::MousePositionCallback.attach(this, &InputMapper::MouseInput);
    }

    void InputMapper::KeyBoardInput(int key, int scancode, int action, int mods)
    {
        switch (action)
        {
        case PressedState:
            m_mKeyStates[static_cast<EActionKey>(key)] = EKeyState::ePress;
            break;
        case ReleasedState:
            m_mKeyStates[static_cast<EActionKey>(key)] = EKeyState::eRelease;
        default:
            return;
            break;
        }
    }

    void InputMapper::MouseInput(double xpos, double ypos, double xmax, double ymax)
    {   
        //Calculate on screen position
        fPosOld = m_mAxisStates[EActionKey::eCursorPos];
        m_mAxisStates[EActionKey::eCursorPos] = {Math::RangeToRange(xpos, 0.0, xmax, -1.0, 1.0), Math::RangeToRange(ypos, 0.0, ymax, -1.0, 1.0)};

        //Calculate cursor pos delta
        m_mAxisStates[EActionKey::eCursorDelta] = (m_mAxisStates[EActionKey::eCursorPos] - fPosOld)*m_fDeltaTime;
    }

    void InputMapper::Update(float fDeltaTime)
    {
        m_fDeltaTime = fDeltaTime;
        for(auto& [key, value] : m_mKeyStates)
        {
            HandleActions(key, value);

            if(value == EKeyState::ePress)
                value = EKeyState::ePressed;
            else if(value == EKeyState::eRelease)
                value = EKeyState::eReleased;
        }

        for(auto& [key, value] : m_mAxisStates)
        {
            HandleAxis(key, value);
        }

        m_mAxisStates[EActionKey::eCursorDelta] = {};
        m_mAxisStates[EActionKey::eCursorDelta] = {};
    }

    FInputAction InputMapper::MakeBindAction(EKeyState eState, EasyDelegate::TDelegate<void()>&& dCallback)
    {
        FInputAction newAction;
        newAction.eState = eState;
        newAction.vListeners.emplace_back(std::move(dCallback));
        return newAction;
    }

    FInputAxis InputMapper::MakeBindAxis(EasyDelegate::TDelegate<void(float, float)>&& dCallback)
    {
        FInputAxis newAxis;
        newAxis.vListeners.emplace_back(std::move(dCallback));
        return newAxis;
    }

    void InputMapper::HandleActions(const EActionKey& eActionKey, const EKeyState& eKeyState)
    {
        auto it = m_mInputActions.find(eActionKey);
        if(it != m_mInputActions.end())
        {
            auto range = m_mInputActions.equal_range(eActionKey);
            for(auto range_it = range.first; range_it != range.second; ++range_it)
            {
                if(range_it->second.eState == eKeyState)
                {
                    auto& vListeners = range_it->second.vListeners;
                    for(auto& listener : vListeners)
                    {
                        listener();
                    }
                }
            }
        }
    }

    void InputMapper::HandleAxis(const EActionKey& eActionKey, glm::vec2 fValue)
    {
        auto axisIt = m_mAxisStates.find(eActionKey);
        if(axisIt != m_mAxisStates.end())
        {
            auto it = m_mInputAxis.find(eActionKey);
            if(it != m_mInputAxis.end())
            {
                auto range = m_mInputAxis.equal_range(eActionKey);
                for(auto range_it = range.first; range_it != range.second; ++range_it)
                {
                    auto& vListeners = range_it->second.vListeners;
                    for(auto& listener : vListeners)
                    {
                        listener(axisIt->second.x, axisIt->second.y);
                    }
                }
            }
        }
    }
}