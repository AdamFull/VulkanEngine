#include "CustomControls.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>

using namespace Engine;

constexpr const float _columnWidth{100.f};

bool FControls::TextInput(const std::string& label, std::string& dest)
{
    bool bValueChanged{false};

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
	ImGui::SetColumnWidth(0, _columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    bValueChanged = ImGui::InputText(("##" + label).c_str(), dest.data(), dest.capacity());

    ImGui::Columns(1);
    ImGui::PopID();

    return bValueChanged;
}

bool FControls::CheckBox(const std::string& label, bool* value)
{
    bool bValueChanged{false};

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
	ImGui::SetColumnWidth(0, _columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    if(*value)
    {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    } 
    else
    {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    }

    bValueChanged = ImGui::Checkbox(("##" + label).c_str(), value);

    ImGui::PopStyleColor(3);

    ImGui::Columns(1);
    ImGui::PopID();

    return bValueChanged;
}

bool FControls::ColorEdit3(const std::string& label, glm::vec3& value)
{
    ImGuiColorEditFlags misc_flags{};
    bool bValueChanged{false};

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
	ImGui::SetColumnWidth(0, _columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    bValueChanged = ImGui::ColorEdit3(("##" + label).c_str(), glm::value_ptr(value), misc_flags);

    ImGui::Columns(1);
    ImGui::PopID();

    return bValueChanged;
}

bool FControls::DragFloatVec3(const std::string& label, glm::vec3& values, float step, float min, float max)
{
    bool bValueChanged{false};
    ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, _columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = boldFont->FontSize + 6.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
	{
        bValueChanged = true;
        values.x = 0.0f;
    }
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	bValueChanged |= ImGui::DragFloat("##X", &values.x, step, min, max, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
	{
        bValueChanged = true;
        values.y = 0.0f;
    }
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	bValueChanged |= ImGui::DragFloat("##Y", &values.y, step, min, max, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
    {
        bValueChanged = true;
        values.z = 0.0f;
    }
		
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	bValueChanged |= ImGui::DragFloat("##Z", &values.z, step, min, max, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
    return bValueChanged;
}

bool FControls::DragFloat(const std::string& label, float* value, float step, float min, float max)
{
    bool bValueChanged{false};
    ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, _columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    float lineHeight = boldFont->FontSize + 6.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("N", buttonSize))
	{
        bValueChanged = true;
        *value = 0.0f;
    }
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	bValueChanged |= ImGui::DragFloat("##N", value, step, min, max, "%.2f");
	//ImGui::PopItemWidth();

    ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();

    return bValueChanged;
}

bool FControls::DragTransform(FTransform& value, float step, float min, float max)
{
    bool bValueChanged{false};
    bValueChanged |= DragFloatVec3("position", value.pos, step, min, max);
    bValueChanged |= DragFloatVec3("rotation", value.rot, step, min, max);
    bValueChanged |= DragFloatVec3("scale", value.scale, step, min, max);
    return bValueChanged;
}