#include "Hierarchy.h"

#include "../../ENTITY/Entity.h"
#include "../../ENTITY/COMPONENT/GeneralComponent.h"
#include "../../EDITOR/WINDOW/Assets.h"
#include "../../EDITOR/WINDOW/Console.h"

static HierarchyWindow hierarchy;
static AssetsWindow* assetsWindow = &AssetsClass();
static ConsoleWindow* consoleWindow = &ConsoleClass();
static AssimpLoader* assimpLoader = &AssimpLoaderClass();

HierarchyWindow& HierarchyClass()
{
	return hierarchy;
}

///////////////////////////////////////////////////////////////

static Entity* ecs = &EntityClass();

void HierarchyWindow::Render()
{
	if (!renderHierarchy)
		return;

	ImGui::Begin("Hierarchy");
	{
		auto view = ecs->registry.view<GeneralComponent>();

		for (auto entity : view)
		{
			if (entity == ecs->root)
				RenderTree(entity);
		}

		if (ImGui::IsWindowHovered() && !ImGui::IsItemHovered())
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
				ecs->selected = entt::null;
	}
	ImGui::End();
}

void HierarchyWindow::RenderTree(entt::entity entity)
{
	auto& genComp = ecs->registry.get<GeneralComponent>(entity);

	///////////////////////////////////////////////////////////

	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick
		| ImGuiTreeNodeFlags_SpanAvailWidth
		| ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_DefaultOpen;

	///////////////////////////////////////////////////////////

	if (genComp.IsParent())
		tree_flags |= ImGuiTreeNodeFlags_Leaf;

	if (ecs->selected == entity)
		tree_flags |= ImGuiTreeNodeFlags_Selected;

	///////////////////////////////////////////////////////////

	if (!genComp.IsActive())
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)entity, tree_flags, genComp.GetName().c_str());

	if (!genComp.IsActive())
		ImGui::PopStyleColor(1);

	///////////////////////////////////////////////////////////

	static const char* type = "DND_DEMO_CELL";

	//// DRAG ////
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload(type, &entity, sizeof(entt::entity));
		ImGui::Text(genComp.GetName().c_str());
		ImGui::EndDragDropSource();
	}

	//// DROP ENTITY ////
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type))
		{
			entt::entity payload_n = *(entt::entity*)payload->Data;
			genComp.AddChild(payload_n);
		}
		ImGui::EndDragDropTarget();
	}

	//// DROP ASS ////
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
		{
			FILEs payload_n = *(FILEs*)payload->Data;
			std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n.file_name;
			if (payload_n.file_type == OBJ || payload_n.file_type == FBX)
			{
				consoleWindow->AddDebugMessage("Loading mesh... %s", buffer.c_str());
				assimpLoader->LoadModel(buffer, entity);
			}
		}
		ImGui::EndDragDropTarget();
	}

	///////////////////////////////////////////////////////////

	if (ImGui::IsItemClicked() && entity != ecs->root)
		ecs->selected = entity;

	///////////////////////////////////////////////////////////

	if (node_open)
	{
		for (size_t i = 0; i < genComp.GetChildren().size(); i++)
			RenderTree(genComp.GetChildren()[i]);
		ImGui::TreePop();
	}
}