#pragma once

#include <Windows.h>
#include "../IMGUI/imgui.h"

typedef void (*MYFUNC)(ImGuiContext*);

class Module
{
public:
	bool Init();

public:
	MYFUNC myFunc = nullptr;
};

Module& ModuleClass();