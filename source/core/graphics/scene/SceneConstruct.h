#pragma once
#include "util/Transform.hpp"
#include "resources/ResourceCunstruct.h"
#include "serializer/Serialization.hpp"

namespace engine
{
    namespace core
    {
        namespace scene
        {
            enum class ESceneObjectType
            {
                eCamera,
                eSkybox,
                eMeshComponent,
                eGltfMesh,
                eLightSource
            };

            enum class ECullingType
            {
                eByPoint,
                eBySphere,
                eByBox
            };

            struct FCullingInfo
            {
                ECullingType eType{ECullingType::eBySphere};
                bool bEnableCulling{true};
                float fSphereRadius{10.f};
                // Add bounding box support
            };

            struct FSceneObject
            {
                std::string srName;
                ESceneObjectType eObjectType;
                std::string srUseVolume;
                resources::FMeshCreateInfo mesh;
                resources::FLightCreateinfo light;
                resources::FTextureCreateInfo texture;
                FTransform fTransform;
                std::vector<glm::vec4> vInstances;
                std::vector<FSceneObject> vSceneObjects;
                FCullingInfo culling;
            };

            struct FSceneCreateInfo
            {
                FSceneObject skybox;
                std::vector<FSceneObject> vSceneObjects;
            };

            /*SCENE*/
            REGISTER_SERIALIZATION_BLOCK_H(FCullingInfo);
            REGISTER_SERIALIZATION_BLOCK_H(FSceneObject);
            REGISTER_SERIALIZATION_BLOCK_H(FSceneCreateInfo);
        }
    }
}