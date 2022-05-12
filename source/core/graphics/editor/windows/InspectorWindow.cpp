#include "InspectorWindow.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/objects/components/MeshComponentBase.h"
#include "graphics/scene/objects/components/camera/CameraComponent.h"
#include "graphics/scene/objects/components/light/LightComponent.h"
#include "graphics/editor/Editor.h"
#include <glm/gtc/type_ptr.hpp>

using namespace Engine::Core;
using namespace Engine::Core::Scene;
using namespace Engine::Core::Editor;

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

        auto selected = CEditor::inst()->getLastSelection();

        if(selected)
        {
            auto& name = selected->getName();
            ImGui::Text("Name");
            if(ImGui::InputText("###roname", name.data(), name.capacity()))
                selected->setName(name);

            ImGui::Separator();

            auto transform = selected->getLocalTransform();
            ImGui::Text("Transform");
            if(ImGui::DragFloat3("position", glm::value_ptr(transform.pos), 0.01, 0.01))
                selected->setPosition(transform.pos);
            if(ImGui::DragFloat3("rotation", glm::value_ptr(transform.rot), 0.01, 0.01))
                selected->setRotation(transform.rot);
            if(ImGui::DragFloat3("scale", glm::value_ptr(transform.scale), 0.01, 0.01))
                selected->setScale(transform.scale);

            ImGui::Separator();

            ImGui::Text("Flags");
            auto isVisible = selected->isVisible();
            if(ImGui::Checkbox("Is visible", &isVisible))
                selected->setVisible(isVisible);
            auto isEnabled = selected->isEnabled();
            if(ImGui::Checkbox("Is enabled", &isEnabled))
                selected->setEnable(isEnabled);

            ImGui::Separator();

            ImGui::Text("Culling");
            auto isCullable = selected->isCullable();
            if(ImGui::Checkbox("Is culling enabled", &isCullable))
                selected->setCullable(isCullable);

            if(isCullable)
            {
                auto cullingType = selected->getCullingType();
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
                selected->setCullingType(cullingType);

                switch (cullingType)
                {
                    case ECullingType::eByPoint: {} break;
                    case ECullingType::eBySphere:  {
                        auto sphereRadius = selected->getCullingRadius();
                        if(ImGui::DragFloat("Culling sphere radius", &sphereRadius, 0.01, 0.01))
                            selected->setCullingRadius(sphereRadius);
                    } break;
                    case ECullingType::eByBox: {
                        auto bounds = selected->getBounds();
                        if(ImGui::DragFloat3("Box Begin", glm::value_ptr(bounds.first), 0.01, 0.01) ||
                        ImGui::DragFloat3("Box End", glm::value_ptr(bounds.second), 0.01, 0.01))
                            selected->setBounds(bounds.first, bounds.second);
                    } break;
                }
            }

            auto objectType = selected->getObjectType();
            switch (objectType)
            {
            case ERenderObjectType::eMesh:{
                auto pMesh = std::dynamic_pointer_cast<CMeshObjectBase>(selected);
                ImGui::Text("Static Mesh");
            }break;
            case ERenderObjectType::eCamera:{
                auto pCamera = std::dynamic_pointer_cast<CCameraComponent>(selected);
                ImGui::Text("Camera");
            }break;
            case ERenderObjectType::ePointLight:{
                auto pPoint = std::dynamic_pointer_cast<CLightComponent>(selected);
                ImGui::Text("Point light");
                auto color = pPoint->getColor();
                if(ImGui::DragFloat3("Color", glm::value_ptr(color)), 0.01, 0.0, 255.0)
                    pPoint->setColor(color);
                auto attenuation = pPoint->getAttenuation();
                if(ImGui::DragFloat("Attenuation", &attenuation), 0.01, 0.1)
                    pPoint->setAttenuation(attenuation);
            }break;
            
            default: break;
            }
        }

        ImGui::End();
    }
}