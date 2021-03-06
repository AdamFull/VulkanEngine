#include "InspectorWindow.h"
#include "graphics/VulkanHighLevel.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/objects/components/CameraComponent.h"
#include "resources/materials/VulkanMaterial.h"
#include "editor/CustomControls.h"

using namespace engine::core;
using namespace engine::editor;
using namespace engine::core::scene;
using namespace engine::resources;

constexpr const size_t culling_types_count{3};
constexpr const char* cull_type_point{"point"};
constexpr const char* cull_type_sphere{"sphere"};
constexpr const char* cull_type_bounds{"bounds"};

const char* culling_types[] = {cull_type_point, cull_type_sphere, cull_type_bounds};

ECullingType toCullingType(const char* inString)
{
    if(strcmp(inString, cull_type_point) == 0) return ECullingType::eByPoint;
    else if(strcmp(inString, cull_type_sphere) == 0) return ECullingType::eBySphere;
    else if(strcmp(inString, cull_type_bounds) == 0) return ECullingType::eByBox;
    return ECullingType::eByPoint;
}

const char* toCullingName(ECullingType eName)
{
    switch (eName)
    {
    case ECullingType::eByPoint: return cull_type_point;
    case ECullingType::eBySphere: return cull_type_sphere;
    case ECullingType::eByBox: return cull_type_bounds;
    }
}

void CInspectorWindow::draw()
{
    if (bOverlayState)
    {
        if (!ImGui::Begin("Inspector", &bOverlayState))
        {
            ImGui::End();
            return;
        }

        auto selected = UEditor->getLastSelection();

        if(selected)
        {
            ImGui::Separator();
            ImGui::Text("Parameters");
            ImGui::Separator();

            auto& name = selected->getName();
            if(FControls::TextInput("Name", name))
                selected->setName(name);

            ImGui::Separator();
            ImGui::Text("Transform");
            ImGui::Separator();

            auto transform = selected->getLocalTransform();
            if(FControls::DragTransform(transform))
                selected->setTransform(transform);

            ImGui::Separator();
            ImGui::Text("Flags");
            ImGui::Separator();

            auto isVisible = selected->isVisible();
            if(FControls::CheckBox("Is visible", &isVisible))
                selected->setVisible(isVisible);
            auto isEnabled = selected->isEnabled();
            if(FControls::CheckBox("Is enabled", &isEnabled))
                selected->setEnable(isEnabled);

            ImGui::Separator();

            if(selected->getMesh())
            {
                auto& pMesh = selected->getMesh();
                ImGui::Text("Static Mesh");
                ImGui::Separator();

                auto isCullable = pMesh->isCullable();
                if(FControls::CheckBox("Is cullable", &isCullable))
                    pMesh->setCullable(isCullable);

                if(isCullable)
                {
                    auto cullingType = pMesh->getCullingType();
                    const char* culling_type = toCullingName(cullingType);
                    if(ImGui::BeginCombo("Culling type", culling_type))
                    {
                        for (int n = 0; n < culling_types_count; n++)
                        {
                            bool is_selected = (culling_type == culling_types[n]);
                            if (ImGui::Selectable(culling_types[n], is_selected))
                                culling_type = culling_types[n];
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    cullingType = toCullingType(culling_type);
                    pMesh->setCullingType(cullingType);
                }

                ImGui::Text("Primitives");
                ImGui::Separator();

                for(auto& primitive : pMesh->getPrimitives())
                {
                    if(isCullable)
                    {
                        FControls::CheckBox("Culling result:", &primitive.bWasCulled);

                        auto cullingType = pMesh->getCullingType();
                        switch (cullingType)
                        {
                            case ECullingType::eByPoint: {} break;
                            case ECullingType::eBySphere:  {
                                FControls::DragFloat("Sphere radius", &primitive.dimensions.radius, 0.01, 0.01);
                            } break;
                            case ECullingType::eByBox: {
                                FControls::DragFloatVec3("Box Begin", primitive.dimensions.min, 0.01, 0.01);
                                FControls::DragFloatVec3("Box End", primitive.dimensions.max, 0.01, 0.01);
                            } break;
                        }
                    }
                    ImGui::Separator();
                    ImGui::Text("Material");
                    ImGui::Separator();

                    auto material = primitive.material.lock();
                    auto& params = material->getParams();

                    FControls::DragFloat("Alpha cutoff", &params.alphaCutoff, 0.005f, 0.005f);
                    FControls::DragFloat("Scale normal", &params.normalScale, 0.005f, 0.005f);
                    FControls::DragFloat("Occlusion strenth", &params.occlusionStrenth, 0.005f, 0.005f);
                    FControls::DragFloat("Metallic factor", &params.metallicFactor, 0.005f, 0.005f);
                    FControls::DragFloat("Roughness factor", &params.roughnessFactor, 0.005f, 0.005f);
                    FControls::DragFloat("Tesselation factor", &params.tessellationFactor, 0.005f, 0.f);
                    FControls::DragFloat("Tesselation Strength", &params.tessStrength, 0.005f, 0.005f);
                    FControls::ColorEdit4("Base color factor", params.baseColorFactor);
                    FControls::ColorEdit3("Emissive factor", params.emissiveFactor);
                }

                //TODO: show primitives and materials
            }

            if(selected->getCamera())
            {
                auto& pCamera = selected->getCamera();
                ImGui::Text("Camera");
                ImGui::Separator();
                auto fov = pCamera->getFieldOfView();
                if(FControls::DragFloat("FOV", &fov, 0.01, 0.1))
                    pCamera->setFieldOfView(fov);
                auto nearPlane = pCamera->getNearPlane();
                if(FControls::DragFloat("Near plane", &nearPlane, 0.01, 0.1))
                    pCamera->setNearPlane(nearPlane);
                auto farPlane = pCamera->getFarPlane();
                if(FControls::DragFloat("Far plane", &farPlane, 0.01, 0.1))
                    pCamera->setFarPlane(farPlane);
            }

            if(selected->getLight())
            {
                //TODO: check light type
                auto& pLight = selected->getLight();
                switch (pLight->getType())
                {
                    case ELightSourceType::eDirectional:
                    {
                        ImGui::Text("Directional light");
                        ImGui::Separator();
                    } break;
                    case ELightSourceType::ePoint: {
                        ImGui::Text("Point light");
                        ImGui::Separator();

                        auto radius = pLight->getRadius();
                        if(FControls::DragFloat("radius", &radius, 0.01))
                            pLight->setRadius(radius);
                    } break;
                    case ELightSourceType::eSpot: {
                        ImGui::Text("Spot light");
                        ImGui::Separator();

                        auto innerConeAngle = pLight->getInnerAngle();
                        if(FControls::DragFloat("inner angle", &innerConeAngle, 0.01))
                            pLight->setInnerAngle(innerConeAngle);

                        auto outerConeAngle = pLight->getOuterAngle();
                        if(FControls::DragFloat("outer angle", &outerConeAngle, 0.01))
                            pLight->setOuterAngle(outerConeAngle);
                    } break;
                }

                //TODO: add light type changing here
                auto lightColor = pLight->getColor();
                if(FControls::ColorEdit3("Color", lightColor))
                    pLight->setColor(lightColor);

                auto lightIntencity = pLight->getIntencity();
                if(FControls::DragFloat("Intencity", &lightIntencity, 0.01, 0.1))
                    pLight->setIntencity(lightIntencity);
            }
        }

        ImGui::End();
    }
}