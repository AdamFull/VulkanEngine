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

CMeshComponent::~CMeshComponent()
{
    vPrimitives.clear();
}

void CMeshComponent::create()
{
    CSceneComponent::create();
    for (auto &primitive : vPrimitives)
    {
        auto material = primitive.material.lock();
        if(material)
            material->create();
    }
}

void CMeshComponent::reCreate()
{
    CSceneComponent::reCreate();
    for (auto &primitive : vPrimitives)
    {
        auto material = primitive.material.lock();
        if(material)
            material->reCreate();
    }
}

void CMeshComponent::render(vk::CommandBuffer &commandBuffer)
{
    CSceneComponent::render(commandBuffer);//render struct with commandBuffer and current frustum

    auto cameraNode = UCamera->getCurrentCamera().lock();
    auto camera = cameraNode->getComponent<CCameraComponent>().lock();

    auto transform = pParent.lock()->getTransform();
    auto view = camera->getView();
    auto projection = camera->getProjection();
    auto model = pParent.lock()->getModel();
    auto modelOld = pParent.lock()->getModelOld();
    auto normal = glm::transpose(glm::inverse(model));
    auto position = pParent.lock()->getPosition();

    for (auto &primitive : vPrimitives)
    {
        auto isShadowPass = URenderer->getStageType() == EStageType::eShadow;
        auto isSkyboxInShadow = isShadowPass && bIsSkybox;

        auto& frustum = camera->getFrustum();
        bool needToRender {true};
        if (bEnableCulling && !isShadowPass)
        {
            switch (eCullingType)
            {
            case ECullingType::eByPoint:
                needToRender = frustum.checkPoint(position);
            break;
            case ECullingType::eBySphere:
                needToRender = frustum.checkSphere(position, primitive.dimensions.radius);
            break;
            case ECullingType::eByBox:
                needToRender = frustum.checkBox(position + primitive.dimensions.min * transform.getScale(), position + primitive.dimensions.max * transform.getScale());
            break;
            }
        }
        primitive.bWasCulled = needToRender;

        //TODO: load primitive position?
        if(needToRender && !isSkyboxInShadow)
        {
            //TODO: add automatic alignment in shader float = 4, 2x float = 8, 3x float = 16
            auto mat = primitive.material.lock();
            if(mat && !isShadowPass)
            {
                auto& pUBO = mat->getUniformBuffer("FUniformData");
                pUBO->set("model", model);
                pUBO->set("model_old", modelOld);
                pUBO->set("view", view);
                pUBO->set("projection", projection);
                pUBO->set("normal", normal);
                pUBO->set("viewDir", camera->viewPos);
                auto ext = UDevice->getExtent(true);
                pUBO->set("viewportDim", glm::vec2(ext.width, ext.height));
                pUBO->set("frustumPlanes", frustum.getFrustumSides());
                //pUBO->set("instancePos", instancePos);

                auto& params = mat->getParams();
                auto& material = mat->getUniformBuffer("UBOMaterial");
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

                mat->update();
                mat->bind(commandBuffer);
            }
            else
            {
                auto& renderer = URenderer->getCurrentRenderer();
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
    CSceneComponent::update(fDeltaTime);
}

void CMeshComponent::addPrimitive(FPrimitive &&primitive)
{
    vPrimitives.emplace_back(primitive);
}