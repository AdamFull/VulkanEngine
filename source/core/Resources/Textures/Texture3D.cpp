#include "Texture3D.h"
#include "Noise/PerlinNoise.hpp"
#include "Noise/FractalNoise.hpp"

using namespace Engine::Resources::Texture;

Texture3D::Texture3D(std::shared_ptr<Core::Device> device)
{
    m_device = device;
}

void Texture3D::ReCreate()
{
    TextureBase::ReCreate();
}

void Texture3D::Update(uint32_t imageIndex)
{
    TextureBase::Update(imageIndex);
}

void Texture3D::Bind(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
{
    TextureBase::Bind(commandBuffer, imageIndex);
}

void Texture3D::Cleanup()
{
    TextureBase::Cleanup();
}

void Texture3D::Destroy()
{
    TextureBase::Destroy();
}

void Texture3D::LoadNoise(ENoisePattern ePattern, uint32_t width, uint32_t height, uint32_t depth)
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

void Texture3D::GeneratePerlinNoise(ktxTexture *texture)
{
    vk::DeviceSize imgSize = fParams.width * fParams.height * fParams.depth;
    texture->pData = static_cast<unsigned char *>(calloc(imgSize, sizeof(unsigned char)));

    Noise::PerlinNoise<float> perlinNoise;

#pragma omp parallel for
    for (int32_t z = 0; z < fParams.depth; z++)
    {
        for (int32_t y = 0; y < fParams.height; y++)
        {
            for (int32_t x = 0; x < fParams.width; x++)
            {
                float nx = (float)x / (float)fParams.width;
                float ny = (float)y / (float)fParams.height;
                float nz = (float)z / (float)fParams.depth;

                float n = 20.0 * perlinNoise.noise(nx, ny, nz);

                n = n - floor(n);

                texture->pData[x + y * fParams.width + z * fParams.width * fParams.height] = static_cast<uint8_t>(floor(n * 255));
            }
        }
    }
}

void Texture3D::GenerateFractalNoise(ktxTexture *texture, uint32_t octaves, float perceptation)
{
    vk::DeviceSize imgSize = fParams.width * fParams.height * fParams.depth;
    texture->pData = static_cast<unsigned char *>(calloc(imgSize, sizeof(unsigned char)));

    Noise::PerlinNoise<float> perlinNoise;
    Noise::FractalNoise<float> fractalNoise(perlinNoise, octaves, perceptation);

    const float noiseScale = static_cast<float>(rand() % 10) + 4.0f;

#pragma omp parallel for
    for (int32_t z = 0; z < fParams.depth; z++)
    {
        for (int32_t y = 0; y < fParams.height; y++)
        {
            for (int32_t x = 0; x < fParams.width; x++)
            {
                float nx = (float)x / (float)fParams.width;
                float ny = (float)y / (float)fParams.height;
                float nz = (float)z / (float)fParams.depth;

                float n = fractalNoise.noise(nx * noiseScale, ny * noiseScale, nz * noiseScale);

                n = n - floor(n);

                texture->pData[x + y * fParams.width + z * fParams.width * fParams.height] = static_cast<uint8_t>(floor(n * 255));
            }
        }
    }
}
