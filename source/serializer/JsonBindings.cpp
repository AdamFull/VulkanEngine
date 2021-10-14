#include "JsonSerializer.h"

// Add errors for some types

namespace glm
{
     void to_json(nlohmann::json &json, const vec3 &type)
    {
        json = nlohmann::json
        {
            {"x", type.x},
            {"y", type.y},
            {"x", type.z}
        };
    }

    void from_json(const nlohmann::json &json, vec3 &type)
    {
        if(json.find("x") != json.end())
            json.at("x").get_to(type.x);
        if(json.find("y") != json.end())
            json.at("y").get_to(type.y);
        if(json.find("z") != json.end())
            json.at("z").get_to(type.z);
    }
}

namespace Engine
{
    /*RESOURCES*/
    NLOHMANN_JSON_SERIALIZE_ENUM(ETextureType, {
        {ETextureType::e2D, "2d"},
        {ETextureType::e3D, "3d"},
        {ETextureType::eCubemap, "cube"},
        {ETextureType::eArray, "array"}
        })

    NLOHMANN_JSON_SERIALIZE_ENUM(ETextureAttachmentType, {
        {ETextureAttachmentType::eColor, "color"},
        {ETextureAttachmentType::eAlbedo, "albedo"},
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
        //Required
        if(json.find("name") != json.end())
            json.at("name").get_to(type.srName);

        //Required
        if(json.find("type") != json.end())
            json.at("type").get_to(type.eType);

        //Required
        if(json.find("attachment") != json.end())
            json.at("attachment").get_to(type.eAttachment);

        //Required
        if(json.find("src") != json.end())
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
        //Optional
        if(json.find("cutoff") != json.end())
            json.at("cutoff").get_to(type.alpha_cutoff);

        //Optional
        if(json.find("metalic") != json.end())
            json.at("metalic").get_to(type.metalic);

        //Optional
        if(json.find("roughness") != json.end())
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
        //Required
        if(json.find("name") != json.end())
            json.at("name").get_to(type.srName);

        //Required
        if(json.find("type") != json.end())
            json.at("type").get_to(type.eType);

        //Optional
        if(json.find("attachments") != json.end())
            json.at("attachments").get_to(type.srAttachments);

        //Optional
        if(json.find("textures") != json.end())
            json.at("textures").get_to(type.vTextures);
        
        //Required
        if(json.find("params") != json.end())
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
        //Required
        if(json.find("name") != json.end())
            json.at("name").get_to(type.srName);

        //Required
        if(json.find("type") != json.end())
            json.at("type").get_to(type.eType);

        //Required
        if(json.find("src") != json.end())
            json.at("src").get_to(type.srSrc);

        //Optional
        if(json.find("attachments") != json.end())
            json.at("attachments").get_to(type.srAttachments);

        //Optional
        if(json.find("materials") != json.end())
            json.at("materials").get_to(type.vMaterials);

        assert((json.find("attachments") != json.end() || json.find("materials") != json.end()) && "Cannot create mesh without materials");
    }

    /*SCENE*/
    NLOHMANN_JSON_SERIALIZE_ENUM(ESceneObjectType, 
    {
        {ESceneObjectType::eCamera, "2d"},
        {ESceneObjectType::eSkybox, "3d"},
        {ESceneObjectType::eMeshComponent, "cube"}
    })

    void to_json(nlohmann::json &json, const FTransform &type)
    {
        json = nlohmann::json
        {
            {"pos", type.pos},
            {"rot", type.rot},
            {"scale", type.scale}
        };
    }

    void from_json(const nlohmann::json &json, FTransform &type)
    {
        //Optional
        if(json.find("pos") != json.end())
            json.at("pos").get_to(type.pos);

        //Optional
        if(json.find("rot") != json.end())
            json.at("rot").get_to(type.rot);

        //Optional    
        if(json.find("scale") != json.end())
            json.at("scale").get_to(type.scale);
    }

    void to_json(nlohmann::json &json, const FSceneObjectResourceBindings &type)
    {
        json = nlohmann::json
        {
            {"textures", {type.vTextures}},
            {"materials", {type.vMaterials}},
            {"meshes", {type.vMeshes}}
        };
    }

    void from_json(const nlohmann::json &json, FSceneObjectResourceBindings &type)
    {
        //Optional
        if(json.find("textures") != json.end())
            json.at("textures").get_to(type.vTextures);

        //Optional
        if(json.find("materials") != json.end())
            json.at("materials").get_to(type.vMaterials);

        //Optional
        if(json.find("meshes") != json.end())
            json.at("meshes").get_to(type.vMeshes);
    }

    void to_json(nlohmann::json &json, const FSceneObject &type)
    {
        json = nlohmann::json
        {
            {"name", type.srName},
            {"type", type.eObjectType},
            {"resources", {type.vResourceBindings}},
            {"transform", type.fTransform},
            {"childs", {type.vSceneObjects}}
        };
    }

    void from_json(const nlohmann::json &json, FSceneObject &type)
    {
        //Required
        if(json.find("name") != json.end())
            json.at("name").get_to(type.srName);
        
        //Required
        if(json.find("type") != json.end())
            json.at("type").get_to(type.eObjectType);

        //Required
        if(json.find("resources") != json.end())
            json.at("resources").get_to(type.vResourceBindings);

        //Optional
        if(json.find("transform") != json.end())
            json.at("transform").get_to(type.fTransform);

        //Optional
        if(json.find("childs") != json.end())
            json.at("childs").get_to(type.vSceneObjects);
    }

    void to_json(nlohmann::json &json, const FSceneCreateInfo &type)
    {
        json = nlohmann::json
        {
            {"resources", type.resources_path},
            {"objects", {type.vSceneObjects}}
        };
    }

    void from_json(const nlohmann::json &json, FSceneCreateInfo &type)
    {
        //Required
        if(json.find("resources") != json.end())
            json.at("resources").get_to(type.resources_path);
        
        //Optional
        if(json.find("objects") != json.end())  
            json.at("objects").get_to(type.vSceneObjects);
    }
} // namespace Engine