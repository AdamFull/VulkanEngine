#include "HierarchyWindow.h"
#include "graphics/scene/objects/RenderObject.h"
#include "graphics/scene/SceneManager.h"
#include "graphics/editor/Editor.h"

using namespace Engine::Core::Scene;
using namespace Engine::Core::Editor;

void CHierarchyWindow::draw()
{
    if (bOverlayState)
    {
        if (!ImGui::Begin("Hierarchy", &bOverlayState))
        {
            ImGui::End();
            return;
        }

        auto current_size = ImGui::GetWindowSize();
        auto& pRoot = CSceneManager::inst()->getScene()->getRoot();
        buildHierarchy(pRoot);

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        {
            if (ImGui::IsMouseClicked(1))
                ImGui::OpenPopup("HierarchyOptions");
        }

        if (ImGui::BeginPopup("HierarchyOptions"))
	    {
            if (ImGui::MenuItem("copy"));
		    if (ImGui::MenuItem("paste"));
		    if (ImGui::MenuItem("duplicate"));
		    if (ImGui::MenuItem("delete"));

            ImGui::Separator();

            if (ImGui::BeginMenu("create"))
            {
                if (ImGui::BeginMenu("primitive"))
                {
                    if (ImGui::MenuItem("plane"));
                    if (ImGui::MenuItem("cube"));
                    if (ImGui::MenuItem("sphere"));
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("light"))
                {
                    if (ImGui::MenuItem("point light"));
                    if (ImGui::MenuItem("spot light"));
                    if (ImGui::MenuItem("directional light"));
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("mesh"))
                {
                    if (ImGui::MenuItem("static mesh"));
                    if (ImGui::MenuItem("skeletal mesh"));
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }

        int size_x = current_size.x;
        int size_y = current_size.y - ImGui::GetCursorPosY() - 10;
        ImGui::SetCursorPosX(0);
        if (ImGui::InvisibleButton("###unselect_all_btn", ImVec2(size_x, size_y)))
        {
            CEditor::inst()->deselectAll();
        }

        ImGui::End();
    }
}

void CHierarchyWindow::buildHierarchy(std::shared_ptr<CRenderObject> pObject)
{
    auto& childs = pObject->getChilds();
    uint32_t flags = ImGuiTreeNodeFlags_OpenOnArrow;

    //Is object selected
    if (CEditor::inst()->isSelected(pObject))
		flags |= ImGuiTreeNodeFlags_Selected;

    //Has object childs
    if (childs.empty())
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    
    //Is object enabled
    if (!pObject->isEnabled())
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 0.4));

    ImGui::PushID(pObject->getId());
    bool isOpen = ImGui::TreeNodeEx(pObject->getName().c_str(), flags);
    if (!pObject->isEnabled())
		ImGui::PopStyleColor();
    ImGui::PopID();

    //Mouse double click event
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
	{
		//TODO: Add object lookAt
	}

    //Mouse click event
    if (ImGui::IsItemClicked(0))
	{
        CEditor::inst()->deselectAll();
        CEditor::inst()->selectObject(pObject);
    }

    // Ctrl + click
    if(ImGui::IsKeyPressed(ImGuiKey_LeftCtrl) && ImGui::IsItemClicked(0))
    {
        CEditor::inst()->selectObject(pObject);
    }

    if(isOpen && !childs.empty())
    {
        for(auto& [name, child] : childs)
            buildHierarchy(child);
        ImGui::TreePop();
    }   
}