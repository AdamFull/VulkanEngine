#include "MeshComponent.h"
#include "resources/materials/VulkanMaterial.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/components/CameraManager.h"

using namespace engine::core::render;
using namespace engine::core::scene;

void FPrimitive::setDimensions(glm::vec3 min, glm::vec3 max)
{
    dimensions.min = min;
    dimensions.max = max;
    dimensions.size = max - min;
    dimensions.center = (min + max) / 2.0f;
    dimensions.radius = glm::distance(min, max) / 2.0f;
}

void CMeshComponent::create()
{
    for (auto &primitive : vPrimitives)
    {
        if(primitive.material)
            primitive.material->create();
    }
}

void CMeshComponent::reCreate()
{
    for (auto &primitive : vPrimitives)
    {
        if(primitive.material)
            primitive.material->reCreate();
    }
}

void CMeshComponent::render(vk::CommandBuffer &commandBuffer)
{
    auto& cameraNode = CCameraManager::inst()->getCurrentCamera();
    auto& camera = cameraNode->getCamera();
    auto& frustumSides = camera->getFrustumSides();

    auto transform = pParent->getTransform();
    auto view = camera->getView();
    auto projection = camera->getProjection();
    auto model = pParent->getModel();
    auto normal = glm::transpose(glm::inverse(model));
    auto position = pParent->getPosition();

    for (auto &primitive : vPrimitives)
    {
        auto isShadowPass = CRenderSystem::inst()->getStageType() == EStageType::eShadow;
        auto isSkyboxInShadow = isShadowPass && bIsSkybox;

        bool needToRender {true};
        if (bEnableCulling)
        {
            switch (eCullingType)
            {
            case ECullingType::eByPoint:
                needToRender = camera->checkPoint(position);
            break;
            case ECullingType::eBySphere:
                needToRender = camera->checkSphere(position, primitive.dimensions.radius);
            break;
            case ECullingType::eByBox:
                needToRender = camera->checkBox(position + primitive.dimensions.min * transform.getScale(), position + primitive.dimensions.max * transform.getScale());
            break;
            }
        }
        
        //For shadows rendering anyway
        needToRender |= isShadowPass;

        if(!isShadowPass)
            primitive.bWasCulled = needToRender;

        //TODO: load primitive position?
        if(needToRender && !isSkyboxInShadow)
        {
            //TODO: add automatic alignment in shader float = 4, 2x float = 8, 3x float = 16
            if(primitive.material && !isShadowPass)
            {
                auto& pUBO = primitive.material->getUniformBuffer("FUniformData");
                pUBO->set("model", model);
                pUBO->set("view", view);
                pUBO->set("projection", projection);
                pUBO->set("normal", normal);
                pUBO->set("viewDir", camera->viewPos);
                auto ext = CDevice::inst()->getExtent(true);
                pUBO->set("viewportDim", glm::vec2(ext.width, ext.height));
                pUBO->set("frustumPlanes", frustumSides);
                //pUBO->set("instancePos", instancePos);

                auto& params = primitive.material->getParams();
                auto& material = primitive.material->getUniformBuffer("UBOMaterial");
                if(material)
                {
                    material->set("baseColorFactor", params.baseColorFactor);
                    material->set("emissiveFactor", params.emissiveFactor);
                    material->set("alphaCutoff", params.alphaCutoff);
                    material->set("normalScale", params.normalScale);
                    material->set("occlusionStrenth", params.occlusionStrenth);
                    material->set("metallicFactor", params.metallicFactor);
                    material->set("roughnessFactor", params.roughnessFactor);
                    material->set("tessellationFactor", params.tessellationFactor);
                    material->set("tessellationStrength", params.tessStrength);
                }

                primitive.material->update();
                primitive.material->bind(commandBuffer);
            }
            else
            {
                auto& renderer = CRenderSystem::inst()->getCurrentRenderer();
                auto& material = renderer->getMaterial();
                auto& pConst = material->getPushConstant("modelData");
                pConst->set("model", model);
                pConst->flush(commandBuffer);
            }
            commandBuffer.drawIndexed(primitive.indexCount, 1, primitive.firstIndex, 0, 0);
        }
    }
}

void CMeshComponent::update(float fDeltaTime)
{
    
}

void CMeshComponent::cleanup()
{
    for (auto &primitive : vPrimitives)
    {
        if(primitive.material)
            primitive.material->cleanup();
    }
}

void CMeshComponent::destroy()
{

}

void CMeshComponent::addPrimitive(FPrimitive &&primitive)
{
    vPrimitives.emplace_back(primitive);
}