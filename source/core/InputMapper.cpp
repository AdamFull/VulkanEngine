#include "InputMapper.h"
#include "WindowHandle.h"
#include "EngineMath.hpp"

namespace Engine
{
    InputMapper::InputMapper(std::unique_ptr<WindowHandle>& winHandle)
    {
        Window::KeyCodeCallback.attach(this, &InputMapper::KeyBoardInput);
        Window::MousePositionCallback.attach(this, &InputMapper::MouseInput);
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
        fPosXOld = m_mAxisStates[EActionKey::eCursorPosX];
        fPosYOld = m_mAxisStates[EActionKey::eCursorPosY];
        m_mAxisStates[EActionKey::eCursorPosX] = Math::RangeToRange(xpos, 0.0, xmax, -1.0, 1.0);
        m_mAxisStates[EActionKey::eCursorPosY] = Math::RangeToRange(ypos, 0.0, ymax, -1.0, 1.0);

        //Calculate cursor speed
        fSpeedXOld = m_mAxisStates[EActionKey::eCursorSpeedX];
        fSpeedYOld = m_mAxisStates[EActionKey::eCursorSpeedY];
        m_mAxisStates[EActionKey::eCursorSpeedX] = (m_mAxisStates[EActionKey::eCursorPosX] - fPosXOld)/fDeltaTime;
        m_mAxisStates[EActionKey::eCursorSpeedY] = (m_mAxisStates[EActionKey::eCursorPosY] - fPosYOld)/fDeltaTime;

        //Calculate cursor acceleration
        m_mAxisStates[EActionKey::eCursorAccelX] = (m_mAxisStates[EActionKey::eCursorSpeedX] - fSpeedXOld)/fDeltaTime;
        m_mAxisStates[EActionKey::eCursorAccelY] = (m_mAxisStates[EActionKey::eCursorSpeedY] - fSpeedYOld)/fDeltaTime;
    }

    void InputMapper::BindAction(EActionKey eKey, EKeyState eState, EasyDelegate::TDelegate<void()>&& dCallback)
    {
        auto it = m_mInputActions.find(eKey);
        if(it != m_mInputActions.end())
        {
            auto range = m_mInputActions.equal_range(eKey);
            for(auto range_it = range.first; range_it != range.second; ++range_it)
            {
                if(range_it->second.eState == eState)
                {
                    range_it->second.vListeners.emplace_back(std::move(dCallback));
                    return;
                }
            }
        }
        m_mInputActions.emplace(eKey, MakeBindAction(eState, std::move(dCallback)));
    }

    void InputMapper::BindAxis(EActionKey eActionKey, EasyDelegate::TDelegate<void(float)>&& dCallback)
    {
        auto it = m_mInputAxis.find(eActionKey);
        if(it != m_mInputAxis.end())
        {
            auto range = m_mInputAxis.equal_range(eActionKey);
            for(auto range_it = range.first; range_it != range.second; ++range_it)
            {
                range_it->second.vListeners.emplace_back(std::move(dCallback));
            }
        }
        m_mInputAxis.emplace(eActionKey, MakeBindAxis(std::move(dCallback)));
    }

    void InputMapper::Update(float fDeltaTime)
    {
        for(auto& [key, value] : m_mKeyStates)
        {
            HandleActions(key, value);
            HandleAxis(key, value);

            if(value == EKeyState::ePress)
                value = EKeyState::ePressed;
            else if(value == EKeyState::eRelease)
                value = EKeyState::eReleased;
        }
    }

    FInputAction InputMapper::MakeBindAction(EKeyState eState, EasyDelegate::TDelegate<void()>&& dCallback)
    {
        FInputAction newAction;
        newAction.eState = eState;
        newAction.vListeners.emplace_back(std::move(dCallback));
        return newAction;
    }

    FInputAxis InputMapper::MakeBindAxis(EasyDelegate::TDelegate<void(float)>&& dCallback)
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

    void InputMapper::HandleAxis(const EActionKey& eActionKey, const EKeyState& eKeyState)
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
                        listener(axisIt->second);
                    }
                }
            }
        }
    }
}