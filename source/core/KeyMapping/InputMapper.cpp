#include "InputMapper.h"
#include "Renderer/Window/WindowHandle.h"
#include "Renderer/Window/WinCallbacks.h"
#include "EngineMath.hpp"

namespace Engine
{
    std::unique_ptr<InputMapper> InputMapper::m_pInstance{nullptr};

    InputMapper::InputMapper()
    {
        WinCallbacks::Subscribe(EWinCallbackType::eInputKey, this, &InputMapper::KeyBoardInput);
        WinCallbacks::Subscribe(EWinCallbackType::
        WindowHandle::MousePositionCallback.attach(this, &InputMapper::MouseMovementInput);
        WindowHandle::MouseWheelCallback.attach(this, &InputMapper::MouseWheelInput);
    }

    InputMapper::~InputMapper()
    {
        WindowHandle::KeyCodeCallback.detach();
        WindowHandle::MousePositionCallback.detach();
        WindowHandle::MouseWheelCallback.detach();
    }

    std::unique_ptr<InputMapper>& InputMapper::GetInstance()
    {
        if(!m_pInstance)
        {
            m_pInstance.reset(new InputMapper());
        }
        return m_pInstance;
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

    void InputMapper::MouseMovementInput(double xpos, double ypos, double xmax, double ymax)
    {
        m_mAxisStates[EActionKey::eCursorOriginal] = {xpos, ypos};
        //Calculate on screen position
        fPosOld = m_mAxisStates[EActionKey::eCursorPos];
        m_mAxisStates[EActionKey::eCursorPos] = {Math::RangeToRange(xpos, 0.0, xmax, -1.0, 1.0), Math::RangeToRange(ypos, 0.0, ymax, -1.0, 1.0)};

        //Calculate cursor pos delta
        m_mAxisStates[EActionKey::eCursorDelta] = (m_mAxisStates[EActionKey::eCursorPos] - fPosOld)*m_fDeltaTime;
    }

    void InputMapper::MouseWheelInput(double xpos, double ypos)
    {
        m_mAxisStates[EActionKey::eScrol] = glm::vec2{xpos * m_fDeltaTime, ypos * m_fDeltaTime};
    }

    void InputMapper::Update(float fDeltaTime)
    {
        m_fDeltaTime = fDeltaTime;

        for(auto& [key, value] : m_mInputDescriptor)
        {
            auto keyState = m_mKeyStates.find(key);
            if(keyState != m_mKeyStates.end())
            {
                //If we have multiple bindings to this key
                auto range = m_mInputDescriptor.equal_range(key);
                for(auto range_it = range.first; range_it != range.second; ++range_it)
                {
                    HandleActions(range_it->second, key, keyState->second);
                }

                if(keyState->second == EKeyState::ePress)
                    keyState->second = EKeyState::ePressed;
                else if(keyState->second == EKeyState::eRelease)
                    keyState->second = EKeyState::eReleased;
            }

            auto keyAxis = m_mAxisStates.find(key);
            if(keyAxis != m_mAxisStates.end())
            {
                //If we have multiple bindings to this key
                auto range = m_mInputDescriptor.equal_range(key);
                for(auto range_it = range.first; range_it != range.second; ++range_it)
                {
                    HandleAxis(range_it->second, keyAxis->second);
                }

                if(key != EActionKey::eCursorOriginal)
                m_mAxisStates[key] = {};        //Reset axis
            }
        }
    }

    void InputMapper::HandleActions(std::string srActionName, EActionKey eKey, const EKeyState& eKeyState)
    {
        auto it = m_mInputActions.find(srActionName);
        if(it != m_mInputActions.end())
        {
            auto range = m_mInputActions.equal_range(srActionName);
            for(auto range_it = range.first; range_it != range.second; ++range_it)
            {
                if(range_it->second.eState == eKeyState)
                {
                    auto& vListeners = range_it->second.vListeners;
                    for(auto& listener : vListeners)
                    {
                        listener(eKey, eKeyState);
                    }
                }
            }
        }
    }

    void InputMapper::HandleAxis(std::string srActionName, glm::vec2 fValue)
    {
        auto it = m_mInputAxis.find(srActionName);
        if (it != m_mInputAxis.end())
        {
            auto range = m_mInputAxis.equal_range(srActionName);
            for (auto range_it = range.first; range_it != range.second; ++range_it)
            {
                auto &vListeners = range_it->second.vListeners;
                for (auto &listener : vListeners)
                {
                    listener(fValue.x, fValue.y);
                }
            }
        }
    }

    FInputAction InputMapper::MakeBindAction(EKeyState eState, EasyDelegate::TDelegate<void(EActionKey, EKeyState)>&& dCallback)
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
}