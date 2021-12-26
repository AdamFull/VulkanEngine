#pragma once

namespace Engine
{
    namespace Resources
    {
        namespace Mesh { class MeshFragment; }
        namespace Loaders
        {
            class GLTFSceneNode;
            struct Skin
            {
                std::string name;
                std::shared_ptr<GLTFSceneNode> skeletonRoot = nullptr;
                std::vector<glm::mat4> inverseBindMatrices;
                std::vector<std::shared_ptr<GLTFSceneNode>> joints;
            };

            class GLTFSceneNode : public DynamicNode<GLTFSceneNode>
            {
            public:
                uint32_t m_index;
		
                std::shared_ptr<Mesh::MeshFragment> m_pMesh;
                std::unique_ptr<Skin> m_pSkin;
            };
        }
    }
}