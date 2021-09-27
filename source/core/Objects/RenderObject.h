#pragma once

namespace Engine
{
    class Device;

    struct FTransform
    {
        glm::vec3 pos{};
        glm::vec3 rot{};
        glm::vec3 scale{1.f, 1.f, 1.f};

        glm::mat4 GetModel();
        glm::mat3 GetNormal();
    };

    class RenderObject : public std::enable_shared_from_this<RenderObject>
    {
    public:
        virtual ~RenderObject() {}

        virtual void Create(std::unique_ptr<Device>& device);
        virtual void Render(float fDeltaTime, vk::CommandBuffer& commandBuffer);
        virtual void Update(float fDeltaTime);

        virtual const glm::vec3 GetForwardVector();
        virtual const glm::vec3 GetRightVector();
        virtual const glm::vec3 GetUpVector();
        virtual inline const FTransform GetTransform() { return m_transform; }
        virtual inline void SetTransform(FTransform transformNew) { m_transform = transformNew; }

        virtual void SetParent(std::shared_ptr<RenderObject> parent);
        virtual void Attach(std::shared_ptr<RenderObject> child);
        virtual void Detach(std::shared_ptr<RenderObject> child);

        virtual inline std::shared_ptr<RenderObject>& GetParent() { return m_pParent; }
        virtual inline std::vector<std::shared_ptr<RenderObject>>& GetChilds() { return m_vChilds; }
    protected:
        virtual void AddChild(std::shared_ptr<RenderObject> child);
        FTransform m_transform;
    private:
        std::shared_ptr<RenderObject> m_pParent;
        std::shared_ptr<RenderObject> m_pParentOld;
        std::vector<std::shared_ptr<RenderObject>> m_vChilds;
    };
}