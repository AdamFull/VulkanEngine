#include "InputMapper.h"

namespace Engine
{
    void InputMapper::BindAction(EActionKey eKey, EKeyState eState, EasyDelegate::TDelegate<void()> dCallback)
    {
        FInputAction action;
        action.eState = eState;
        //????
    }

    void InputMapper::BindAxis(EActionKey eKey, EasyDelegate::TDelegate<void(float)> dCallback)
    {

    }
}