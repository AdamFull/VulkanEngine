#include "MeshVolumeComponent.h"
#include "Resources/Materials/Generators/GeneratorBRDF.h"
#include "Resources/Materials/Generators/GeneratorIrradiate.h"
#include "Resources/Materials/Generators/GeneratorPrefiltred.h"
#include "Resources/Textures/VulkanTexture.h"
#include "Resources/ResourceManager.h"

using namespace Engine::Objects::Components;
using namespace Engine::Resources;
using namespace Engine::Resources::Material::Generator;

void MeshVolumeComponent::Create(std::shared_ptr<Resources::ResourceManager> resourceMgr)
{
    /*if(m_pMesh)
    {
        auto primitive = m_pMesh->GetPrimitive(0);

        auto pGeneratorBRDF = std::make_shared<GeneratorBRDF>();
        pGeneratorBRDF->Create();
        pGeneratorBRDF->Generate(primitive.indexCount, primitive.firstIndex);
        resourceMgr->AddExisting(GetName() + "_brdf", pGeneratorBRDF->Get());
        pGeneratorBRDF->Destroy();

        auto pGeneratorIrradiate = std::make_shared<GeneratorIrradiate>();
        pGeneratorIrradiate->AddTexture(ETextureAttachmentType::eCubemap, primitive.material->GetTexture(ETextureAttachmentType::eCubemap));
        pGeneratorIrradiate->Create();
        pGeneratorIrradiate->Generate(primitive.indexCount, primitive.firstIndex);
        resourceMgr->AddExisting(GetName() + "_irradiate_cube", pGeneratorIrradiate->Get());
        pGeneratorIrradiate->Destroy();

        auto pGeneratorPrefiltred = std::make_shared<GeneratorIrradiate>();
        pGeneratorPrefiltred->AddTexture(ETextureAttachmentType::eCubemap, primitive.material->GetTexture(ETextureAttachmentType::eCubemap));
        pGeneratorPrefiltred->Create();
        pGeneratorPrefiltred->Generate(primitive.indexCount, primitive.firstIndex);
        resourceMgr->AddExisting("_prefiltred_cube", pGeneratorPrefiltred->Get());
        pGeneratorPrefiltred->Destroy();
    }
*/
    MeshComponentBase::Create(resourceMgr);
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