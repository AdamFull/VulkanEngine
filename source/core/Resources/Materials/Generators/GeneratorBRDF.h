#pragma once
#include "GeneratorBase.h"

namespace Engine
{
    namespace Resources
    {
        namespace Texture
        {
            class TextureBase;
        }
        namespace Material
        {
            namespace Generator
            {
                class GeneratorBRDF : public GeneratorBase
                {
                public:
                    void Create() override;
                    void Generate(uint32_t indexCount, uint32_t firstIndex) override;
                protected:
                    inline Core::Pipeline::EShaderSet GetShaderSet() override { return Core::Pipeline::EShaderSet::eBRDF; }
                    void CreateDescriptors(uint32_t images) override;
                    void CreateTextures() override;
                };
            }
        }
    }
}