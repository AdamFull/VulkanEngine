#pragma once
#include "Objects/RenderObject.h"

namespace Engine
{
    enum class ESceneObjectType
    {
        eCamera,
        eSkybox,
        eMeshComponent
    };

    struct FSceneObjectResourceBindings
    {
        std::vector<std::string> vTextures;
        std::vector<std::string> vMaterials;
        std::vector<std::string> vMeshes;
    };

    struct FSceneObject
    {
        std::string srName;
        ESceneObjectType eObjectType;
        std::vector<FSceneObjectResourceBindings> vResourceBindings;
        FTransform fTransform;
        std::vector<FSceneObject> vSceneObjects;
    };

    struct FSceneCreateInfo
    {
        std::string resources_path;
        std::vector<FSceneObject> vSceneObjects;
    };
}