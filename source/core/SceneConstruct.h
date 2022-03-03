#pragma once
#include "Resources/ResourceCunstruct.h"
#include "serializer/Serialization.hpp"

namespace Engine
{
    enum class ESceneObjectType
    {
        eCamera,
        eSkybox,
        eMeshComponent,
        eGltfMesh,
        eEnvironment,
        eLightSource
    };

    struct FSceneObject
    {
        std::string srName;
        ESceneObjectType eObjectType;
        std::string srUseVolume;
        Resources::FMeshCreateInfo mesh;
        Resources::FLightCreateinfo light; 
        Resources::FTextureCreateInfo texture;
        FTransform fTransform;
        std::vector<glm::vec4> vInstances;
        std::vector<FSceneObject> vSceneObjects;
    };

    struct FSceneCreateInfo
    {
        FSceneObject skybox;
        FSceneObject environment;
        std::vector<FSceneObject> vSceneObjects;
    };

    /*SCENE*/
    REGISTER_SERIALIZATION_BLOCK_H(FSceneObject);
    REGISTER_SERIALIZATION_BLOCK_H(FSceneCreateInfo);
}