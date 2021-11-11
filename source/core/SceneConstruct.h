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
        eVolume
    };

    struct FSceneObject
    {
        std::string srName;
        ESceneObjectType eObjectType;
        std::string srUseVolume;
        Resources::FMeshCreateInfo mesh;
        Objects::FTransform fTransform;
        std::vector<FSceneObject> vSceneObjects;
    };

    struct FSceneCreateInfo
    {
        FSceneObject skybox;
        std::vector<FSceneObject> vSceneObjects;
    };
}