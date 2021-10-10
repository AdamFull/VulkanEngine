#include "ResourceManager.h"
#include "filesystem/FilesystemHelper.h"
#include "serializer/JsonSerializer.h"

namespace Engine
{
    std::unique_ptr<ResourceManager> ResourceManager::m_pInstance{nullptr};

    std::unique_ptr<ResourceManager>& ResourceManager::GetInstance()
    {
        if(!m_pInstance)
        {
            m_pInstance.reset(new ResourceManager());
        }
        return m_pInstance;
    }

    void ResourceManager::Load(std::string srResourcesPath)
    {
        auto input = FilesystemHelper::ReadFile(srResourcesPath);
        auto res_json = nlohmann::json::parse(input).front();

        std::vector<Engine::FTextureCreateInfo> vTextures;
        std::vector<Engine::FMaterialCreateInfo> vMaterials;
        std::vector<Engine::FMeshCreateInfo> vMeshes;

        res_json.at("textures").get_to(vTextures);
        res_json.at("materials").get_to(vMaterials);
        res_json.at("meshes").get_to(vMeshes);

        for(auto texture : vTextures)
        {
            m_mTextures.emplace
        }
    }

    void ResourceManager::AddResource(std::string srResourceName, std::shared_ptr<ResourceBase> pResource)
    {
        auto it = m_mResources.find(srResourceName);
        if(it != m_mResources.end())
            assert(false && "Resource named: is already exists.");
        m_mResources.emplace(srResourceName, pResource);
    }

    std::shared_ptr<ResourceBase> ResourceManager::GetResource(std::string srResourceName)
    {
        auto it = m_mResources.find(srResourceName);
        if(it != m_mResources.end())
            return it->second;
        return nullptr;
    }

    void ResourceManager::Destroy(std::string srResourceName)
    {
        auto it = m_mResources.find(srResourceName);
        if(it != m_mResources.end())
            it->second->Destroy();
        assert(false && "Cannot find resource named: .");
    }

    void ResourceManager::DestroyAll()
    {
        for(auto& [key, value] : m_mResources)
        {
            value->Destroy();
        }
    }
}