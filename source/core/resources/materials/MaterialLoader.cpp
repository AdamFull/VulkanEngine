#include "MaterialLoader.h"

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
}

namespace Engine
{
    namespace Resources
    {
        void to_json(nlohmann::json &json, const FMaterialInfo::FCreationInfo &type)
        {
            json = nlohmann::json
            {
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
            ParseArgument(json, type.culling, "culling", true);
            ParseArgument(json, type.frontface, "frontface", true);
            ParseArgument(json, type.enableDepth, "depth", true);
            ParseArgument(json, type.dynamicStateEnables, "dynamicStates", true);
            ParseArgument(json, type.stages, "stages", true);
            ParseArgument(json, type.defines, "defines", true);
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

using namespace Engine::Resources;

template<>
std::unique_ptr<CMaterialLoader> utl::singleton<CMaterialLoader>::_instance{nullptr};

CMaterialLoader::CMaterialLoader()
{
    load();
}

CMaterialLoader::~CMaterialLoader()
{
    //save();
}

void CMaterialLoader::load()
{
    std::ifstream infile("../../assets/materials.json", std::ios::in | std::ios::binary);
    infile.rdbuf()->pubsetbuf(0, 0);
    auto tmp = std::string(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());
    if(!tmp.empty())
    {
        auto bson = nlohmann::json::parse(tmp);
        bson.get_to(data);
    }
    infile.close();
}

void CMaterialLoader::save()
{
    std::ofstream outfile("../../assets/materials.json", std::ios::out | std::ios::binary);
    auto json = nlohmann::json(data);
    outfile << json;
    outfile.close();
}
