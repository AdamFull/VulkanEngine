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
        eGltfMesh
    };

    struct FSceneObject
    {
        std::string srName;
        ESceneObjectType eObjectType;
        FMeshCreateInfo mesh;
        FTransform fTransform;
        std::vector<FSceneObject> vSceneObjects;
    };

    struct FSceneCreateInfo
    {
        FSceneObject skybox;
        std::vector<FSceneObject> vSceneObjects;
    };
}