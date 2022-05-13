#include "InspectorWindow.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/objects/components/MeshComponentBase.h"
#include "graphics/scene/objects/components/camera/CameraComponent.h"
#include "graphics/scene/objects/components/light/LightComponent.h"
#include "graphics/editor/Editor.h"
#include <glm/gtc/type_ptr.hpp>
#include "graphics/editor/CustomControls.h"

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
            ImGui::Text("Culling");
            ImGui::Separator();

            auto isCullable = selected->isCullable();
            if(FControls::CheckBox("Is cullable", &isCullable))
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
                        if(FControls::DragFloat("Sphere radius", &sphereRadius, 0.01, 0.01))
                            selected->setCullingRadius(sphereRadius);
                    } break;
                    case ECullingType::eByBox: {
                        auto bounds = selected->getBounds();
                        if(FControls::DragFloatVec3("Box Begin", bounds.first, 0.01, 0.01) ||
                        FControls::DragFloatVec3("Box End", bounds.second, 0.01, 0.01))
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
                if(FControls::ColorPicker3("Color", color))
                    pPoint->setColor(color);
                auto attenuation = pPoint->getAttenuation();
                if(FControls::DragFloat("Attenuation", &attenuation, 0.01, 0.1))
                    pPoint->setAttenuation(attenuation);
            }break;
            
            default: break;
            }
        }

        ImGui::End();
    }
}