#pragma once
#include "external/gltf/tiny_gltf.h"

namespace Engine
{
    class TextureBase;
    class MaterialBase;
    class RenderObject;
    class ResourceManager;
    namespace Loaders
    {
        namespace Model
        {
            struct GLTFLoader
            {
            public:
                struct LoaderTemporaryObject
                {
                    bool bLoadTextures;
                    bool bLoadMaterials;
                    std::string srModelName;
                    std::vector<std::shared_ptr<TextureBase>> vTextures;
                    std::vector<std::shared_ptr<MaterialBase>> vMaterials;
                };

                static std::shared_ptr<RenderObject> Load(std::string srPath, std::string srName, std::shared_ptr<LoaderTemporaryObject> pTmp, std::shared_ptr<ResourceManager> pResMgr);
            private:
                static void LoadNode(std::shared_ptr<LoaderTemporaryObject> tmp, std::shared_ptr<RenderObject> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale);
                static void LoadMaterials(std::shared_ptr<LoaderTemporaryObject> tmp, std::shared_ptr<ResourceManager> pResMgr, const tinygltf::Model &model);
                static void LoadTextures(std::shared_ptr<LoaderTemporaryObject> tmp, std::shared_ptr<ResourceManager> pResMgr, const tinygltf::Model &model);
                static std::shared_ptr<TextureBase> LoadTexture(const tinygltf::Image &model, std::string path);
                static void LoadSkins(const tinygltf::Model &model);

                static uint32_t current_primitive;
            };
        }
    }
}