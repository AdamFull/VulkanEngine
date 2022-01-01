#pragma once
#include "GeneratorBase.h"

namespace Engine
{
    namespace Resources
    {
        namespace Material
        {
            namespace Generator
            {
                class GeneratorBRDF : public GeneratorBase
                {
                public:
                    GeneratorBRDF() = default;

                    void Create(std::shared_ptr<ResourceManager> pResMgr) override;
                    void Generate(std::shared_ptr<Mesh::MeshBase> pMesh) override;
                protected:
                    void CreateTextures() override;
                };
            }
        }
    }
}