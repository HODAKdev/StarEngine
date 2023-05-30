#pragma once

#undef min
#undef max

#include "entt/entt.hpp"

class Entity
{
public:
	bool Init();

public:
	entt::registry registry;
	entt::entity selected = entt::null;
	entt::entity root = entt::null;

public:
	entt::entity CreateEntity();
	void SetSelectedEntity(entt::entity entity);
	entt::entity GetSelectedEntity();
	void SetRootEntity(entt::entity entity);
	entt::entity GetRootEntity();
	entt::registry& GetRegistry();

public:
	void CreateEmptyEntity(entt::entity entity);
	void CreateCubeEntity(entt::entity entity);
	void CreateSphereEntity(entt::entity entity);
	void CreateCapsuleEntity(entt::entity entity);
	void CreatePlaneEntity(entt::entity entity);

public:
	void CreateCameraEntity(entt::entity entity);
	void CreateTextMeshEntity(entt::entity entity);

public:
	void LoadCoreModels();

public:
	template <typename T>
	void AddComponent(entt::entity entity)
	{
		registry.emplace<T>(entity);
	}
	template <typename T>
	T& GetComponent(entt::entity entity)
	{
		return registry.get<T>(entity);
	}
	template <typename T>
	bool HasComponent(entt::entity entity)
	{
		return registry.any_of<T>(entity);
	}
	template <typename T>
	void RemoveComponent(entt::entity entity)
	{
		registry.remove<T>(entity);
	}
};

Entity& EntityClass();