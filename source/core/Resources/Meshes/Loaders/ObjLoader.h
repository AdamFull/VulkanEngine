#pragma once

namespace Engine
{
    namespace Core
    {
        namespace Scene { class CRenderObject; }
    }

    namespace Resources
    {
        namespace Loaders
        {
            struct ObjLoader
            {
            public:
                static void Load(std::string srPath, ref_ptr<Core::Scene::CRenderObject>& pRoot);
            };
        }
    }

}