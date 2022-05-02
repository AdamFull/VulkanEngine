#include "OverlaySceneGraph.h"
#include "graphics/scene/objects/RenderObject.h"

using namespace Engine::Core::Overlay;

void COverlaySceneGraph::draw()
{
    if (bOverlayState)
    {
        if (!ImGui::Begin(srOverlayName.c_str(), &bOverlayState))
        {
            ImGui::End();
            return;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::BeginTable("split", 1, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
        {
            auto& sceneObjects = m_pRoot->getChilds();
            uint32_t objCounter{0};
            for (auto& object : sceneObjects)
            {
                if(object.second)
                {
                    createObject(object.second->getName(), object.second, objCounter);
                    objCounter++;
                }
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::End();
    }
}

void COverlaySceneGraph::createObject(std::string name, std::shared_ptr<Scene::CRenderObject> pObject, int id)
{
    ImGui::PushID(id);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Object", "%s_%u", name.c_str(), id);

    // Including nodes
    if (node_open)
    {
        auto& nodeChilds = pObject->getChilds();
        uint32_t objCounter{0};
        for (auto& child : nodeChilds)
        {
            ImGui::PushID(objCounter); // Use field index as identifier.
            if(child.second)
                createObject(child.second->getName(), child.second, objCounter);
            objCounter++;
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}