#include "InputMapper.h"
#include "graphics/Window/WindowHandle.h"
#include "graphics/Window/WinCallbacks.h"
#include "EngineMath.hpp"

using namespace Engine;

template <>
std::unique_ptr<CInputMapper> utl::singleton<CInputMapper>::_instance{nullptr};

CInputMapper::CInputMapper()
{
    Core::Window::WinCallbacks::SubscribeKeyInput(this, &CInputMapper::keyBoardInput);
    Core::Window::WinCallbacks::SubscribeMousePosition(this, &CInputMapper::mouseMovementInput);
    Core::Window::WinCallbacks::SubscribeMouseScroll(this, &CInputMapper::mouseWheelInput);
    Core::Window::WinCallbacks::SubscribeMouseButton(this, &CInputMapper::mouseButtonInput);
}

CInputMapper::~CInputMapper()
{
    /*WindowHandle::KeyCodeCallback.detach();
    WindowHandle::MousePositionCallback.detach();
    WindowHandle::MouseWheelCallback.detach();*/
}

void CInputMapper::keyBoardInput(int key, int scancode, int action, int mods)
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

void CInputMapper::mouseButtonInput(int button, int action, int mods)
{
    keyBoardInput(button, 0, action, mods);
}

void CInputMapper::mouseMovementInput(float xpos, float ypos)
{
    float xmax = static_cast<float>(Core::Window::CWindowHandle::m_iWidth);
    float ymax = static_cast<float>(Core::Window::CWindowHandle::m_iHeight);

    m_mAxisStates[EActionKey::eCursorOriginal] = {xpos, ypos};
    // Calculate on screen position
    fPosOld = m_mAxisStates[EActionKey::eCursorPos];
    m_mAxisStates[EActionKey::eCursorPos] =
        {
            Math::RangeToRange<float>(xpos, 0.0, xmax, -1.0, 1.0),
            Math::RangeToRange<float>(ypos, 0.0, ymax, -1.0, 1.0)};

    // Calculate cursor pos delta
    m_mAxisStates[EActionKey::eCursorDelta] = (m_mAxisStates[EActionKey::eCursorPos] - fPosOld) * m_fDeltaTime;
}

void CInputMapper::mouseWheelInput(float xpos, float ypos)
{
    m_mAxisStates[EActionKey::eScrol] = glm::vec2{xpos * m_fDeltaTime, ypos * m_fDeltaTime};
}

void CInputMapper::update(float fDeltaTime)
{
    m_fDeltaTime = fDeltaTime;

    for (auto &[key, value] : m_mInputDescriptor)
    {
        auto keyState = m_mKeyStates.find(key);
        if (keyState != m_mKeyStates.end())
        {
            // If we have multiple bindings to this key
            auto range = m_mInputDescriptor.equal_range(key);
            for (auto range_it = range.first; range_it != range.second; ++range_it)
            {
                handleActions(range_it->second, key, keyState->second);
            }

            if (keyState->second == EKeyState::ePress)
                keyState->second = EKeyState::ePressed;
            else if (keyState->second == EKeyState::eRelease)
                keyState->second = EKeyState::eReleased;
        }

        auto keyAxis = m_mAxisStates.find(key);
        if (keyAxis != m_mAxisStates.end())
        {
            // If we have multiple bindings to this key
            auto range = m_mInputDescriptor.equal_range(key);
            for (auto range_it = range.first; range_it != range.second; ++range_it)
            {
                handleAxis(range_it->second, keyAxis->second);
            }

            if (key != EActionKey::eCursorOriginal)
                m_mAxisStates[key] = {}; // Reset axis
        }
    }
}

void CInputMapper::handleActions(const std::string &srActionName, EActionKey eKey, const EKeyState &eKeyState)
{
    auto it = m_mInputActions.find(srActionName);
    if (it != m_mInputActions.end())
    {
        auto range = m_mInputActions.equal_range(srActionName);
        for (auto range_it = range.first; range_it != range.second; ++range_it)
        {
            if (range_it->second.eState == eKeyState)
            {
                auto &vListeners = range_it->second.vListeners;
                for (auto &listener : vListeners)
                {
                    listener(eKey, eKeyState);
                }
            }
        }
    }
}

void CInputMapper::handleAxis(const std::string &srActionName, glm::vec2 fValue)
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

FInputAction CInputMapper::makeBindAction(EKeyState eState, utl::function<void(EActionKey, EKeyState)> &&dCallback)
{
    FInputAction newAction;
    newAction.eState = eState;
    newAction.vListeners.emplace_back(std::move(dCallback));
    return newAction;
}

FInputAxis CInputMapper::makeBindAxis(utl::function<void(float, float)> &&dCallback)
{
    FInputAxis newAxis;
    newAxis.vListeners.emplace_back(std::move(dCallback));
    return newAxis;
}