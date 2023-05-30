#include "File.h"
#include "Assets.h"

static FileWindow file;

FileWindow& FileClass()
{
	return file;
}

///////////////////////////////////////////////////////////////

static AssetsWindow* assetsWindow = &AssetsClass();
static Sky* sky = &SkyClass();

void FileWindow::Render()
{
	if (!renderFile)
		return;

	ImGui::Begin("File");
	{
		FILEs* selected = assetsWindow->GetSelFile();

		if (selected != nullptr)
		{
			if (!selected->is_dir)
			{
				ImGui::Text("File name: %s", selected->file_name.c_str());
				ImGui::Text("File size: %i KB", selected->file_size / 1024); /* B TO KB */
				ImGui::Separator();
			}
			if (selected->file_type == PNG || selected->file_type == DDS)
			{
				/* code */
			}
			else
			{
				if (!selected->is_dir)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
					ImGui::Text("This file is unknown.");
					ImGui::PopStyleColor();
				}
			}
		}
	}
	ImGui::End();
}