#include "OverlaySceneGraph.h"

namespace Engine
{
    void OverlaySceneGraph::Draw()
    {
        if (!ImGui::Begin(srOverlayName.c_str(), &bOverlayState))
        {
            ImGui::End();
            return;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
        {
            // Iterate placeholder objects (all the same data)
            for (int obj_i = 0; obj_i < 4; obj_i++)
            {
                CreateObject("Object", obj_i);
            }
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        ImGui::End();
    }
    
    void OverlaySceneGraph::CreateObject(std::string name, int id)
    {
        ImGui::PushID(id);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        bool node_open = ImGui::TreeNode("Object", "%s_%u", name.c_str(), id);

        //Including nodes
        if (node_open)
        {
            static float placeholder_members[8] = { 0.0f, 0.0f, 1.0f, 3.1416f, 100.0f, 999.0f };
            for (int i = 0; i < 8; i++)
            {
                ImGui::PushID(i); // Use field index as identifier.
                if (i < 2)
                {
                    CreateObject("Child", 424242);
                }
                else
                {
                    
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}