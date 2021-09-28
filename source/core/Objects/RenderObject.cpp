#include "RenderObject.h"
#include "VulkanDevice.h"

namespace Engine
{
    glm::mat4 FTransform::GetModel()
    {
        const float c3 = glm::cos(rot.z);
        const float s3 = glm::sin(rot.z);
        const float c2 = glm::cos(rot.x);
        const float s2 = glm::sin(rot.x);
        const float c1 = glm::cos(rot.y);
        const float s1 = glm::sin(rot.y);
        return glm::mat4{
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                scale.y * (c3 * s1 * s2 - c1 * s3),
                scale.y * (c2 * c3),
                scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                scale.z * (c2 * s1),
                scale.z * (-s2),
                scale.z * (c1 * c2),
                0.0f,
            },
            {pos.x, pos.y, pos.z, 1.0f}};
    }

    glm::mat3 FTransform::GetNormal()
    {
        const float c3 = glm::cos(rot.z);
        const float s3 = glm::sin(rot.z);
        const float c2 = glm::cos(rot.x);
        const float s2 = glm::sin(rot.x);
        const float c1 = glm::cos(rot.y);
        const float s1 = glm::sin(rot.y);
        const glm::vec3 invScale = 1.0f / scale;

        return glm::mat3{
            {
                invScale.x * (c1 * c3 + s1 * s2 * s3),
                invScale.x * (c2 * s3),
                invScale.x * (c1 * s2 * s3 - c3 * s1),
            },
            {
                invScale.y * (c3 * s1 * s2 - c1 * s3),
                invScale.y * (c2 * c3),
                invScale.y * (c1 * c3 * s2 + s1 * s3),
            },
            {
                invScale.z * (c2 * s1),
                invScale.z * (-s2),
                invScale.z * (c1 * c2),
            },
        };
    }

    void RenderObject::Create(std::unique_ptr<Device>& device)
    {
        for(auto& [name, child] : m_mChilds)
            child->Create(device);
    }

    void RenderObject::Render(float fDeltaTime, vk::CommandBuffer& commandBuffer) 
    {
        for(auto& [name, child] : m_mChilds)
            child->Render(fDeltaTime, commandBuffer);
    }

    void RenderObject::Update(float fDeltaTime, std::unique_ptr<SwapChain>& swapchain) 
    {
        for(auto& [name, child] : m_mChilds)
            child->Update(fDeltaTime, swapchain);
    }

    std::shared_ptr<RenderObject> RenderObject::Find(std::string srName)
    {
        auto it = m_mChilds.find(srName);
        if(it != m_mChilds.end())
            return it->second;
        
        for(auto& [name, child] : m_mChilds)
            child->Find(srName);

        return nullptr;
    }

    const glm::vec3 RenderObject::GetForwardVector() 
    { 
        float yaw = m_transform.rot.y;
        float pitch = m_transform.rot.x;
        return glm::vec3{sin(yaw), -pitch, cos(yaw)};
    }

    const glm::vec3 RenderObject::GetRightVector() 
    { 
        auto forward = GetForwardVector();
        return glm::vec3{forward.z, 0.f, -forward.x};
    }

    const glm::vec3 RenderObject::GetUpVector() 
    { 
        return glm::vec3{0.f, -1.f, 0.f};
    }

    void RenderObject::SetParent(std::shared_ptr<RenderObject> parent)
    {
        m_pParentOld = m_pParent;
        m_pParent = parent;
        //If you set parent for this, you should attach self to parent's child's
        m_pParent->AddChild(shared_from_this());
        if(m_pParentOld)
            m_pParentOld->Detach(shared_from_this());
    }

    void RenderObject::AddChild(std::shared_ptr<RenderObject> child)
    {
        m_mChilds.emplace(child->GetName(), child);
    }

    void RenderObject::Attach(std::shared_ptr<RenderObject> child)
    {
        child->SetParent(shared_from_this());
    }

    void RenderObject::Detach(std::shared_ptr<RenderObject> child)
    {
        auto it = m_mChilds.find(child->GetName());
        if(it != m_mChilds.end())
            m_mChilds.erase(it);
    }
}