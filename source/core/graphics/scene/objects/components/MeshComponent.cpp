#include "MeshComponent.h"
#include "resources/materials/VulkanMaterial.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/components/CameraManager.h"

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

    auto transform = pParent->getTransform();
    auto view = camera->getView();
    auto projection = camera->getProjection();
    auto model = transform.getModel();
    auto normal = glm::transpose(glm::inverse(model));
    auto position = transform.getPosition();

    for (auto &primitive : vPrimitives)
    {
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
        primitive.bWasCulled = needToRender;

        //TODO: load primitive position?
        if(needToRender)
        {
            if(primitive.material && primitive.bUseMaterial)
            {
                auto& pUBO = primitive.material->getUniformBuffer("FUniformData");
                pUBO->set("model", model);
                pUBO->set("view", view);
                pUBO->set("projection", projection);
                pUBO->set("normal", normal);
                //pUBO->set("instancePos", instancePos);
                
                primitive.material->update();
                primitive.material->bind(commandBuffer);

                auto& params = primitive.material->getParams();
                auto& material = primitive.material->getPushConstant("ubo");
                if(material)
                {
                    material->set("alphaCutoff", params.alphaCutoff);
                    material->set("emissiveFactor", params.emissiveFactor);
                    material->set("normalScale", params.normalScale);
                    material->set("occlusionStrenth", params.occlusionStrenth);
                    material->set("baseColorFactor", params.baseColorFactor);
                    material->set("metallicFactor", params.metallicFactor);
                    material->set("roughnessFactor", params.roughnessFactor);
                    material->set("viewDir", camera->viewPos);
                    material->flush(commandBuffer);  
                }
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