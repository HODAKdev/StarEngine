#pragma once

//#define SOL_ALL_SAFETIES_ON 1
#define SOL_PRINT_ERRORS 1
//#define SOL_EXCEPTIONS 1
#include <sol/sol.hpp>
#include <entt/entt.hpp>

struct EntityX
{
	entt::entity entity = entt::null;

	void CreateEntity();
	void AddComponent(const char* component_name);
	sol::object GetComponent(const char* component_name);
	bool HasComponent(const char* component_name);
	void RemoveComponent(const char* component_name);
};

class ScriptingSystem
{
public:
	bool Init();
	sol::state& GetState();
	void CreateScript(const char* filename, const char* name);

private:
	sol::state lua;

private:
	/* system */
	void lua_add_vector2();
	void lua_add_vector3();
	void lua_add_vector4();
	void lua_add_quaternion();
	void lua_add_matrix();
	void lua_add_console();
	void lua_add_time();
	void lua_add_input();
	void lua_add_bounding_box();

	/* entity */
	void lua_add_entity();
	void lua_add_general_component();
	void lua_add_transform_component();
	void lua_add_camera_component();
	void lua_add_rigidbody_component();
	void lua_add_mesh_component();
	void lua_add_text_mesh_component();
};

ScriptingSystem& ScriptingSystemClass();

struct ScriptBuffer
{
public:
	std::string filePath;
	std::string fileName;
	std::string fileNameToUpper; /* for imgui */

public:
	bool activeComponent = true;
	bool error = false;
};

struct ScriptingComponent
{
public:
	void Render();
	void AddScript(const char* path);
	void RecompileScripts();

public:
	void lua_call_start();
	void lua_call_update();

private:
	void lua_add_entity_from_component();

private:
	std::vector<ScriptBuffer> scripts;
};