#include "JsonSerializer.h"

namespace Engine
{
    NLOHMANN_JSON_SERIALIZE_ENUM(ETextureType, {
        {ETextureType::e2D, "2d"},
        {ETextureType::e3D, "3d"},
        {ETextureType::eCubemap, "cube"}
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(ETextureAttachmentType, {
        {ETextureAttachmentType::eColor, "color"},
        {ETextureAttachmentType::eMetalicRoughness, "roughness"},
        {ETextureAttachmentType::eNormal, "normal"},
        {ETextureAttachmentType::eHeight, "height"},
        {ETextureAttachmentType::eOcclusion, "occlusion"},
        {ETextureAttachmentType::eEmissive, "emissive"},
        {ETextureAttachmentType::eSpecular, "specular"}})

    NLOHMANN_JSON_SERIALIZE_ENUM(EMaterialType, {
        {EMaterialType::eUI, "ui"},
        {EMaterialType::eDiffuse, "diffuse"},
        {EMaterialType::eGLTF, "gltf"}})

    NLOHMANN_JSON_SERIALIZE_ENUM(EMeshType, {
        {EMeshType::eStatic, "static"},
        {EMeshType::eSkeletal, "skeletal"},
        {EMeshType::eGLTF, "gltf"}})

    void to_json(nlohmann::json &json, const FTextureCreateInfo &type)
    {
        json = nlohmann::json
        {
            {"name", type.srName},
            {"type", type.eType},
            {"attachment", type.eAttachment},
            {"src", type.srSrc}
        };
    }

    void from_json(const nlohmann::json &json, FTextureCreateInfo &type)
    {
        if(json.find("name") != json.end())
        json.at("name").get_to(type.srName);
        json.at("type").get_to(type.eType);
        json.at("attachment").get_to(type.eAttachment);
        json.at("src").get_to(type.srSrc);
    }

    void to_json(nlohmann::json &json, const const FMaterialParams &type)
    {
        json = nlohmann::json
        {
            {"cutoff", type.alpha_cutoff},
            {"metalic", type.metalic},
            {"roughness", type.specular}
        };
    }

    void from_json(const nlohmann::json &json, FMaterialParams &type)
    {
        json.at("cutoff").get_to(type.alpha_cutoff);
        json.at("metalic").get_to(type.metalic);
        json.at("roughness").get_to(type.specular);
    }

    void to_json(nlohmann::json &json, const FMaterialCreateInfo &type)
    {
        json = nlohmann::json
        {
            {"name", type.srName},
            {"type", type.eType},
            {"attachments", type.srAttachments},
            {"textures", {type.vTextures}},
            {"params", type.fParams}
        };
    }

    void from_json(const nlohmann::json &json, FMaterialCreateInfo &type)
    {
        if(json.find("name") != json.end())
        json.at("name").get_to(type.srName);
        json.at("type").get_to(type.eType);

        if(json.find("attachments") != json.end())
        json.at("attachments").get_to(type.srAttachments);

        if(json.find("textures") != json.end())
        json.at("textures").get_to(type.vTextures);
        json.at("params").get_to(type.fParams);

        assert((json.find("attachments") != json.end() || json.find("textures") != json.end()) && "Cannot create material without textures");
    }

    void to_json(nlohmann::json &json, const FMeshCreateInfo &type)
    {
        json = nlohmann::json
        {
            {"name", type.srName},
            {"type", type.eType},
            {"src", type.srSrc},
            {"attachments", type.srAttachments},
            {"materials", {type.vMaterials}}
        };
    }

    void from_json(const nlohmann::json &json, FMeshCreateInfo &type)
    {
        if(json.find("name") != json.end())
        json.at("name").get_to(type.srName);
        json.at("type").get_to(type.eType);
        json.at("src").get_to(type.srSrc);

        if(json.find("attachments") != json.end())
        json.at("attachments").get_to(type.srAttachments);

        if(json.find("materials") != json.end())
        json.at("materials").get_to(type.vMaterials);

        assert((json.find("attachments") != json.end() || json.find("materials") != json.end()) && "Cannot create mesh without materials");
    }

} // namespace Engine