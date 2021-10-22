#pragma once

namespace Engine
{
    class RenderObject;
    namespace Loaders
    {
        namespace Model
        {
            struct ObjLoader
            {
                public:
                    static void Load(std::string srPath, std::shared_ptr<RenderObject> pRoot);
            };
        }
    }
}