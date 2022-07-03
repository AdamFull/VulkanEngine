#pragma once

namespace engine
{
    namespace core
    {
        namespace scene { class CRenderObject; }
    }

    namespace resources
    {
        namespace loaders
        {
            struct ObjLoader
            {
            public:
                static void Load(std::string srPath, utl::ref_ptr<core::scene::CRenderObject>& pRoot);
            };
        }
    }

}