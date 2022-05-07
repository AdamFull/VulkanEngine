#include "MaterialLoader.h"
#include "filesystem/FilesystemHelper.h"
#include "graphics/data_types/VulkanVertex.hpp"

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

namespace Engine
{
    namespace Resources
    {
        namespace Material
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

using namespace Engine::Resources;
using namespace Engine::Resources::Material;
using namespace Engine::Core;
using namespace Engine::Core::Pipeline;

template<>
std::unique_ptr<CMaterialLoader> utl::singleton<CMaterialLoader>::_instance{nullptr};

CMaterialLoader::CMaterialLoader()
{
    load();
}

std::shared_ptr<CMaterialBase> CMaterialLoader::create(const std::string& name)
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

        std::shared_ptr<CMaterialBase> material = std::make_unique<CMaterialBase>();
        material->m_pPipeline = CPipelineBase::Builder().
        setVertexInput(std::move(vertexInput)).
        setCulling(ci.culling).
        setFontFace(ci.frontface).
        setDepthEnabled(ci.enableDepth).
        setDynamicStates(ci.dynamicStateEnables).
        setShaderStages(ci.stages).
        setDefines(ci.defines).
        build(ci.bindPoint);
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
