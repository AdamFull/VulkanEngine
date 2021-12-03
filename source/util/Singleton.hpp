#pragma once
#include "NonCopyable.hpp"
#include "NonMovable.hpp"

template<class T>
class Singleton : public NonCopyable, public NonMovable
{
protected:
    static std::unique_ptr<T> m_pInstance;
public:
    static std::unique_ptr<T>& getInstance()
    {
        if(!m_pInstance)
            m_pInstance.reset(new T());
        return m_pInstance;
    }
};