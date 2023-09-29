#include "ProjectSceneSystem.h"
#include "../ENTITY/Entity.h"
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../EDITOR/WINDOW/Viewport.h"
#include <fstream>
#include "../MAIN/Main.h"

#define XFILE "C:\\DX\\STAR\\ENGINE\\data.scene"

static ProjectSceneSystem projectSceneSystem;

ProjectSceneSystem& ProjectSceneSystemClass()
{
	return projectSceneSystem;
}

/* ----------------------------------- */

static Entity* ecs = &EntityClass();
static ViewportWindow* viewportWindow = &ViewportClass();

void ProjectSceneSystem::ClearScene()
{
	ecs->selected = entt::null;
	auto& generalComponent = ecs->GetComponent<GeneralComponent>(ecs->root);
	generalComponent.DestroyChildren();
}
void ProjectSceneSystem::NewScene()
{
	ClearScene();

	entt::entity cube = ecs->CreateEntity();
	ecs->CreateCubeEntity(cube);
	ecs->selected = cube;

	entt::entity camera = ecs->CreateEntity();
	ecs->CreateCameraEntity(camera);
	ecs->GetComponent<TransformComponent>(camera).SetPosition(Vector3(0.0f, 0.0f, -5.0f));

	viewportWindow->SetCamPosition(DEFAULT_CAM_POS);
	viewportWindow->SetCamRotation(DEFAULT_CAM_ROT);
}

void ProjectSceneSystem::SaveScene()
{
	StarHelpers::AddLog("[Scene] -> Saving Scene...");

	YAML::Emitter out;

	out << YAML::BeginMap;
	{
		out << YAML::Key << "STAR" << YAML::Value << YAML::BeginMap;
		{
			out << YAML::Key << "VERSION" << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "MAJOR" << YAML::Value << MAJOR;
				out << YAML::Key << "MINOR" << YAML::Value << MINOR;
				out << YAML::Key << "PATCH" << YAML::Value << PATCH;
			}
			out << YAML::EndMap;
			out << YAML::Key << "DATA" << YAML::Value << YAML::BeginMap;
			{
				SerializeHierarchy(out);
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}
	out << YAML::EndMap;

	SaveFile(out, XFILE);
}

void ProjectSceneSystem::OpenScene()
{
	StarHelpers::AddLog("[Scene] -> Opening Scene...");

	ClearScene();

	YAML::Node in = YAML::LoadFile(XFILE);

	if (!in["STAR"]) return;
}

void ProjectSceneSystem::SerializeEntity(YAML::Emitter& out, entt::entity entity)
{
	auto& parent = ecs->GetComponent<GeneralComponent>(entity);

	out << YAML::BeginMap;
	{
		out << YAML::Key << "ENTITY" << YAML::Value << YAML::BeginMap;
		{
			if (ecs->HasComponent<GeneralComponent>(entity))
				ecs->GetComponent<GeneralComponent>(entity).SerializeComponent(out);
			if (ecs->HasComponent<TransformComponent>(entity))
				ecs->GetComponent<TransformComponent>(entity).SerializeComponent(out);
			if (ecs->HasComponent<MeshComponent>(entity))
				ecs->GetComponent<MeshComponent>(entity).SerializeComponent(out);
		}
		out << YAML::EndMap;
	}
	out << YAML::EndMap;

	for (size_t i = 0; i < parent.GetChildren().size(); i++)
	{
		auto& child = ecs->GetComponent<GeneralComponent>(parent.GetChildren()[i]);
		SerializeEntity(out, parent.GetChildren()[i]);
	}
}

void ProjectSceneSystem::SerializeHierarchy(YAML::Emitter& out)
{
	out << YAML::Key << "HIERARCHY" << YAML::Value << YAML::BeginSeq;
	{
		SerializeEntity(out, ecs->root);
	}
	out << YAML::EndSeq;
}

void ProjectSceneSystem::SaveFile(YAML::Emitter& out, const char* filename)
{
	StarHelpers::AddLog("[Scene] -> Saving File...");
	if (!out.good()) StarHelpers::AddLog("[YAML] -> %s", out.GetLastError().c_str());
	std::ofstream stream(filename);
	stream << out.c_str();
	stream.close();
}