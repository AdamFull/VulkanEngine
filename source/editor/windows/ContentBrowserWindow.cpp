#include "ContentBrowserWindow.h"
#include "filesystem/FilesystemHelper.h"

using namespace engine::editor;

CContentBrowserWindow::CContentBrowserWindow()
{
    currentPath = FilesystemHelper::getWorkDir();
    bOverlayState = false;
}

void CContentBrowserWindow::draw()
{
    if (bOverlayState)
    {
        if (!ImGui::Begin("Content browser", &bOverlayState))
        {
            ImGui::End();
            return;
        }

        if (currentPath != FilesystemHelper::getWorkDir())
		{
			if (ImGui::Button("<-"))
			{
				currentPath = currentPath.parent_path();
			}
            ImGui::SameLine();
		}

        auto rp = std::filesystem::relative(currentPath, FilesystemHelper::getWorkDir());
        ImGui::Text(rp.string().c_str());

        static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(currentPath))
		{
            const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, FilesystemHelper::getWorkDir());
			std::string filenameString = relativePath.filename().string();

            ImGui::PushID(filenameString.c_str());
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::Button(("##" + filenameString).c_str(), { thumbnailSize, thumbnailSize });

            if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

            ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					currentPath /= path.filename();

			}
			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
        }

        ImGui::Columns(1);

        ImGui::End();
    }
}