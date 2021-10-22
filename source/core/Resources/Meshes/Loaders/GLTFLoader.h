#pragma once

namespace Engine
{
    class RenderObject;
    class ResourceManager;
    class GLTFSceneObjectComponent;
    namespace Loaders
    {
        namespace Model
        {
            struct GLTFLoader
            {
            public:
                static void Load(std::string srPath, std::shared_ptr<ResourceManager> pResMgr, std::shared_ptr<RenderObject> pRoot);
            private:
                static void LoadNode(std::shared_ptr<GLTFSceneObjectComponent> pParent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale);
                static void LoadMaterials(std::shared_ptr<ResourceManager> pResMgr, const tinygltf::Model &model);
                static void LoadTextures(std::shared_ptr<ResourceManager> pResMgr, const tinygltf::Model &model);
            };
        }
    }
}