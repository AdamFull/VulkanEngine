#include "MeshVolumeComponent.h"
#include "Core/VulkanHighLevel.h"
#include "Resources/ResourceManager.h"
#include "Resources/Materials/Generators/GeneratorBRDF.h"
#include "Resources/Materials/Generators/GeneratorIrradiate.h"
#include "Resources/Materials/Generators/GeneratorPrefiltred.h"

using namespace Engine::Objects::Components;
using namespace Engine::Resources;
using namespace Engine::Resources::Material::Generator;

void MeshVolumeComponent::Create(std::shared_ptr<Resources::ResourceManager> resourceMgr)
{
    MeshComponentBase::Create(resourceMgr);

    if(m_pMesh)
    {
        auto pGeneratorBRDF = std::make_shared<GeneratorBRDF>();
        pGeneratorBRDF->Create(resourceMgr);
        pGeneratorBRDF->Generate(m_pMesh);
        resourceMgr->AddExisting(GetName() + "_brdf", pGeneratorBRDF->Get());
        //pGeneratorBRDF->Destroy();

        auto pGeneratorIrradiate = std::make_shared<GeneratorIrradiate>();
        pGeneratorIrradiate->AddTexture(ETextureAttachmentType::eCubemap, m_pTexture);
        pGeneratorIrradiate->Create(resourceMgr);
        pGeneratorIrradiate->Generate(m_pMesh);
        resourceMgr->AddExisting(GetName() + "_irradiate_cube", pGeneratorIrradiate->Get());
        //pGeneratorIrradiate->Destroy();

        auto pGeneratorPrefiltred = std::make_shared<GeneratorPrefiltred>();
        pGeneratorPrefiltred->AddTexture(ETextureAttachmentType::eCubemap, m_pTexture);
        pGeneratorPrefiltred->Create(resourceMgr);
        pGeneratorPrefiltred->Generate(m_pMesh);
        resourceMgr->AddExisting(GetName() + "_prefiltred_cube", pGeneratorPrefiltred->Get());
        //pGeneratorPrefiltred->Destroy();
    }
}

void MeshVolumeComponent::ReCreate()
{
    MeshComponentBase::ReCreate();
}

void MeshVolumeComponent::Update(float fDeltaTime)
{
    MeshComponentBase::Update(fDeltaTime);
}

void MeshVolumeComponent::Render(vk::CommandBuffer &commandBuffer, uint32_t imageIndex)
{
    MeshComponentBase::Render(commandBuffer, imageIndex);
}

void MeshVolumeComponent::Cleanup()
{
    MeshComponentBase::Cleanup();
}

void MeshVolumeComponent::Destroy()
{
    MeshComponentBase::Destroy();
}