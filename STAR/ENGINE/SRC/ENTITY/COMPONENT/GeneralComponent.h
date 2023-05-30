#pragma once

#include <string>
#include "entt/entt.hpp"
#include "../../IMGUI/imgui.h"
#include "../../ENTITY/Entity.h"
#include "../../HELPERS/Helpers.h"
#include "../../IMGUI/imgui_stdlib.h"
#include <yaml-cpp/yaml.h>

struct GeneralComponent
{
private:
	std::string nameEntity = "Entity";        /* entity name */
	std::string tagEntity = "Empty";          /* entity tag */
	bool activeEntity = true;                 /* if active entity */
	bool staticEntity = false;                /* if static entity */
	entt::entity parentEntity = entt::null;   /* if entity has parent entity */
	std::vector<entt::entity> childrenEntity; /* all children of entity */

public:
	void Render(); /* render component */

public:
	void SetName(std::string name);     /* set name of entity */
	void SetTag(std::string tag);       /* set tag of entity */
	void SetActive(bool arg);           /* set active of entity */
	void SetStatic(bool arg);           /* set static of entity */
	void AddChild(entt::entity entity); /* add child */
	void Destroy();                     /* destroy entity & clear cache */
	void DestroyChild();                /* destroy all entity child & clear cache */

public:
	bool IsParent();                         /* return true if has entity any child */
	std::string GetName();                   /* return entity name */
	std::string GetTag();                    /* return entity tag */
	bool IsActive();                         /* return true if entity is active */
	bool IsStatic();                         /* return true if entity is static */
	entt::entity GetParent();                /* return entity parent */
	std::vector<entt::entity> GetChildren(); /* return entity child */

private:
	void EntitySkip(entt::entity entity, int& size);
	void SetActiveAll(entt::entity entity, bool arg);
	void SetStaticAll(entt::entity entity, bool arg);
	void DestroyAll(entt::entity entity);

public:
	void MoveUp();
	void MoveDown();

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);
};