#include "SSAOPass.h"
#include "graphics/VulkanHighLevel.h"
#include "resources/materials/MaterialLoader.h"
#include "graphics/scene/objects/components/CameraManager.h"
#include <cmath>

using namespace engine::core::render;
using namespace engine::core::scene;
using namespace engine::resources;
using namespace engine::resources::material;

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}  

void CSSAOPass::create()
{
    std::default_random_engine rndEngine(0); //std::chrono::steady_clock::now().time_since_epoch().count()
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

    for (uint32_t i = 0; i < SSAO_KERNEL_SIZE; ++i)
	{
		glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
		sample = glm::normalize(sample);
		sample *= rndDist(rndEngine);
		float scale = float(i) / float(SSAO_KERNEL_SIZE);
		scale = lerp(0.1f, 1.0f, scale * scale);
		vSSAOKernel[i] = glm::vec4(sample * scale, 0.0f);
	}

    std::array<glm::vec4, SSAO_NOISE_DIM * SSAO_NOISE_DIM> vSSAONoise;
    for (uint32_t i = 0; i < static_cast<uint32_t>(vSSAONoise.size()); i++)
		vSSAONoise[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);

    pNoise = utl::make_ref<CImage>();
    pNoise->create(vSSAONoise.data(), vk::Extent3D{SSAO_NOISE_DIM, SSAO_NOISE_DIM, sizeof(glm::vec4)}, vk::Format::eR32G32B32A32Sfloat);

	pMaterial = CMaterialLoader::inst()->create("ssao_pass");
	pMaterial->addDefinition("SSAO_KERNEL_SIZE", std::to_string(SSAO_KERNEL_SIZE));
	pMaterial->addDefinition("SSAO_RADIUS", std::to_string(SSAO_RADIUS));
    pMaterial->create();

    CSubpass::create();
}

void CSSAOPass::reCreate()
{
	CSubpass::reCreate();
}

void CSSAOPass::render(vk::CommandBuffer &commandBuffer)
{
	CRenderSystem::inst()->setStageType(EStageType::eDeferred);
    auto& images = CRenderSystem::inst()->getCurrentImages();

	pMaterial->addTexture("packed_tex", images["packed_tex"]);
    pMaterial->addTexture("depth_tex", images["depth_tex"]);
	pMaterial->addTexture("ssao_noise_tex", pNoise);

	auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();
	auto view = camera->getView();
    auto projection = camera->getProjection();
    auto invViewProjection = glm::inverse(projection * view);

	auto& pUBO = pMaterial->getUniformBuffer("UBOSSAOKernel");
    pUBO->set("samples", vSSAOKernel);
    pUBO->set("invViewProjection", invViewProjection);
    pUBO->set("projection", projection);
    pUBO->set("view", view);

	pMaterial->update();
    pMaterial->bind(commandBuffer);

    commandBuffer.draw(3, 1, 0, 0);
}

void CSSAOPass::cleanup()
{
	CSubpass::cleanup();
}
