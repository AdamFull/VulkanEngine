#include "MaterialLoader.h"
#include "filesystem/FilesystemHelper.h"
#include "graphics/data_types/VulkanVertex.hpp"
#include "graphics/pipeline/ComputePipeline.h"
#include "graphics/pipeline/GraphicsPipeline.h"

namespace vk
{
    NLOHMANN_JSON_SERIALIZE_ENUM
    (
        CullModeFlagBits,
        {
            {CullModeFlagBits::eNone, "none"},
            {CullModeFlagBits::eBack, "back"},
            {CullModeFlagBits::eFront, "front"},
            {CullModeFlagBits::eFrontAndBack, "frontAndBack"}
        }
    )

    NLOHMANN_JSON_SERIALIZE_ENUM
    (
        FrontFace,
        {
            {FrontFace::eClockwise, "clockwise"},
            {FrontFace::eCounterClockwise, "counterClockwise"}
        }
    )

    NLOHMANN_JSON_SERIALIZE_ENUM
    (
        DynamicState,
        {
            {DynamicState::eBlendConstants, "blendConstants"},
            {DynamicState::eColorWriteEnableEXT, "colorWriteEnableEXT"},
            {DynamicState::eCullModeEXT, "cullModeEXT"},
            {DynamicState::eDepthBias, "depthBias"},
            {DynamicState::eDepthBiasEnableEXT, "depthBiasEnableEXT"},
            {DynamicState::eDepthBounds, "depthBounds"},
            {DynamicState::eDepthBoundsTestEnableEXT, "depthBoundsTestEnableEXT"},
            {DynamicState::eDepthCompareOpEXT, "depthCompareOpEXT"},
            {DynamicState::eDepthTestEnableEXT, "depthTestEnableEXT"},
            {DynamicState::eDiscardRectangleEXT, "discardRectangleEXT"},
            {DynamicState::eExclusiveScissorNV, "exclusiveScissorNV"},
            {DynamicState::eFragmentShadingRateKHR, "fragmentShadingRateKHR"},
            {DynamicState::eFrontFaceEXT, "frontFaceEXT"},
            {DynamicState::eLineStippleEXT, "lineStippleEXT"},
            {DynamicState::eLineWidth, "lineWidth"},
            {DynamicState::eLogicOpEXT, "logicOpEXT"},
            {DynamicState::ePatchControlPointsEXT, "patchControlPointsEXT"},
            {DynamicState::ePrimitiveRestartEnableEXT, "primitiveRestartEnableEXT"},
            {DynamicState::ePrimitiveTopologyEXT, "primitiveTopologyEXT"},
            {DynamicState::eRasterizerDiscardEnableEXT, "rasterizerDiscardEnableEXT"},
            {DynamicState::eRayTracingPipelineStackSizeKHR, "rayTracingPipelineStackSizeKHR"},
            {DynamicState::eSampleLocationsEXT, "sampleLocationsEXT"},
            {DynamicState::eScissor, "scissor"},
            {DynamicState::eScissorWithCountEXT, "scissorWithCountEXT"},
            {DynamicState::eStencilCompareMask, "stencilCompareMask"},
            {DynamicState::eStencilOpEXT, "stencilOpEXT"},
            {DynamicState::eStencilReference, "stencilReference"},
            {DynamicState::eStencilTestEnableEXT, "stencilTestEnableEXT"},
            {DynamicState::eStencilWriteMask, "stencilWriteMask"},
            {DynamicState::eVertexInputBindingStrideEXT, "vertexInputBindingStrideEXT"},
            {DynamicState::eVertexInputEXT, "vertexInputEXT"},
            {DynamicState::eViewport, "viewport"},
            {DynamicState::eViewportCoarseSampleOrderNV, "viewportCoarseSampleOrderNV"},
            {DynamicState::eViewportShadingRatePaletteNV, "viewportShadingRatePaletteNV"},
            {DynamicState::eViewportWithCountEXT, "viewportWithCountEXT"},
            {DynamicState::eViewportWScalingNV, "viewportWScalingNV"}
        }
    )

    NLOHMANN_JSON_SERIALIZE_ENUM
    (
        PipelineBindPoint,
        {
            {PipelineBindPoint::eCompute, "compute"},
            {PipelineBindPoint::eGraphics, "graphics"},
            {PipelineBindPoint::eRayTracingKHR, "raytracingKHR"},
            {PipelineBindPoint::eRayTracingNV, "raytracingNV"},
            {PipelineBindPoint::eSubpassShadingHUAWEI, "subpassShadingHuawei"}
        }
    )
}

namespace engine
{
    namespace resources
    {
        namespace material
        {
            NLOHMANN_JSON_SERIALIZE_ENUM
            (
                FMaterialInfo::EVertexType,
                {
                    {FMaterialInfo::EVertexType::eNone, "none"},
                    {FMaterialInfo::EVertexType::eDefault, "default"},
                    {FMaterialInfo::EVertexType::eImgui, "imgui"}
                }
            )

            void to_json(nlohmann::json &json, const FMaterialInfo::FCreationInfo &type)
            {
                json = nlohmann::json
                {
                    {"vertexType", type.vertexType},
                    {"bindPoint", type.bindPoint},
                    {"culling", type.culling},
                    {"frontface", type.frontface},
                    {"depth", type.enableDepth},
                    {"dynamicStates", type.dynamicStateEnables},
                    {"stages", type.stages},
                    {"defines", type.defines},
                    {"multisampling", type.multisampling}
                };
            }

            void from_json(const nlohmann::json &json, FMaterialInfo::FCreationInfo &type)
            {
                ParseArgument(json, type.vertexType, "vertexType", false);
                ParseArgument(json, type.bindPoint, "bindPoint", true);
                ParseArgument(json, type.culling, "culling", false);
                ParseArgument(json, type.frontface, "frontface", false);
                ParseArgument(json, type.enableDepth, "depth", false);
                ParseArgument(json, type.dynamicStateEnables, "dynamicStates", false);
                ParseArgument(json, type.stages, "stages", true);
                ParseArgument(json, type.defines, "defines", false);
                ParseArgument(json, type.multisampling, "multisampling", false);
            }

            void to_json(nlohmann::json &json, const FMaterialInfo &type)
            {
                json = nlohmann::json
                {
                    {"materials", type.creationInfo}
                };
            }

            void from_json(const nlohmann::json &json, FMaterialInfo &type)
            {
                ParseArgument(json, type.creationInfo, "materials", true);
            }
        }
    }
}

using namespace engine::resources;
using namespace engine::resources::material;
using namespace engine::core;
using namespace engine::core::pipeline;

template<>
scope_ptr<CMaterialLoader> utl::singleton<CMaterialLoader>::_instance{nullptr};

CMaterialLoader::CMaterialLoader()
{
    load();
}

ref_ptr<CMaterialBase> CMaterialLoader::create(const std::string& name)
{
    //TODO: Check ability to store same materials
    auto it = data.creationInfo.find(name);
    if(it != data.creationInfo.end())
    {
        auto& ci = it->second;
        CVertexInput vertexInput{};
        switch(ci.vertexType)
        {
            case FMaterialInfo::EVertexType::eNone: break;
            case FMaterialInfo::EVertexType::eDefault: vertexInput = CVertexInput(FVertex::getBindingDescription(), FVertex::getAttributeDescriptions()); break;
            case FMaterialInfo::EVertexType::eImgui: vertexInput = CVertexInput(FVertexUI::getBindingDescription(), FVertexUI::getAttributeDescriptions()); break;
        }

        ref_ptr<CMaterialBase> material = make_scope<CMaterialBase>();
    
        switch (ci.bindPoint)
        {
        case vk::PipelineBindPoint::eCompute: material->pPipeline = make_scope<CComputePipeline>(); break;
        case vk::PipelineBindPoint::eGraphics: material->pPipeline = make_scope<CGraphicsPipeline>(); break;
        case vk::PipelineBindPoint::eRayTracingKHR: material->pPipeline = make_scope<CComputePipeline>(); break;
        case vk::PipelineBindPoint::eSubpassShadingHUAWEI: material->pPipeline = make_scope<CComputePipeline>(); break;
        }

        material->pPipeline->setBindPoint(ci.bindPoint);
        material->pPipeline->setDefines(std::move(ci.defines));
        material->pPipeline->setShaderStages(std::move(ci.stages));

        if(ci.bindPoint == vk::PipelineBindPoint::eGraphics)
        {
            material->pPipeline->setVertexInput(std::move(vertexInput));
            material->pPipeline->setCulling(ci.culling);
            material->pPipeline->setFontFace(ci.frontface);
            material->pPipeline->setDepthEnabled(ci.enableDepth);
            material->pPipeline->setDynamicStates(ci.dynamicStateEnables);
        }    

        return material;
    }
    
    return nullptr;
}

void CMaterialLoader::load()
{
    auto tmp = FilesystemHelper::readFile("materials.json");
    if(!tmp.empty())
    {
        auto bson = nlohmann::json::parse(tmp);
        bson.get_to(data);
    }
}

void CMaterialLoader::save()
{
    auto json = nlohmann::json(data).dump();
    FilesystemHelper::writeFile("materials.json", json);
}
