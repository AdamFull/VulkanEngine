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
                FTransform fTransform;
                FCullingInfo culling;
                std::vector<std::variant<
                    resources::FSkyboxCreateInfo,
                    resources::FCameraCreateInfo,
                    resources::FMeshCreateInfo, 
                    resources::FLightCreateinfo,
                    resources::FStriptsCreateInfo>> components;
                std::vector<FSceneObject> vSceneObjects;
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