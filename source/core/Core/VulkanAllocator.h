#pragma once
#include "VulkanHighLevel.h"

#include "Resources/Textures/VulkanTexture.h"
#include "Resources/Textures/Texture2D.h"
#include "Resources/Textures/Texture2DArray.h"
#include "Resources/Textures/Texture3D.h"
#include "Resources/Textures/TextureCubemap.h"

#include "Resources/Materials/VulkanMaterial.h"
#include "Resources/Materials/MaterialUI.h"
#include "Resources/Materials/MaterialSkybox.h"
#include "Resources/Materials/MaterialDiffuse.h"
#include "Resources/Materials/MaterialDeferred.h"

#include "Resources/Materials/Generators/GeneratorBase.h"
#include "Resources/Materials/Generators/GeneratorBRDF.h"
#include "Resources/Materials/Generators/GeneratorIrradiate.h"
#include "Resources/Materials/Generators/GeneratorPrefiltred.h"

#include "Resources/Lights/LightSourceBase.h"

#include "Resources/Meshes/VulkanMesh.h"

#include "VulkanBuffer.h"
#include "VulkanUniform.h"
#include "VulkanVBO.h"

#include "Pipeline/VulkanPipeline.h"
#include "Pipeline/GraphicsPipeline.h"

#include "Descriptor/DescriptorPool.h"
#include "Descriptor/DescriptorSet.h"
#include "Descriptor/DescriptorSetContainer.h"
#include "Descriptor/DescriptorSetLayout.h"
#include "Descriptor/DescriptorWriter.h"

namespace Engine
{
    namespace Core
    {
        struct FDefaultAllocator
        {
            template<class T>
            static std::unique_ptr<T> Allocate() 
            { 
                assert(false && "Specialisation was not found"); 
                return std::unique_ptr<T>(nullptr);
            }

            template<> static std::unique_ptr<Resources::Texture::TextureBase> Allocate() { return std::make_unique<Resources::Texture::TextureBase>(UDevice); }
            template<> static std::unique_ptr<Resources::Texture::Texture2D> Allocate() { return std::make_unique<Resources::Texture::Texture2D>(UDevice); }
            template<> static std::unique_ptr<Resources::Texture::Texture2DArray> Allocate() { return std::make_unique<Resources::Texture::Texture2DArray>(UDevice); }
            template<> static std::unique_ptr<Resources::Texture::Texture3D> Allocate() { return std::make_unique<Resources::Texture::Texture3D>(UDevice); }
            template<> static std::unique_ptr<Resources::Texture::TextureCubemap> Allocate() { return std::make_unique<Resources::Texture::TextureCubemap>(UDevice); }

            template<> static std::unique_ptr<Resources::Material::MaterialBase> Allocate() { return std::make_unique<Resources::Material::MaterialBase>(UDevice, USwapChain); }
            template<> static std::unique_ptr<Resources::Material::MaterialUI> Allocate() { return std::make_unique<Resources::Material::MaterialUI>(UDevice, USwapChain); }
            template<> static std::unique_ptr<Resources::Material::MaterialSkybox> Allocate() { return std::make_unique<Resources::Material::MaterialSkybox>(UDevice, USwapChain); }
            template<> static std::unique_ptr<Resources::Material::MaterialDiffuse> Allocate() { return std::make_unique<Resources::Material::MaterialDiffuse>(UDevice, USwapChain); }
            template<> static std::unique_ptr<Resources::Material::MaterialDeferred> Allocate() { return std::make_unique<Resources::Material::MaterialDeferred>(UDevice, USwapChain); }

            template<> static std::unique_ptr<Resources::Material::Generator::GeneratorBase> Allocate() { return std::make_unique<Resources::Material::Generator::GeneratorBase>(UDevice, USwapChain); }
            template<> static std::unique_ptr<Resources::Material::Generator::GeneratorBRDF> Allocate() { return std::make_unique<Resources::Material::Generator::GeneratorBRDF>(UDevice, USwapChain); }
            template<> static std::unique_ptr<Resources::Material::Generator::GeneratorIrradiate> Allocate() { return std::make_unique<Resources::Material::Generator::GeneratorIrradiate>(UDevice, USwapChain); }
            template<> static std::unique_ptr<Resources::Material::Generator::GeneratorPrefiltred> Allocate() { return std::make_unique<Resources::Material::Generator::GeneratorPrefiltred>(UDevice, USwapChain); }

            template<> static std::unique_ptr<VulkanBuffer> Allocate() { return std::make_unique<VulkanBuffer>(UDevice); }
            template<> static std::unique_ptr<UniformBuffer> Allocate() { return std::make_unique<UniformBuffer>(UDevice); }
            template<> static std::unique_ptr<VulkanVBO> Allocate() { return std::make_unique<VulkanVBO>(UDevice); }

            template<> static std::unique_ptr<Pipeline::GraphicsPipeline> Allocate() { return std::make_unique<Pipeline::GraphicsPipeline>(UDevice, USwapChain); }

            template<> static std::unique_ptr<Descriptor::VulkanDescriptorPool> Allocate() { return std::make_unique<Descriptor::VulkanDescriptorPool>(UDevice); }
            template<> static std::unique_ptr<Descriptor::VulkanDescriptorSet> Allocate() { return std::make_unique<Descriptor::VulkanDescriptorSet>(UDevice); }
            template<> static std::unique_ptr<Descriptor::VulkanDescriptorSetLayout> Allocate() { return std::make_unique<Descriptor::VulkanDescriptorSetLayout>(UDevice); }
        };
    }
}