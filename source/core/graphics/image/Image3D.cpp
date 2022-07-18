#include "Image3D.h"

using namespace engine::core;
using namespace engine::core::noise;
using namespace engine::resources;

CImage3D::CImage3D(resources::ENoisePattern ePattern, const vk::Extent3D& extent)
{
    loadNoise(ePattern, extent);
}

void CImage3D::loadNoise(resources::ENoisePattern ePattern, const vk::Extent3D& extent)
{
    utl::scope_ptr<FImageCreateInfo> texture = utl::make_scope<FImageCreateInfo>();
    texture->baseWidth = extent.width;
    texture->baseHeight = extent.height;
    texture->baseDepth = extent.depth;
    texture->numDimensions = 3;
    texture->isArray = false;
    texture->numLayers = 1;
    texture->numFaces = 1;
    texture->dataSize = extent.width * extent.height * (extent.depth > 1 ? extent.depth : 4);

    auto format = vk::Format::eR8Unorm;

    //TODO: fix that
    //initializeTexture(texture, format);

    switch (ePattern)
    {
    case ENoisePattern::ePerlin:
        generatePerlinNoise(texture);
        break;
    case ENoisePattern::eFractal:
        generateFractalNoise(texture, 6, 0.5);
        break;
    }

    writeImageData(texture, format);
    updateDescriptor();
}

void CImage3D::generatePerlinNoise(utl::scope_ptr<FImageCreateInfo>& texture)
{
    vk::DeviceSize imgSize = _extent.width * _extent.height * _extent.depth;
    //texture->pData.get() = static_cast<unsigned char *>(calloc(imgSize, sizeof(unsigned char)));

    PerlinNoise<float> perlinNoise;

#pragma omp parallel for
    for (int32_t z = 0; z < _extent.depth; z++)
    {
        for (int32_t y = 0; y < _extent.height; y++)
        {
            for (int32_t x = 0; x < _extent.width; x++)
            {
                float nx = (float)x / (float)_extent.width;
                float ny = (float)y / (float)_extent.height;
                float nz = (float)z / (float)_extent.depth;

                float n = 20.0 * perlinNoise.noise(nx, ny, nz);

                n = n - floor(n);

                texture->pData[x + y * _extent.width + z * _extent.width * _extent.height] = static_cast<uint8_t>(floor(n * 255));
            }
        }
    }
}

void CImage3D::generateFractalNoise(utl::scope_ptr<FImageCreateInfo>& texture, uint32_t octaves, float perceptation)
{
    vk::DeviceSize imgSize = _extent.width * _extent.height * _extent.depth;
    //texture->pData.get() = static_cast<unsigned char *>(calloc(imgSize, sizeof(unsigned char)));

    PerlinNoise<float> perlinNoise;
    FractalNoise<float> fractalNoise(perlinNoise, octaves, perceptation);

    const float noiseScale = static_cast<float>(rand() % 10) + 4.0f;

#pragma omp parallel for
    for (int32_t z = 0; z < _extent.depth; z++)
    {
        for (int32_t y = 0; y < _extent.height; y++)
        {
            for (int32_t x = 0; x < _extent.width; x++)
            {
                float nx = (float)x / (float)_extent.width;
                float ny = (float)y / (float)_extent.height;
                float nz = (float)z / (float)_extent.depth;

                float n = fractalNoise.noise(nx * noiseScale, ny * noiseScale, nz * noiseScale);

                n = n - floor(n);

                texture->pData[x + y * _extent.width + z * _extent.width * _extent.height] = static_cast<uint8_t>(floor(n * 255));
            }
        }
    }
}
