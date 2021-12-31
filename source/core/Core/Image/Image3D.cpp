#include "Image3D.h"

using namespace Engine::Core;
using namespace Engine::Core::Noise;
using namespace Engine::Resources;

void Image3D::LoadNoise(ENoisePattern ePattern, uint32_t width, uint32_t height, uint32_t depth)
{
    vk::Format format;
    ktxTexture *texture;
    Loaders::ImageLoader::AllocateRawDataAsKTXTexture(&texture, &format, width, height, depth, 3, 0x8229);

    InitializeTexture(texture, format);

    switch (ePattern)
    {
    case ENoisePattern::ePerlin:
        GeneratePerlinNoise(texture);
        break;
    case ENoisePattern::eFractal:
        GenerateFractalNoise(texture, 6, 0.5);
        break;
    }

    WriteImageData(texture, format);

    Loaders::ImageLoader::Close(&texture);
    UpdateDescriptor();
}

void Image3D::GeneratePerlinNoise(ktxTexture *texture)
{
    vk::DeviceSize imgSize = m_extent.width * m_extent.height * m_extent.depth;
    texture->pData = static_cast<unsigned char *>(calloc(imgSize, sizeof(unsigned char)));

    Noise::PerlinNoise<float> perlinNoise;

#pragma omp parallel for
    for (int32_t z = 0; z < m_extent.depth; z++)
    {
        for (int32_t y = 0; y < m_extent.height; y++)
        {
            for (int32_t x = 0; x < m_extent.width; x++)
            {
                float nx = (float)x / (float)m_extent.width;
                float ny = (float)y / (float)m_extent.height;
                float nz = (float)z / (float)m_extent.depth;

                float n = 20.0 * perlinNoise.noise(nx, ny, nz);

                n = n - floor(n);

                texture->pData[x + y * m_extent.width + z * m_extent.width * m_extent.height] = static_cast<uint8_t>(floor(n * 255));
            }
        }
    }
}

void Image3D::GenerateFractalNoise(ktxTexture *texture, uint32_t octaves, float perceptation)
{
    vk::DeviceSize imgSize = m_extent.width * m_extent.height * m_extent.depth;
    texture->pData = static_cast<unsigned char *>(calloc(imgSize, sizeof(unsigned char)));

    Noise::PerlinNoise<float> perlinNoise;
    Noise::FractalNoise<float> fractalNoise(perlinNoise, octaves, perceptation);

    const float noiseScale = static_cast<float>(rand() % 10) + 4.0f;

#pragma omp parallel for
    for (int32_t z = 0; z < m_extent.depth; z++)
    {
        for (int32_t y = 0; y < m_extent.height; y++)
        {
            for (int32_t x = 0; x < m_extent.width; x++)
            {
                float nx = (float)x / (float)m_extent.width;
                float ny = (float)y / (float)m_extent.height;
                float nz = (float)z / (float)m_extent.depth;

                float n = fractalNoise.noise(nx * noiseScale, ny * noiseScale, nz * noiseScale);

                n = n - floor(n);

                texture->pData[x + y * m_extent.width + z * m_extent.width * m_extent.height] = static_cast<uint8_t>(floor(n * 255));
            }
        }
    }
}
