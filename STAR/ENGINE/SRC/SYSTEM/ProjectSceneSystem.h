#pragma once

#include <yaml-cpp/yaml.h>
#include <entt/entt.hpp>

class ProjectSceneSystem
{
public:
	void NewScene();
	void OpenScene();
	void SaveScene();
	void ClearScene();

private:
	void SaveFile(YAML::Emitter& out, const char* filename);
	void SerializeHierarchy(YAML::Emitter& out);
	void SerializeEntity(YAML::Emitter& out, entt::entity entity);
};

ProjectSceneSystem& ProjectSceneSystemClass();