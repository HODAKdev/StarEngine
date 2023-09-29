#include "GeneralComponent.h"
#include "../../ENTITY/COMPONENT/MeshComponent.h"

static Entity* ecs = &EntityClass();

void GeneralComponent::Render()
{
	if (ImGui::CollapsingHeader("GENERAL", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("GeneralComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Active");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Name");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Static");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Tag");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				bool _Active = IsActive();
				if (ImGui::Checkbox("##ActiveGeneralComponent", &_Active))
					if (_Active) SetActive(true); else SetActive(false);

				//ImVec2 size = ImGui::GetItemRectSize();
				//StarHelpers::AddLog("%f", size.y);

				std::string _Name = GetName();
				if (ImGui::InputText("##NameGeneralComponent", &_Name, ImGuiInputTextFlags_EnterReturnsTrue))
					SetName(_Name);

				bool _Static = IsStatic();
				if (ImGui::Checkbox("##StaticGeneralComponent", &_Static))
					if (_Static) SetStatic(true); else SetStatic(false);

				std::string _Tag = GetTag();
				if (ImGui::InputText("##TagGeneralComponent", &_Tag, ImGuiInputTextFlags_EnterReturnsTrue))
					SetTag(_Tag);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

void GeneralComponent::SetName(std::string name)
{
	if (!name.compare(""))
		return;

	nameEntity = name;
}
void GeneralComponent::SetTag(std::string tag)
{
	if (!tag.compare(""))
		return;

	tagEntity = tag;
}
std::vector<entt::entity> GeneralComponent::GetChildren()
{
	return childrenEntity;
}
entt::entity GeneralComponent::GetParent()
{
	return parentEntity;
}
bool GeneralComponent::IsStatic()
{
	return staticEntity;
}
bool GeneralComponent::IsActive()
{
	return activeEntity;
}
std::string GeneralComponent::GetTag()
{
	return tagEntity;
}
std::string GeneralComponent::GetName()
{
	return nameEntity;
}
bool GeneralComponent::HasChildren()
{
	if (childrenEntity.empty()) return true; else return false;
}

void GeneralComponent::SetActiveAll(entt::entity entity, bool arg)
{
	auto& generalComp = ecs->registry.get<GeneralComponent>(entity);
	generalComp.activeEntity = arg;

	for (int i = 0; i < generalComp.GetChildren().size(); i++)
		SetActiveAll(generalComp.GetChildren()[i], arg);
}

void GeneralComponent::SetActive(bool arg)
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	SetActiveAll(entity, arg);
}

void GeneralComponent::SetStaticAll(entt::entity entity, bool arg)
{
	auto& generalComp = ecs->registry.get<GeneralComponent>(entity);
	generalComp.staticEntity = arg;

	for (int i = 0; i < generalComp.GetChildren().size(); i++)
		SetStaticAll(generalComp.GetChildren()[i], arg);
}

void GeneralComponent::SetStatic(bool arg)
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	SetStaticAll(entity, arg);
}

void GeneralComponent::EntitySkip(entt::entity entity, int& size)
{
	auto& generalComp = ecs->registry.get<GeneralComponent>(entity);
	entt::entity thisEntity = entt::to_entity(ecs->registry, *this);

	std::vector<entt::entity>::iterator find = std::find(generalComp.childrenEntity.begin(), generalComp.childrenEntity.end(), thisEntity);
	if (find != generalComp.childrenEntity.end())
		size++;

	for (int i = 0; i < generalComp.childrenEntity.size(); i++)
		EntitySkip(generalComp.childrenEntity[i], size);
}

void GeneralComponent::AddChild(entt::entity entity)
{
	auto& genComp = ecs->registry.get<GeneralComponent>(entity);

	//// if move root so return code ////
	if (entity == ecs->root)
		return;

	//// 2. issue fix ////
	if (genComp.parentEntity == entt::null)
	{
		genComp.parentEntity = entt::to_entity(ecs->registry, *this);
		childrenEntity.push_back(entity);
	}
	else
	{
		//// 3. issue fix ////
		int size = 0;
		EntitySkip(entity, size);
		if (size != 0)
			return;

		auto& parent = ecs->registry.get<GeneralComponent>(genComp.parentEntity);

		//// 4. issue fix ////
		std::vector<entt::entity>::iterator old = std::find(parent.childrenEntity.begin(), parent.childrenEntity.end(), entity);
		if (old != parent.childrenEntity.end())
		{
			auto index = std::distance(parent.childrenEntity.begin(), old);
			parent.childrenEntity.erase(parent.childrenEntity.begin() + index);
		}

		//// 5. issue fix ////
		std::vector<entt::entity>::iterator back = std::find(childrenEntity.begin(), childrenEntity.end(), entity);
		if (back == childrenEntity.end())
		{
			genComp.parentEntity = entt::to_entity(ecs->registry, *this);
			childrenEntity.push_back(entity);
		}
	}
}

static std::vector<entt::entity> toDestroy;

void GeneralComponent::Destroy()
{
	ecs->selected = entt::null;
	entt::entity entity = entt::to_entity(ecs->registry, *this);

	///////////////////////////////////////////////////////////////

	auto& parent = ecs->registry.get<GeneralComponent>(parentEntity);
	std::vector<entt::entity>::iterator old = std::find(parent.childrenEntity.begin(), parent.childrenEntity.end(), entity);

	///////////////////////////////////////////////////////////////

	DestroyAll(entity);

	for (size_t i = 0; i < toDestroy.size(); i++)
		ecs->registry.destroy(toDestroy[i]);

	toDestroy.clear();

	///////////////////////////////////////////////////////////////

	if (old != parent.childrenEntity.end())
	{
		auto index = std::distance(parent.childrenEntity.begin(), old);
		parent.childrenEntity.erase(parent.childrenEntity.begin() + index);
	}
}

void GeneralComponent::DestroyAll(entt::entity entity)
{
	auto& generalComp = ecs->registry.get<GeneralComponent>(entity);

	toDestroy.push_back(entity);

	for (size_t i = 0; i < generalComp.childrenEntity.size(); i++)
		DestroyAll(generalComp.childrenEntity[i]);
}

void GeneralComponent::DestroyChildren()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	DestroyAll(entity);

	toDestroy.erase(toDestroy.begin());

	for (size_t i = 0; i < toDestroy.size(); i++)
		ecs->registry.destroy(toDestroy[i]);

	toDestroy.clear();
	childrenEntity.clear();
}

void GeneralComponent::MoveUp()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this); /* get entity from component */
	auto& parent = ecs->registry.get<GeneralComponent>(GetParent()); /* get parent from this entity */
	std::vector<entt::entity>::iterator iterator = std::find(parent.childrenEntity.begin(), parent.childrenEntity.end(), entity);
	if (iterator != parent.childrenEntity.end())
	{
		size_t index = std::distance(parent.childrenEntity.begin(), iterator);
		if (index == NULL) /* completely up so return */
			return;

		std::swap(parent.childrenEntity[index], parent.childrenEntity[index - 1]);
	}
}

void GeneralComponent::MoveDown()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this); /* get entity from component */
	auto& parent = ecs->registry.get<GeneralComponent>(GetParent()); /* get parent from this entity */
	std::vector<entt::entity>::iterator iterator = std::find(parent.childrenEntity.begin(), parent.childrenEntity.end(), entity);
	if (iterator != parent.childrenEntity.end())
	{
		size_t index = std::distance(parent.childrenEntity.begin(), iterator);
		if (index == parent.childrenEntity.size() - 1) /* completely down so return */
			return;

		std::swap(parent.childrenEntity[index], parent.childrenEntity[index + 1]);
	}
}

void GeneralComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::Key << "GeneralComponent";
	out << YAML::BeginMap;
	{
		out << YAML::Key << "Name" << YAML::Value << nameEntity;
		out << YAML::Key << "Tag" << YAML::Value << tagEntity;
		out << YAML::Key << "Active" << YAML::Value << activeEntity;
		out << YAML::Key << "Static" << YAML::Value << staticEntity;
		out << YAML::Key << "Children" << YAML::Value << childrenEntity.size();
	}
	out << YAML::EndMap;
}

void GeneralComponent::DeserializeComponent(YAML::Node& in)
{
	nameEntity = in["Name"].as<std::string>();
	tagEntity = in["Tag"].as<std::string>();
	activeEntity = in["Active"].as<bool>();
	staticEntity = in["Static"].as<bool>();
}