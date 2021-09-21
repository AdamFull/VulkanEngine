#include "InputMapper.h"
#include "WindowHandle.h"

namespace Engine
{
    InputMapper::InputMapper(std::unique_ptr<WindowHandle>& winHandle)
    {
        winHandle->KeyCodeCallback.attach(this, &InputMapper::KeyBoardInput);
        winHandle->MousePositionCallback.attach(this, &InputMapper::MouseInput);
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

    void InputMapper::MouseInput(double xpos, double ypos)
    {
        float fMouseX = xpos/100.f;
        float fMouseY = ypos/100.f;

        m_mAxisStates[EActionKey::eMouseX] = fMouseX;
        m_mAxisStates[EActionKey::eMouseY] = fMouseY;
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
                    range_it->second.vListeners.emplace_back(dCallback);
                    return;
                }
            }
        }
        m_mInputActions.emplace(eKey, MakeBindAction(eState, dCallback));
    }

    void InputMapper::CreateAxis(std::string srAxisName, EActionKey eKeyFirst, float fFMax, EActionKey eKeySecond, float fSMax)
    {
        m_mInputAxis.emplace(srAxisName, MakeBindAxis(eKeyFirst, fFMax, eKeySecond, fSMax));
    }

    void InputMapper::BindAxis(std::string srAxisName, EasyDelegate::TDelegate<void(float)>&& dCallback)
    {
        auto it = m_mInputAxis.find(srAxisName);
        if(it != m_mInputAxis.end())
        {
            auto range = m_mInputAxis.equal_range(srAxisName);
            for(auto range_it = range.first; range_it != range.second; ++range_it)
            {
                range_it->second.dListener = dCallback;
            }
        }
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

    FInputAction InputMapper::MakeBindAction(EKeyState eState, EasyDelegate::TDelegate<void()> dCallback)
    {
        FInputAction newAction;
        newAction.eState = eState;
        newAction.vListeners.emplace_back(dCallback);
        return newAction;
    }

    FInputAxis InputMapper::MakeBindAxis(EActionKey eKeyFirst, float fFMax, EActionKey eKeySecond, float fSMax)
    {
        FInputAxis newAxis;
        newAxis.pAxisFirst = std::make_pair(eKeyFirst, fFMax);
        newAxis.pAxisSecond = std::make_pair(eKeySecond, fSMax);
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
        for(auto it = m_mInputAxis.begin(); it != m_mInputAxis.end(); ++it)
        {
            auto range = m_mInputAxis.equal_range(it->first);
            for(auto range_it = range.first; range_it != range.second; ++range_it)
            {
                auto axisIt = m_mAxisStates.find(eActionKey);
                if(axisIt != m_mAxisStates.end())
                {
                    float fAxis = axisIt->second;
                    if(range_it->second.pAxisFirst.first == eActionKey)
                    {
                        float outval = fAxis * range_it->second.pAxisFirst.second;             //Get axis range and clamp betwen 0.0 and fMax
                        range_it->second.dListener(outval);
                    }
                    else if(range_it->second.pAxisSecond.first == eActionKey)
                    {
                        float outval = fAxis * range_it->second.pAxisSecond.second;             //Get axis range and clamp betwen 0.0 and fMax
                        range_it->second.dListener(outval);
                    }
                }
                return;
            }
        }
    }
}