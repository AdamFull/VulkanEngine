#pragma once
#include "KeycodeConfig.h"

namespace engine
{
    struct FInputAction
    {
        EKeyState eState;
        std::vector<utl::function<void(EActionKey, EKeyState)>> vListeners;
    };

    struct FInputAxis
    {
        std::vector<utl::function<void(float, float)>> vListeners;
    };

    /**
     * @brief Simple input mapper singleton class. Creates bindings for input devices
     * 
     */
    class CInputMapper : public utl::singleton<CInputMapper>
    {
    public:
        CInputMapper();
        ~CInputMapper();

        /**
         * @brief Creates binding with KeyCode and action name. After creating action, you can bind your function to this name.
         * 
         * @tparam Args Variadic arguments. In this case should contain EActionKey type
         * @param srActionName New action name
         * @param args Action keys
         */
        template<class ...Args>
        void createAction(const std::string& srActionName, Args &&...args)
        {
            //static_assert(std::is_same_v<Args, EActionKey>&&..., "Type that you send to CreateAction is not supported.");
            std::array<EActionKey, sizeof...(Args)> aKeys = { { args... } };
            for(auto key : aKeys)
            {
                m_mInputDescriptor.emplace(std::make_pair(key, srActionName));
            }
        }

        template<class ...Args>
        void bindAction(const std::string& srActionName, EKeyState eState, Args &&...args)
        {
            auto it = m_mInputActions.find(srActionName);
            if(it != m_mInputActions.end())
            {
                auto range = m_mInputActions.equal_range(srActionName);
                for(auto range_it = range.first; range_it != range.second; ++range_it)
                {
                    if(range_it->second.eState == eState)
                    {
                        range_it->second.vListeners.emplace_back(utl::function<void(EActionKey, EKeyState)>(std::forward<Args>(args)...));
                        return;
                    }
                }
            }
            m_mInputActions.emplace(srActionName, makeBindAction(eState, utl::function<void(EActionKey, EKeyState)>(std::forward<Args>(args)...)));
        }

        template<class ...Args>
        void bindAxis(const std::string& srAxisName, Args &&...args)
        {
            auto it = m_mInputAxis.find(srAxisName);
            if(it != m_mInputAxis.end())
            {
                auto range = m_mInputAxis.equal_range(srAxisName);
                for(auto range_it = range.first; range_it != range.second; ++range_it)
                {
                    range_it->second.vListeners.emplace_back(utl::function<void(float, float)>(std::forward<Args>(args)...));
                }
            }
            m_mInputAxis.emplace(srAxisName, makeBindAxis(utl::function<void(float, float)>(std::forward<Args>(args)...)));
        }

        void update(float fDeltaTime);
    private:
        void keyBoardInput(int key, int scancode, int action, int mods);
        void mouseButtonInput(int button, int action, int mods);
        void mouseMovementInput(float xpos, float ypos);
        void mouseWheelInput(float xpos, float ypos);

        void handleActions(const std::string& srActionName, EActionKey eKey, const EKeyState& eKeyState);
        void handleAxis(const std::string& srAxisName, const glm::vec2& fValue);

        FInputAction makeBindAction(EKeyState eState, utl::function<void(EActionKey, EKeyState)>&& dCallback);
        FInputAxis makeBindAxis(utl::function<void(float, float)>&& dCallback);

        std::multimap<EActionKey, std::string> m_mInputDescriptor;

        std::multimap<std::string, FInputAction> m_mInputActions;
        std::multimap<std::string, FInputAxis> m_mInputAxis;

        std::map<EActionKey, EKeyState> m_mKeyStates;
        std::map<EActionKey, glm::vec2> m_mAxisStates;

        glm::vec2 fPosOld{0.f, 0.f};
        float m_fDeltaTime{0};
    };
}