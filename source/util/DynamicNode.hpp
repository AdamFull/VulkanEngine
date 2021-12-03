#pragma once
#include "Transform.hpp"

template<class T>
class DynamicNode : public std::enable_shared_from_this<DynamicNode<T>>
{
public:
    virtual ~DynamicNode()
    {

    }

    inline void SetName(std::string srName) 
    { 
        m_srName = srName; 
    }

    inline std::string& GetName() 
    { 
        return m_srName;
    }

    inline std::shared_ptr<T> &GetParent() 
    { 
        return m_pParent; 
    }

    inline std::unordered_map<std::string, std::shared_ptr<T>> &GetChilds() 
    { 
        return m_mChilds;
    }

    inline std::shared_ptr<T> Find(std::string srName)
    {
        auto it = m_mChilds.find(srName);
        if (it != m_mChilds.end())
            return it->second;

        for (auto &[name, child] : m_mChilds)
            child->Find(srName);

        return nullptr;
    }

    inline void AddChild(std::shared_ptr<T> child)
    {
        m_mChilds.emplace(child->m_srName, child);
        child->SetParent(std::dynamic_pointer_cast<T>(shared_from_this()));
    }

    inline void SetParent(std::shared_ptr<T> parent)
    {
        m_pParentOld = m_pParent;
        m_pParent = parent;
        // If you set parent for this, you should attach self to parent's child's
        if (m_pParentOld)
            m_pParentOld->Detach(std::dynamic_pointer_cast<T>(shared_from_this()));
    }

    inline void Attach(std::shared_ptr<T> child)
    {
        AddChild(child);
    }

    inline void Detach(std::shared_ptr<T> child)
    {
        auto it = m_mChilds.find(child->m_srName);
        if (it != m_mChilds.end())
            m_mChilds.erase(it);
    }

    inline FTransform GetTransform()
    {
        FTransform transform = m_transform;
        if (m_pParent)
            transform += m_pParent->GetTransform();
        return transform;
    }

    inline const glm::vec3 GetPosition() const
    {
        glm::vec3 position = m_transform.pos;
        if (m_pParent)
            position += m_pParent->GetPosition();
        return position;
    }

    inline const glm::vec3 GetRotation() const
    {
        glm::vec3 rotation = m_transform.rot;
        if (m_pParent)
            rotation += m_pParent->GetRotation();
        return rotation;
    }

    inline const glm::vec3 GetScale() const
    {
        glm::vec3 scale = m_transform.scale;
        if (m_pParent)
            scale *= m_pParent->GetScale();
        return scale;
    }

    inline void SetTransform(FTransform transformNew) 
    { 
        m_transform = transformNew; 
    }

    inline void SetPosition(glm::vec3 position) 
    { 
        m_transform.pos = position; 
    }

    inline void SetRotation(glm::vec3 rotation) 
    { 
        m_transform.rot = rotation; 
    }

    inline void SetScale(glm::vec3 scale) 
    { 
        m_transform.scale = scale; 
    }
protected:
    std::string m_srName;
    FTransform m_transform;

    std::shared_ptr<T> m_pParent;
    std::shared_ptr<T> m_pParentOld;
    std::unordered_map<std::string, std::shared_ptr<T>> m_mChilds;
};