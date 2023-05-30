#pragma once

#include <chrono>
#include "../DX/DX.h"
#include "../IMGUI/imgui.h"
#include "../HELPERS/Helpers.h"
#include "../IMGUI/imgui_impl_dx11.h"
#include "../IMGUI/imgui_impl_win32.h"
#include <IconFontCppHeaders/IconsFontAwesome5.h>

#define WINDOW_DOWN 32

enum PrimitiveState : unsigned char
{
	PrimitiveTriangle = 0x01,
	PrimitiveLine = 0x02,
	PrimitivePoint = 0x03,
};

class Editor
{
public:
	bool Init();
	void Render();
	void Shutdown();

public:
	void SetProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void SetStyle();
	void SetFont();
	void RenderBar();
	void RenderDownBar();
	void RenderUpBar();

private:
	void SetDocking();
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	bool opt_fullscreen = true;
	bool opt_padding = false;
	unsigned char renderState = PrimitiveState::PrimitiveTriangle;

public:
	ImFont* font = nullptr;
	ImFont* icons = nullptr;

public:
	ImVec4 myColor = ImVec4(ImGui::ColorConvertU32ToFloat4(IM_COL32(0xE2, 0x52, 0x52, 0xFF)));

private:
	void RenderFileMenuBar();
	void RenderEditMenuBar();
	void RenderAssetsMenuBar();
	void RenderEntityMenuBar();
	void RenderWindowMenuBar();
	void RenderHelpMenuBar();
	void RenderScriptMenuBar();

private:
	int fps = 0;
	void UpdateTimer();
	void TimedFunction();
	std::chrono::steady_clock::time_point start_time;
	std::chrono::steady_clock::time_point end_time;
};

Editor& EditorClass();