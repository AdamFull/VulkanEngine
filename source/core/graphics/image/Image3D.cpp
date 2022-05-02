#include "Image3D.h"

using namespace Engine::Core;
using namespace Engine::Core::Noise;
using namespace Engine::Resources;

void CImage3D::loadNoise(ENoisePattern ePattern, uint32_t width, uint32_t height, uint32_t depth)
{
    vk::Format format;
    ktxTexture *texture;
    Loaders::CImageLoader::allocateRawDataAsKTXTexture(&texture, &format, width, height, depth, 3, 0x8229);

    initializeTexture(texture, format);

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

    Loaders::CImageLoader::close(&texture);
    updateDescriptor();
}

void CImage3D::generatePerlinNoise(ktxTexture *texture)
{
    vk::DeviceSize imgSize = _extent.width * _extent.height * _extent.depth;
    texture->pData = static_cast<unsigned char *>(calloc(imgSize, sizeof(unsigned char)));

    Noise::PerlinNoise<float> perlinNoise;

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

void CImage3D::generateFractalNoise(ktxTexture *texture, uint32_t octaves, float perceptation)
{
    vk::DeviceSize imgSize = _extent.width * _extent.height * _extent.depth;
    texture->pData = static_cast<unsigned char *>(calloc(imgSize, sizeof(unsigned char)));

    Noise::PerlinNoise<float> perlinNoise;
    Noise::FractalNoise<float> fractalNoise(perlinNoise, octaves, perceptation);

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