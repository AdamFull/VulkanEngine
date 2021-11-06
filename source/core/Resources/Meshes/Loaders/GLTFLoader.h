#pragma once
#include "external/gltf/tiny_gltf.h"

namespace Engine
{
    namespace Objects
    {
        class RenderObject;
    }
    namespace Resources
    {
        class ResourceManager;

        namespace Texture { class TextureBase; }
        namespace Material { class MaterialBase; }

        namespace Loaders
        {
            struct GLTFLoader
            {
            public:
                struct LoaderTemporaryObject
                {
                    bool bLoadTextures;
                    bool bLoadMaterials;
                    std::string srModelName;
                    std::string srVolumeName;
                    std::vector<std::shared_ptr<Texture::TextureBase>> vTextures;
                    std::vector<std::shared_ptr<Material::MaterialBase>> vMaterials;
                };

                static void Load(std::string srPath, std::string srName, std::shared_ptr<LoaderTemporaryObject> pTmp, std::shared_ptr<Objects::RenderObject> pRootComponent, std::shared_ptr<Resources::ResourceManager> pResMgr);

            private:
                static void LoadNode(std::shared_ptr<LoaderTemporaryObject> tmp, std::shared_ptr<Resources::ResourceManager> pResMgr, std::shared_ptr<Objects::RenderObject> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale);
                static void LoadMaterials(std::shared_ptr<LoaderTemporaryObject> tmp, std::shared_ptr<Resources::ResourceManager> pResMgr, const tinygltf::Model &model);
                static void LoadTextures(std::shared_ptr<LoaderTemporaryObject> tmp, std::shared_ptr<Resources::ResourceManager> pResMgr, const tinygltf::Model &model);
                static std::shared_ptr<Texture::TextureBase> LoadTexture(const tinygltf::Image &model, std::string path);
                static void LoadSkins(const tinygltf::Model &model);

                static uint32_t current_primitive;
            };
        }
    }
}