#include "JsonSerializer.h"
#include "Resources/ResourceFactory.h"

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::ETextureType, {
    {Engine::ETextureType::e2D, "2d"},
    {Engine::ETextureType::e3D, "3d"},
    {Engine::ETextureType::eCubemap, "cube"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::ETextureAttachmentType, {
    {Engine::ETextureAttachmentType::eColor, "color"},
    {Engine::ETextureAttachmentType::eMetalicRoughness, "roughness"},
    {Engine::ETextureAttachmentType::eNormal, "normal"},
    {Engine::ETextureAttachmentType::eHeight, "height"},
    {Engine::ETextureAttachmentType::eOcclusion, "occlusion"},
    {Engine::ETextureAttachmentType::eEmissive, "emissive"},
    {Engine::ETextureAttachmentType::eSpecular, "specular"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::EMaterialType, {
    {Engine::EMaterialType::eUI, "ui"},
    {Engine::EMaterialType::eDiffuse, "diffuse"},
    {Engine::EMaterialType::eGLTF, "gltf"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(Engine::EMeshType, {
    {Engine::EMeshType::eStatic, "static"},
    {Engine::EMeshType::eSkeletal, "skeletal"},
    {Engine::EMeshType::eGLTF, "gltf"}
})


//From struct to json
template<>
void to_json<Engine::FTextureCreateInfo>(nlohmann::json& json, const Engine::FTextureCreateInfo& type)
{
    json = nlohmann::json
    {
        {"type", type.eType},
        {"attachment", type.eAttachment}
    };
}

template<>
void to_json<Engine::FMaterialCreateInfo>(nlohmann::json& json, const Engine::FMaterialCreateInfo& type)
{
    json = nlohmann::json
    {
        {"type", type.eType},
        {"textures", type.vTextures}
    };
}

template<>
void to_json<Engine::FMeshCreateInfo>(nlohmann::json& json, const Engine::FMeshCreateInfo& type)
{
    json = nlohmann::json
    {
        {"type", type.eType},
        {"materials", type.vMaterials}
    };
}

//From json to object
template<>
void from_json<Engine::FTextureCreateInfo>(nlohmann::json& json, const Engine::FTextureCreateInfo& type)
{
    json.at("type").get_to(type.eType);
    json.at("attachment").get_to(type.eAttachment);
}

template<>
void from_json<Engine::FMaterialCreateInfo>(nlohmann::json& json, const Engine::FMaterialCreateInfo& type)
{
    json.at("type").get_to(type.eType);
    type.vTextures = json.at("textures").get<decltype(type.vTextures)>();
}

template<>
void from_json<Engine::FMeshCreateInfo>(nlohmann::json& json, const Engine::FMeshCreateInfo& type)
{
    json.at("type").get_to(type.eType);
    type.vMaterials = json.at("materials").get<decltype(type.vMaterials)>();
}