#pragma once
#include "SceneConstruct.h"
#include "serializer/JsonSerializer.h"

namespace Engine
{
    class RenderScene;
    class RenderObject;

    struct SceneFactory
    {
        using object_creator_t = std::unique_ptr<RenderObject>(FSceneObject);
        static std::unique_ptr<RenderScene> Create(std::string srScenePath);
    private:
        static std::map<ESceneObjectType, std::function<object_creator_t>> m_mFactories;

        static void CreateComponents(std::shared_ptr<RenderObject> pRoot, std::vector<FSceneObject> sceneObjects);
        static std::shared_ptr<RenderObject> CreateComponent(FSceneObject info);
    };
}