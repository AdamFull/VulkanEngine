#pragma once

namespace Engine
{
    namespace Objects
    {
        class RenderObject;
    }

    namespace Resources
    {
        namespace Loaders
        {
            struct ObjLoader
            {
            public:
                static void Load(std::string srPath, std::shared_ptr<Objects::RenderObject> pRoot);
            };
        }
    }

}