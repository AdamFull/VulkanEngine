#pragma once
#include "Objects/Transform.h"
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
        Objects::FTransform fTransform;
        std::vector<FSceneObject> vSceneObjects;
    };

    struct FSceneCreateInfo
    {
        FSceneObject skybox;
        FSceneObject environment;
        std::vector<FSceneObject> vSceneObjects;
    };
}