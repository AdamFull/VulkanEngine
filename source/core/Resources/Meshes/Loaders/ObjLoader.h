#pragma once

namespace Engine
{
    namespace Core
    {
        namespace Scene
        {
            namespace Objects
            {
                class RenderObject;
            }
        }
    }

    namespace Resources
    {
        namespace Loaders
        {
            struct ObjLoader
            {
            public:
                static void Load(std::string srPath, std::shared_ptr<Core::Scene::Objects::RenderObject> pRoot);
            };
        }
    }

}