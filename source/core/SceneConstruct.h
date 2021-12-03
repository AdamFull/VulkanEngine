#pragma once
#include "Resources/ResourceCunstruct.h"

namespace Engine
{
    enum class ESceneObjectType
    {
        eCamera,
        eSkybox,
        eMeshComponent,
        eGltfMesh,
        eEnvironment
    };

    struct FSceneObject
    {
        std::string srName;
        ESceneObjectType eObjectType;
        std::string srUseVolume;
        Resources::FMeshCreateInfo mesh;
        Resources::FTextureCreateInfo texture;
        FTransform fTransform;
        std::vector<FSceneObject> vSceneObjects;
    };

    struct FSceneCreateInfo
    {
        FSceneObject skybox;
        FSceneObject environment;
        std::vector<FSceneObject> vSceneObjects;
    };
}