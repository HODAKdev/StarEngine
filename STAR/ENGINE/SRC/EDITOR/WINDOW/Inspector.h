#pragma once

#include <entt/entt.hpp>
#include "../../IMGUI/imgui.h"

class InspectorWindow
{
public:
	void Render();
	bool renderInspector = true;

private:
	void RenderAdd();

private:
	template <typename T>
	void RenderComponent(entt::entity entity);
};

InspectorWindow& InspectorClass();