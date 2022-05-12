#include "MeshFragment.h"
#include "util/uuid.hpp"
#include "resources/materials/VulkanMaterial.h"
#include "graphics/VulkanDevice.hpp"
#include "graphics/VulkanHighLevel.h"
#include "util/Frustum.hpp"
#include <glm/gtx/matrix_decompose.hpp>


using namespace Engine::Util;
using namespace Engine::Resources::Mesh;

void FPrimitive::setDimensions(glm::vec3 min, glm::vec3 max)
{
    dimensions.min = min;
    dimensions.max = max;
    dimensions.size = max - min;
    dimensions.center = (min + max) / 2.0f;
    dimensions.radius = glm::distance(min, max) / 2.0f;
}

void CMeshFragment::create()
{
    for (auto &primitive : m_vPrimitives)
    {
        if(primitive.material)
            primitive.material->create();
    }
}

void CMeshFragment::addPrimitive(FPrimitive &&primitive)
{
    m_vPrimitives.emplace_back(primitive);
}

FPrimitive& CMeshFragment::getPrimitive(uint32_t index)
{
    return m_vPrimitives.at(index);
}

void CMeshFragment::reCreate()
{
    for (auto &primitive : m_vPrimitives)
    {
        if(primitive.material)
            primitive.material->reCreate();
    }
}

void CMeshFragment::render(vk::CommandBuffer commandBuffer, vk::DescriptorBufferInfo& uboDesc, const glm::mat4& model, uint32_t imageIndex, uint32_t instanceCount)
{
    glm::vec3 scale;
    glm::quat rot;
    glm::vec3 pos;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(model, scale, rot, pos, skew, perspective);

    for (auto &primitive : m_vPrimitives)
    {
        bool needToRender{true};
        switch (2)
        {
        case 0:{
            needToRender = CFrustum::inst()->checkPoint(pos);
        }break;
        case 1:{
            needToRender = CFrustum::inst()->checkSphere(pos, primitive.dimensions.radius);
        }break;
        case 2:{
            needToRender = CFrustum::inst()->checkBox(pos + primitive.dimensions.min * scale, pos + primitive.dimensions.max * scale);
        }break;
        }

        if(needToRender)
        {
            if(primitive.material && primitive.bUseMaterial)
            {
                primitive.material->addBuffer("FUniformData", uboDesc);
                primitive.material->update(imageIndex);
                primitive.material->bind(commandBuffer, imageIndex);
            }
            commandBuffer.drawIndexed(primitive.indexCount, instanceCount, primitive.firstIndex, 0, 0);
        }
    }
}

void CMeshFragment::cleanup()
{
    for (auto &primitive : m_vPrimitives)
    {
        if(primitive.material)
            primitive.material->cleanup();
    }
}

void CMeshFragment::setName(const std::string& srName)
{
    m_srName = srName + uuid::generate();
}