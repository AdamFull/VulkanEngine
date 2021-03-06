#pragma once

namespace engine
{
    constexpr const int PressedState = GLFW_PRESS;
    constexpr const int ReleasedState = GLFW_RELEASE;

    enum class EKeyState
    {
        ePress,         // Just pressed
        ePressed,       // Pressed and not released
        eRelease,       // Just released
        eReleased       // Flying
    };

    enum class EActionKey
    {
        //Mouse
        eMouseLeft = GLFW_MOUSE_BUTTON_LEFT,
        eMouseRight = GLFW_MOUSE_BUTTON_RIGHT,
        eMouseMiddle = GLFW_MOUSE_BUTTON_MIDDLE,
        eMouse4 = GLFW_MOUSE_BUTTON_4,
        eMouse5 = GLFW_MOUSE_BUTTON_5,
        eMouse6 = GLFW_MOUSE_BUTTON_6,
        eMouse7 = GLFW_MOUSE_BUTTON_7,
        eMouse8 = GLFW_MOUSE_BUTTON_8,

        eGamepadA = eMouseLeft,
        eGamepadB = eMouseRight,
        eGamepadX = eMouseMiddle,
        eGamepadY = eMouse4,
        eGamepadLeftBumper = eMouse5,
        eGamepadRightBumper = eMouse6,
        eGamepadBack = eMouse7,
        eGamepadStart = eMouse8,
        eGamepadGuide = GLFW_GAMEPAD_BUTTON_GUIDE,
        eGamepadLeftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
        eGamepadRightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
        eGamepadDPadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,
        eGamepadDPadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
        eGamepadDPadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
        eGamepadDPadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
        eGamepadCross = eGamepadA,
        eGamepadCircle = eGamepadB,
        eGamepadSquare = eGamepadX,
        eGamepadTriangle = eGamepadY,

        eSpace = GLFW_KEY_SPACE,
        eApostrophe = GLFW_KEY_APOSTROPHE,
        eComma = GLFW_KEY_COMMA,
        eMinus = GLFW_KEY_MINUS,
        ePeriod = GLFW_KEY_PERIOD,
        eSlash = GLFW_KEY_SLASH,
        e0 = GLFW_KEY_0,
        e1 = GLFW_KEY_1,
        e2 = GLFW_KEY_2,
        e3 = GLFW_KEY_3,
        e4 = GLFW_KEY_4,
        e5 = GLFW_KEY_5,
        e6 = GLFW_KEY_6,
        e7 = GLFW_KEY_7,
        e8 = GLFW_KEY_8,
        e9 = GLFW_KEY_9,
        eSemicolon = GLFW_KEY_SEMICOLON,
        eEqual = GLFW_KEY_EQUAL,
        eA = GLFW_KEY_A,
        eB = GLFW_KEY_B,
        eC = GLFW_KEY_C,
        eD = GLFW_KEY_D,
        eE = GLFW_KEY_E,
        eF = GLFW_KEY_F,
        eG = GLFW_KEY_G,
        eH = GLFW_KEY_H,
        eI = GLFW_KEY_I,
        eJ = GLFW_KEY_J,
        eK = GLFW_KEY_K,
        eL = GLFW_KEY_L,
        eM = GLFW_KEY_M,
        eN = GLFW_KEY_N,
        eO = GLFW_KEY_O,
        eP = GLFW_KEY_P,
        eQ = GLFW_KEY_Q,
        eR = GLFW_KEY_R,
        eS = GLFW_KEY_S,
        eT = GLFW_KEY_T,
        eU = GLFW_KEY_U,
        eV = GLFW_KEY_V,
        eW = GLFW_KEY_W,
        eX = GLFW_KEY_X,
        eY = GLFW_KEY_Y,
        eZ = GLFW_KEY_Z,
        eLeftBracket = GLFW_KEY_LEFT_BRACKET,
        eBacksLash = GLFW_KEY_BACKSLASH,
        eRightBracket = GLFW_KEY_RIGHT_BRACKET,
        eGraveAccent = GLFW_KEY_GRAVE_ACCENT,
        eWorld1 = GLFW_KEY_WORLD_1,
        eWorld2 = GLFW_KEY_WORLD_2,
        eEscape = GLFW_KEY_ESCAPE,
        eEnter = GLFW_KEY_ENTER,
        eTAB = GLFW_KEY_TAB,
        eBackspace = GLFW_KEY_BACKSPACE,
        eInsert = GLFW_KEY_INSERT,
        eDelete = GLFW_KEY_DELETE,
        eRight = GLFW_KEY_RIGHT,
        eLeft = GLFW_KEY_LEFT,
        eDown = GLFW_KEY_DOWN,
        eUp = GLFW_KEY_UP,
        ePageUp = GLFW_KEY_PAGE_UP,
        ePageDown = GLFW_KEY_PAGE_DOWN,
        eHome = GLFW_KEY_HOME,
        eEnd = GLFW_KEY_END,
        eCapsLock = GLFW_KEY_CAPS_LOCK,
        eScrollLock = GLFW_KEY_SCROLL_LOCK,
        eNumLock = GLFW_KEY_NUM_LOCK, 
        ePrintScreen = GLFW_KEY_PRINT_SCREEN,
        ePause = GLFW_KEY_PAUSE,
        eF1 = GLFW_KEY_F1,
        eF2 = GLFW_KEY_F2,
        eF3 = GLFW_KEY_F3,
        eF4 = GLFW_KEY_F4,
        eF5 = GLFW_KEY_F5,
        eF6 = GLFW_KEY_F6,
        eF7 = GLFW_KEY_F7,
        eF8 = GLFW_KEY_F8,
        eF9 = GLFW_KEY_F9,
        eF10 = GLFW_KEY_F10,
        eF11 = GLFW_KEY_F11,
        eF12 = GLFW_KEY_F12,
        eF13 = GLFW_KEY_F13,
        eF14 = GLFW_KEY_F14,
        eF15 = GLFW_KEY_F15,
        eF16 = GLFW_KEY_F16,
        eF17 = GLFW_KEY_F17,
        eF18 = GLFW_KEY_F18,
        eF19 = GLFW_KEY_F19,
        eF20 = GLFW_KEY_F20,
        eF21 = GLFW_KEY_F21,
        eF22 = GLFW_KEY_F22,
        eF23 = GLFW_KEY_F23,
        eF24 = GLFW_KEY_F24,
        eF25 = GLFW_KEY_F25,
        eKP0 = GLFW_KEY_KP_0,
        eKP1 = GLFW_KEY_KP_1,
        eKP2 = GLFW_KEY_KP_2,
        eKP3 = GLFW_KEY_KP_3,
        eKP4 = GLFW_KEY_KP_4,
        eKP5 = GLFW_KEY_KP_5,
        eKP6 = GLFW_KEY_KP_6,
        eKP7 = GLFW_KEY_KP_7,
        eKP8 = GLFW_KEY_KP_8,
        eKP9 = GLFW_KEY_KP_9,
        eKPDecimal = GLFW_KEY_KP_DECIMAL,
        eKPDivide = GLFW_KEY_KP_DIVIDE,
        eKPMultiply = GLFW_KEY_KP_MULTIPLY,
        eKPSubtract = GLFW_KEY_KP_SUBTRACT,
        eKPAdd = GLFW_KEY_KP_ADD,
        eKPEnter = GLFW_KEY_KP_ENTER,
        eKPEqual = GLFW_KEY_KP_EQUAL,
        eLeftShift = GLFW_KEY_LEFT_SHIFT,
        eLeftControl = GLFW_KEY_LEFT_CONTROL,
        eLeftAlt = GLFW_KEY_LEFT_ALT,
        eLeftSuper = GLFW_KEY_LEFT_SUPER,
        eRightShift = GLFW_KEY_RIGHT_SHIFT,
        eRightControl = GLFW_KEY_RIGHT_CONTROL,
        eRightAlt = GLFW_KEY_RIGHT_ALT,
        eRightSuper = GLFW_KEY_RIGHT_SUPER,
        eMenu = GLFW_KEY_MENU,
        eCursorOriginal,
        eCursorPos,
        eCursorDelta,
        eScrol
    };
}