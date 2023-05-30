#pragma once

#include <entt/entt.hpp>
#include "../../IMGUI/imgui.h"

class HierarchyWindow
{
public:
	void Render();
	bool renderHierarchy = true;
private:
	void RenderTree(entt::entity entity);
};

HierarchyWindow& HierarchyClass();