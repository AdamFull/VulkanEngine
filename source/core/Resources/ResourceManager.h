#pragma once
#include "ResourceFactory.h"

namespace Engine
{
    class ResourceManager
    {
    protected:
        ResourceManager() = default;
        static std::unique_ptr<ResourceManager> m_pInstance;
    public:
        ResourceManager(const ResourceManager&) = delete;
        void operator=(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

        static std::unique_ptr<ResourceManager>& GetInstance();

        void Load(std::string srResourcesPath);

        void AddResource(std::string srResourceName, std::shared_ptr<ResourceBase> pResource);

        std::shared_ptr<ResourceBase> GetResource(std::string srResourceName);

        void Destroy(std::string srResourceName);
        void DestroyAll();
    private:
        std::map<std::string, std::shared_ptr<ResourceBase>> m_mResources;
    };
}