#include "TextMeshComponent.h"
#include "../Entity.h"
#include "GeneralComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "../../EDITOR/Editor.h"
#include <IconFontCppHeaders/IconsFontAwesome5.h>
#include <filesystem>
#include "../../EDITOR/WINDOW/Assets.h"
#include "../../EDITOR/WINDOW/Console.h"
#include "../../MAIN/Main.h"

static Entity* ecs = &EntityClass();
static Editor* editor = &EditorClass();
static AssetsWindow* assetsWindow = &AssetsClass();
static ConsoleWindow* consoleWindow = &ConsoleClass();

void TextMeshComponent::Render()
{
	if (ImGui::CollapsingHeader("TEXTMESH", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
			{
				entt::entity entity = entt::to_entity(ecs->registry, *this);
				ecs->registry.remove<TextMeshComponent>(entity);
			}
			ImGui::EndPopup();
		}

		/////////////////////////////////////////////////////////////

		if (ImGui::BeginTable("TextMeshComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Text");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Font");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Quality");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Align");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Mesh");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("	Extrude");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Spacing");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("	Character");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("	Word");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("	Line");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				ImGui::InputTextMultiline("##TextMeshComponent", &buffer, ImVec2(0.0f, 22.0f));
				if (ImGui::IsItemDeactivated()) SetupTextMesh();

				if (ImGui::Button(fontPath.empty() ? "None" : std::filesystem::path(fontPath).stem().string().c_str()))
					SetFont("");

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
					{
						FILEs payload_n = *(FILEs*)payload->Data;
						std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n.file_name;
						if (payload_n.file_type == TTF) SetFont(buffer.c_str());
					}
					ImGui::EndDragDropTarget();
				}

				if (ImGui::Combo("##QualityMeshComponent", &item_current, items, IM_ARRAYSIZE(items)))
				{
					if (item_current == 0) SetQuality(MeshQuality::MeshHigh);
					if (item_current == 1) SetQuality(MeshQuality::MeshNormal);
					if (item_current == 2) SetQuality(MeshQuality::MeshLow);
				}

				ImGui::PushFont(editor->icons);
				{
					if (alignGlyph == AlignGlyph::AlignLeft)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, editor->myColor);
						ImGui::Button(ICON_FA_ALIGN_LEFT, ImVec2(22.0f, 22.0f));
						ImGui::PopStyleColor();
					}
					else
					{
						if (ImGui::Button(ICON_FA_ALIGN_LEFT, ImVec2(22.0f, 22.0f)))
							alignGlyph = AlignGlyph::AlignLeft;
					}

					ImGui::SameLine();

					if (alignGlyph == AlignGlyph::AlignCenter)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, editor->myColor);
						ImGui::Button(ICON_FA_ALIGN_CENTER, ImVec2(22.0f, 22.0f));
						ImGui::PopStyleColor();
					}
					else
					{
						if (ImGui::Button(ICON_FA_ALIGN_CENTER, ImVec2(22.0f, 22.0f)))
							alignGlyph = AlignGlyph::AlignCenter;
					}

					ImGui::SameLine();

					if (alignGlyph == AlignGlyph::AlignRight)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, editor->myColor);
						ImGui::Button(ICON_FA_ALIGN_RIGHT, ImVec2(22.0f, 22.0f));
						ImGui::PopStyleColor();
					}
					else
					{
						if (ImGui::Button(ICON_FA_ALIGN_RIGHT, ImVec2(22.0f, 22.0f)))
							alignGlyph = AlignGlyph::AlignRight;
					}
				}
				ImGui::PopFont();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

				ImGui::DragFloat("##MeshDepthMeshComponent", &meshDepth, 0.1f, 0.0f, FLT_MAX);
				if (ImGui::IsItemDeactivated()) SetupTextMesh();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

				if (ImGui::Checkbox("##AutoCharSpacing", &autoCharSpacing))
					SetupTextMesh();
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Auto");
				ImGui::SameLine();
				if (autoCharSpacing) ImGui::BeginDisabled();
				{
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
					{
						ImGui::DragFloat("##CharacterSpacingMeshComponent", &charSpacing, 0.1f, 0.0f, FLT_MAX);
						if (ImGui::IsItemDeactivated()) SetupTextMesh();
					}
					ImGui::PopItemWidth();
				}
				if (autoCharSpacing) ImGui::EndDisabled();

				ImGui::DragFloat("##WordSpacingMeshComponent", &wordSpacing, 0.1f, 0.0f, FLT_MAX);
				if (ImGui::IsItemDeactivated()) SetupTextMesh();

				ImGui::DragFloat("##LineSpacingMeshComponent", &lineSpacing, 0.1f, 0.0f, FLT_MAX);
				if (ImGui::IsItemDeactivated()) SetupTextMesh();
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

bool TextMeshComponent::SetupTextMesh()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this); /* get this entity */
	ecs->registry.get<GeneralComponent>(entity).DestroyChildren();  /* destroy all child */

	offset = ImVec2(NULL, NULL);

	{
		ttf_t* font = NULL;
		if (ttf_load_from_file(fontPath.empty() ? DEFAULT_FONT : fontPath.c_str(), &font, false) != TTF_DONE) /* load font */
			return false;

		y_max = GetMax(font);

		for (size_t i = 0; i < buffer.size(); i++)
		{
			if (buffer[i] == '\n') /* is new line */
			{
				if (lineSpacing == -1.0f)
				{
					offset.y = offset.y - y_max - 0.1f;
					offset.x = 0.0f;
				}
				else
				{
					offset.y = offset.y - lineSpacing;
					offset.x = 0.0f;
				}

				continue;
			}
			else if (isspace(buffer[i])) /* is space */
			{
				offset.x = offset.x + wordSpacing;
				continue;
			}
			else if (buffer[i] == '\0') /* is null char */
			{
				continue;
			}
			else
			{
				if (!isascii(buffer[i])) /* only ascii */
				{
					continue;
				}
			}

			int index = ttf_find_glyph(font, buffer[i]); /* find glyph from letter */
			if (index < TTF_DONE) return false;

			ttf_mesh3d_t* out = NULL;
			if (ttf_glyph2mesh3d(&font->glyphs[index], &out, GetQuality(), TTF_FEATURES_DFLT, meshDepth) != TTF_DONE) /* create mesh */
				return false;

			auto glyph = ecs->registry.create();
			ecs->registry.emplace<GeneralComponent>(glyph);
			ecs->registry.emplace<TransformComponent>(glyph);
			ecs->registry.emplace<MeshComponent>(glyph);
			ecs->registry.get<GeneralComponent>(glyph).SetName(std::to_string(buffer[i]));
			ecs->registry.get<GeneralComponent>(entity).AddChild(glyph);

			ecs->registry.get<TransformComponent>(glyph).SetPosition(Vector3(offset.x, offset.y, 0.0f));

			if (autoCharSpacing) offset.x = offset.x + 0.0f + font->glyphs[index].xbounds[1];
			else offset.x = offset.x + charSpacing;

			auto& meshComponent = ecs->registry.get<MeshComponent>(glyph);

			{
				for (int i = 0; i < out->nvert; i++) /* apply verts */
				{
					Vertex vertex;
					vertex.position.x = out->vert[i].x;
					vertex.position.y = out->vert[i].y;
					vertex.position.z = out->vert[i].z;
					vertex.normal.x = out->normals[i].x;
					vertex.normal.y = out->normals[i].y;
					vertex.normal.z = out->normals[i].z;
					meshComponent.AddVertices(vertex);
				}

				for (int i = 0; i < out->nfaces; i++) /* apply faces */
				{
					meshComponent.AddIndices(out->faces[i].v1);
					meshComponent.AddIndices(out->faces[i].v2);
					meshComponent.AddIndices(out->faces[i].v3);
				}

				meshComponent.SetupMesh();
			}

			ttf_free_mesh3d(out);
		}

		ttf_free(font);
	}

	return true;
}

void TextMeshComponent::SetQuality(unsigned char quality)
{
	meshQuality = quality;
	SetupTextMesh();
}
unsigned char TextMeshComponent::GetQuality()
{
	return meshQuality;
}
void TextMeshComponent::SetAlign(unsigned char align)
{
	alignGlyph = align;
	SetupTextMesh();
}
unsigned char TextMeshComponent::GetAlign()
{
	return alignGlyph;
}
void TextMeshComponent::SetFont(const char* path)
{
	fontPath = path;
	SetupTextMesh();
}
const char* TextMeshComponent::GetFont()
{
	return fontPath.c_str();
}
void TextMeshComponent::SetText(const char* text)
{
	buffer = text;
	SetupTextMesh();
}
const char* TextMeshComponent::GetText()
{
	return buffer.c_str();
}
float TextMeshComponent::GetMax(ttf_t* ttf)
{
	float max = 0.0f;
	std::string string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (size_t i = 0; i < string.size(); i++)
	{
		int index = ttf_find_glyph(ttf, string[i]);
		float y = ttf->glyphs[index].ybounds[1];
		max = std::max(max, y);
	}

	return max;
}
void TextMeshComponent::SetMeshDepth(float value)
{
	meshDepth = value;
	SetupTextMesh();
}
float TextMeshComponent::GetMeshDepth()
{
	return meshDepth;
}
void TextMeshComponent::SetCharSpacing(float value)
{
	charSpacing = value;
	SetupTextMesh();
}
float TextMeshComponent::GetCharSpacing()
{
	return charSpacing;
}
void TextMeshComponent::SetLineSpacing(float value)
{
	lineSpacing = value;
	SetupTextMesh();
}
float TextMeshComponent::GetLineSpacing()
{
	return lineSpacing;
}
void TextMeshComponent::SetWordSpacing(float value)
{
	wordSpacing = value;
	SetupTextMesh();
}
float TextMeshComponent::GetWordSpacing()
{
	return wordSpacing;
}