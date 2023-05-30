#include "Editor.h"

#include "WINDOW/File.h"
#include "WINDOW/Assets.h"
#include "WINDOW/Viewport.h"
#include "WINDOW/Inspector.h"
#include "WINDOW/Hierarchy.h"
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../GAME/Game.h"
#include "WINDOW/Console.h"
#include "../MAIN/Main.h"
#include "WINDOW/About.h"
#include "../SYSTEM/ProjectSceneSystem.h"
#include "../SYSTEM/ScriptingSystem.h"
#include <wincodec.h>
#include "../MODULE/Module.h"

static Editor editor;

Editor& EditorClass()
{
	return editor;
}

///////////////////////////////////////////////////////////////

static DX* dx = &DXClass();
static Entity* ecs = &EntityClass();
static AssetsWindow* assetsWindow = &AssetsClass();
static ViewportWindow* viewportWindow = &ViewportClass();
static HierarchyWindow* hierarchyWindow = &HierarchyClass();
static InspectorWindow* inspectorWindow = &InspectorClass();
static FileWindow* fileWindow = &FileClass();
static Game* game = &GameClass();
static ConsoleWindow* consoleWindow = &ConsoleClass();
static AboutWindow* aboutWindow = &AboutClass();
static ProjectSceneSystem* projectSceneSystem = &ProjectSceneSystemClass();
static ScriptingSystem* scriptingSystem = &ScriptingSystemClass();
static Module* module = &ModuleClass();

static ImVec2 mainMenuBarSize = ImVec2(NULL, NULL);

bool Editor::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (!ImGui_ImplWin32_Init(dx->hwnd)) return false;
	if (!ImGui_ImplDX11_Init(dx->dxDevice, dx->dxDeviceContext)) return false;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	SetStyle();
	SetFont();

	{
		io.Fonts->AddFontDefault();
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
		icons_config.GlyphOffset = ImVec2(0.f, 2.5f);
		icons = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icons_ranges);
	}

	assetsWindow->Init();
	consoleWindow->Init();
	aboutWindow->Init();

	start_time = std::chrono::steady_clock::now();

	return true;
}

void Editor::Render()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		UpdateTimer();

		SetDocking();
		RenderBar();
		RenderDownBar();
		RenderUpBar();
		assetsWindow->Render();
		viewportWindow->Render();
		hierarchyWindow->Render();
		inspectorWindow->Render();
		fileWindow->Render();
		consoleWindow->Render();
		aboutWindow->Render();
		//if (module) module->myFunc(ImGui::GetCurrentContext());
		//ImGui::ShowDemoWindow();
	}
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Editor::Shutdown()
{
	aboutWindow->Shutdown();
	assetsWindow->Shutdown();
	viewportWindow->ReleaseBuffer();

	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Editor::SetProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
}

void Editor::SetFont()
{
	font = ImGui::GetIO().Fonts->AddFontFromFileTTF(DEFAULT_FONT, 16.0f);
}

void Editor::RenderBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		mainMenuBarSize = ImGui::GetWindowSize();

		RenderFileMenuBar();
		RenderEditMenuBar();
		RenderAssetsMenuBar();
		RenderEntityMenuBar();
		RenderScriptMenuBar();
		RenderWindowMenuBar();
		RenderHelpMenuBar();

		ImGui::EndMainMenuBar();
	}
}

void Editor::SetDocking()
{
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + WINDOW_DOWN));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - (WINDOW_DOWN + WINDOW_DOWN)));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace", NULL, window_flags);

	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();
}

void Editor::RenderDownBar()
{
	static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::SetNextWindowPos(ImVec2(0.0f, (float)StarHelpers::GetContextHeight() - WINDOW_DOWN));
	ImGui::SetNextWindowSize(ImVec2((float)StarHelpers::GetContextWidth(), WINDOW_DOWN));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

	ImGui::Begin("DownBar", NULL, window_flags);
	{
		/*
		if (fps < 30)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			ImGui::Text("%i FPS", fps);
			ImGui::SameLine();
			ImGui::PopStyleColor();
		}
		else if (fps < 60)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.4f, 1.0f));
			ImGui::Text("%i FPS", fps);
			ImGui::SameLine();
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::Text("%i FPS", fps);
			ImGui::SameLine();
		}
		*/
		ImGui::Text("%i FPS", fps);
		/*
		ImGui::SameLine();
		ImGui::Text("%f DeltaTime", ImGui::GetIO().DeltaTime);
		*/
		ImGui::SameLine();
		ImGui::Text("%i Entities", (ecs->registry.alive() - 1));
	}
	ImGui::End();
	ImGui::PopStyleVar(2);
}

void Editor::RenderUpBar()
{
	static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::SetNextWindowPos(ImVec2(0.f, mainMenuBarSize.y));
	ImGui::SetNextWindowSize(ImVec2((float)StarHelpers::GetContextWidth(), WINDOW_DOWN));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	static ImVec2 size = ImVec2(WINDOW_DOWN, WINDOW_DOWN);

	ImGui::Begin("UpBar", NULL, window_flags);
	{
		ImGui::PushFont(icons);
		ImGui::Button(ICON_FA_SAVE, size);
		ImGui::SameLine();
		ImGui::Button(ICON_FA_ARROW_CIRCLE_LEFT, size);
		ImGui::SameLine();
		ImGui::Button(ICON_FA_ARROW_CIRCLE_RIGHT, size);
		ImGui::SameLine();

		///////////////////////////////////////////////////////

		if (viewportWindow->GetMode() == ImGuizmo::LOCAL)
		{
			if (ImGui::Button(ICON_FA_CUBE, size))
				viewportWindow->SetMode(ImGuizmo::WORLD);
		}
		else
		{
			if (ImGui::Button(ICON_FA_GLOBE, size))
				viewportWindow->SetMode(ImGuizmo::LOCAL);
		}

		ImGui::SameLine();

		///////////////////////////////////////////////////////

		if (viewportWindow->GetOperation() == ImGuizmo::TRANSLATE)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, myColor);
			ImGui::Button(ICON_FA_ARROWS_ALT, size);
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::Button(ICON_FA_ARROWS_ALT, size))
				viewportWindow->SetOperation(ImGuizmo::TRANSLATE);
		}

		ImGui::SameLine();

		///////////////////////////////////////////////////////

		if (viewportWindow->GetOperation() == ImGuizmo::ROTATE)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, myColor);
			ImGui::Button(ICON_FA_SYNC_ALT, size);
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::Button(ICON_FA_SYNC_ALT, size))
				viewportWindow->SetOperation(ImGuizmo::ROTATE);
		}

		ImGui::SameLine();

		///////////////////////////////////////////////////////

		if (viewportWindow->GetOperation() == ImGuizmo::SCALE)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, myColor);
			ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT, size);
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT, size))
				viewportWindow->SetOperation(ImGuizmo::SCALE);
		}

		ImGui::SameLine();

		///////////////////////////////////////////////////////

		if (viewportWindow->useSnap)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, myColor);
			if (ImGui::Button(ICON_FA_MAGNET, size))
				viewportWindow->useSnap = false;
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::Button(ICON_FA_MAGNET, size))
				viewportWindow->useSnap = true;
		}

		if (ImGui::BeginPopupContextItem())
		{
			ImGui::PushFont(font);
			ImGui::PushItemWidth(64.0f);
			ImGui::DragFloat("##snap", &viewportWindow->snap, 1.0f, 0.0f, FLT_MAX);
			ImGui::PopItemWidth();
			ImGui::PopFont();
			ImGui::EndPopup();
		}

		///////////////////////////////////////////////////////

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_VECTOR_SQUARE, size))
		{
			if (renderState == PrimitiveState::PrimitiveTriangle)
			{
				renderState = PrimitiveState::PrimitiveLine;
				dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			}
			else if (renderState == PrimitiveState::PrimitiveLine)
			{
				renderState = PrimitiveState::PrimitivePoint;
				dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
			}
			else if (renderState == PrimitiveState::PrimitivePoint)
			{
				renderState = PrimitiveState::PrimitiveTriangle;
				dx->dxDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_CAMERA, size))
		{
			/*
			ID3D11Resource* pResource = nullptr;
			viewportWindow->GetShaderResourceView()->GetResource(&pResource);

			DirectX::ScratchImage image;
			if (FAILED(CaptureTexture(dx->dxDevice, dx->dxDeviceContext, pResource, image)))
				StarHelpers::AddLog("Failed to capture texture!");

			if (pResource) pResource->Release();

			if (FAILED(SaveToWICFile(
				image.GetImages(),
				image.GetImageCount(),
				DirectX::WIC_FLAGS_NONE,
				GUID_ContainerFormatBmp,
				L"texture.bmp", &GUID_WICPixelFormat24bppBGR)))
				StarHelpers::AddLog("Failed to save texture file!");

			image.Release();
			*/
		}

		ImGui::SameLine();
		if (game->hide_window)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, myColor);
			if (ImGui::Button(ICON_FA_LEAF, size))
			{
				game->hide_window = false;
				game->SetWindowState(SW_NORMAL);
			}
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::Button(ICON_FA_LEAF, size))
			{
				game->hide_window = true;
				game->SetWindowState(SW_HIDE);
			}
		}

		///////////////////////////////////////////////////////

		ImGui::SameLine();

		if (game->GetGameState() == GameState::GamePlay)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, myColor);
			if (ImGui::Button(ICON_FA_PLAY, size))
				game->StopGame();
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::Button(ICON_FA_PLAY, size))
				game->StartGame(dx->hwnd);
		}

		///////////////////////////////////////////////////////

		ImGui::SameLine();
		ImGui::Button(ICON_FA_PAUSE, size);

		///////////////////////////////////////////////////////

		ImGui::PopFont();
	}
	ImGui::End();
	ImGui::PopStyleVar(5);
}

void Editor::SetStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	/* SIMPLE COLOR SET (18) */

	/*
	ImVec4 text = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	ImVec4 textDisabled = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	ImVec4 windowBg = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	ImVec4 childBg = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	ImVec4 popupBg = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	ImVec4 border = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	ImVec4 borderShadow = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	ImVec4 frameBg = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	ImVec4 frameBgHovered = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	ImVec4 titleBg = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	ImVec4 menuBarBg = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	ImVec4 scrollbarGrab = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	ImVec4 scrollbarGrabHovered = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	ImVec4 sliderGrab = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	ImVec4 headerHovered = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	ImVec4 tab = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	ImVec4 tabHovered = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	ImVec4 plotHistogram = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	*/

	unsigned char offset = 0x10;

	ImVec4 text = ImVec4(1.000f, 1.000f, 1.000f, 1.000f); /* OK */
	ImVec4 textDisabled = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x80 - offset, 0x80 - offset, 0x80 - offset, 0xFF));
	ImVec4 windowBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x2E - offset, 0x2E - offset, 0x2E - offset, 0xFF));
	ImVec4 childBg = ImVec4(0.280f, 0.280f, 0.280f, 0.000f); /* OK */
	ImVec4 popupBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x50 - offset, 0x50 - offset, 0x50 - offset, 0xFF));
	ImVec4 border = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x44 - offset, 0x44 - offset, 0x44 - offset, 0xFF));
	ImVec4 borderShadow = ImVec4(0.000f, 0.000f, 0.000f, 0.000f); /* OK */
	ImVec4 frameBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x29 - offset, 0x29 - offset, 0x29 - offset, 0xFF));
	ImVec4 frameBgHovered = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x33 - offset, 0x33 - offset, 0x33 - offset, 0xFF));
	ImVec4 titleBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x26 - offset, 0x26 - offset, 0x26 - offset, 0xFF));
	ImVec4 menuBarBg = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x32 - offset, 0x32 - offset, 0x32 - offset, 0xFF));
	ImVec4 scrollbarGrab = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x47 - offset, 0x47 - offset, 0x47 - offset, 0xFF));
	ImVec4 scrollbarGrabHovered = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x4D - offset, 0x4D - offset, 0x4D - offset, 0xFF));
	ImVec4 sliderGrab = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x64 - offset, 0x64 - offset, 0x64 - offset, 0xFF));
	ImVec4 headerHovered = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x78 - offset, 0x78 - offset, 0x78 - offset, 0xFF));
	ImVec4 tab = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x19 - offset, 0x19 - offset, 0x19 - offset, 0xFF));
	ImVec4 tabHovered = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x5A - offset, 0x5A - offset, 0x5A - offset, 0xFF));
	ImVec4 plotHistogram = ImGui::ColorConvertU32ToFloat4(IM_COL32(0x95 - offset, 0x95 - offset, 0x95 - offset, 0xFF));

	colors[ImGuiCol_Text] = text;
	colors[ImGuiCol_TextDisabled] = textDisabled;
	colors[ImGuiCol_WindowBg] = windowBg;
	colors[ImGuiCol_ChildBg] = childBg;
	colors[ImGuiCol_PopupBg] = popupBg;
	colors[ImGuiCol_Border] = border;
	colors[ImGuiCol_BorderShadow] = borderShadow;
	colors[ImGuiCol_FrameBg] = frameBg;
	colors[ImGuiCol_FrameBgHovered] = frameBgHovered;
	colors[ImGuiCol_FrameBgActive] = ImVec4(colors[ImGuiCol_ChildBg].x, colors[ImGuiCol_ChildBg].y, colors[ImGuiCol_ChildBg].z, 1.000f);
	colors[ImGuiCol_TitleBg] = titleBg;
	colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
	colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBg];
	colors[ImGuiCol_MenuBarBg] = menuBarBg;
	colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_FrameBg];
	colors[ImGuiCol_ScrollbarGrab] = scrollbarGrab;
	colors[ImGuiCol_ScrollbarGrabHovered] = scrollbarGrabHovered;
	colors[ImGuiCol_ScrollbarGrabActive] = myColor;
	colors[ImGuiCol_CheckMark] = colors[ImGuiCol_Text];
	colors[ImGuiCol_SliderGrab] = sliderGrab;
	colors[ImGuiCol_SliderGrabActive] = myColor;
	colors[ImGuiCol_Button] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.391f);
	colors[ImGuiCol_Header] = colors[ImGuiCol_PopupBg];
	colors[ImGuiCol_HeaderHovered] = headerHovered;
	colors[ImGuiCol_HeaderActive] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = colors[ImGuiCol_SliderGrab];
	colors[ImGuiCol_SeparatorActive] = myColor;
	colors[ImGuiCol_ResizeGrip] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(colors[ImGuiCol_Text].x, colors[ImGuiCol_Text].y, colors[ImGuiCol_Text].z, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = myColor;
	colors[ImGuiCol_Tab] = tab;
	colors[ImGuiCol_TabHovered] = tabHovered;
	colors[ImGuiCol_TabActive] = colors[ImGuiCol_MenuBarBg];
	colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
	colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_MenuBarBg];
	colors[ImGuiCol_DockingPreview] = ImVec4(myColor.x, myColor.y, myColor.z, 0.781f);
	colors[ImGuiCol_DockingEmptyBg] = colors[ImGuiCol_WindowBg];
	colors[ImGuiCol_PlotLines] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_PlotLinesHovered] = myColor;
	colors[ImGuiCol_PlotHistogram] = plotHistogram;
	colors[ImGuiCol_PlotHistogramHovered] = myColor;
	colors[ImGuiCol_TextSelectedBg] = colors[ImGuiCol_ButtonHovered];
	colors[ImGuiCol_DragDropTarget] = myColor;
	colors[ImGuiCol_NavHighlight] = myColor;
	colors[ImGuiCol_NavWindowingHighlight] = myColor;
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(colors[ImGuiCol_BorderShadow].x, colors[ImGuiCol_BorderShadow].y, colors[ImGuiCol_BorderShadow].z, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = colors[ImGuiCol_NavWindowingDimBg];

	/* ADVANCED COLOR SET (50) */

	/*
	colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	*/

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 2.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 4.0f;
}

void Editor::RenderFileMenuBar()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::BeginMenu("New"))
		{
			if (ImGui::MenuItem("Scene"))
				projectSceneSystem->NewScene();
			ImGui::MenuItem("Project");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Open"))
		{
			if (ImGui::MenuItem("Scene"))
				projectSceneSystem->OpenScene();
			ImGui::MenuItem("Project");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem("Scene"))
				projectSceneSystem->SaveScene();
			ImGui::MenuItem("Project");
			ImGui::EndMenu();
		}
		ImGui::Separator();
		if (ImGui::MenuItem("Exit"))
			PostQuitMessage(0);
		ImGui::EndMenu();
	}
}

void Editor::RenderEditMenuBar()
{
	if (ImGui::BeginMenu("Edit"))
	{
		ImGui::MenuItem("Undo");
		ImGui::MenuItem("Redo");
		ImGui::Separator();
		ImGui::MenuItem("Project");
		ImGui::MenuItem("Scene");
		ImGui::EndMenu();
	}
}

void Editor::RenderAssetsMenuBar()
{
	if (ImGui::BeginMenu("Assets"))
	{
		if (assetsWindow->GetOutCore())
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					std::string buffer = assetsWindow->GetNowDirPath() + "\\" + "Folder"; /* path + name */
					unsigned int safe = assetsWindow->GetSafeName(buffer, "");
					std::string str = buffer + std::to_string(safe);
					assetsWindow->CreateDir(str);
					assetsWindow->RefreshDir();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Lua Script"))
				{
					std::string buffer = assetsWindow->GetNowDirPath() + "\\" + "LuaScript"; /* path + name */
					unsigned int safe = assetsWindow->GetSafeName(buffer, LUA);
					std::string str = buffer + std::to_string(safe) + LUA;
					std::string table = "LuaScript" + std::to_string(safe);
					scriptingSystem->CreateScript(str.c_str(), table.c_str());
					assetsWindow->RefreshDir();
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (assetsWindow->GetSelFile() != nullptr)
			{
				if (ImGui::MenuItem("Remove"))
				{
					std::string buffer = assetsWindow->GetNowDirPath() + "\\" + assetsWindow->GetSelFile()->file_name;
					assetsWindow->RemoveAsset(buffer);
					assetsWindow->RefreshDir();
				}
			}
			else
			{
				ImGui::MenuItem("Remove", "", false, false);
			}
		}
		else
		{
			if (ImGui::BeginMenu("Create"))
			{
				ImGui::MenuItem("Folder", "", false, false);
				ImGui::Separator();
				ImGui::MenuItem("Lua Script", "", false, false);
				ImGui::EndMenu();
			}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
		}
		ImGui::EndMenu();
	}
}

void Editor::RenderEntityMenuBar()
{
	if (ImGui::BeginMenu("Entity"))
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Empty"))
			{
				auto entity = ecs->CreateEntity();
				ecs->CreateEmptyEntity(entity);
				ecs->selected = entity;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Cube"))
			{
				auto entity = ecs->CreateEntity();
				ecs->CreateCubeEntity(entity);
				ecs->selected = entity;
			}
			if (ImGui::MenuItem("Sphere"))
			{
				auto entity = ecs->CreateEntity();
				ecs->CreateSphereEntity(entity);
				ecs->selected = entity;
			}
			if (ImGui::MenuItem("Capsule"))
			{
				auto entity = ecs->CreateEntity();
				ecs->CreateCapsuleEntity(entity);
				ecs->selected = entity;
			}
			if (ImGui::MenuItem("Plane"))
			{
				auto entity = ecs->CreateEntity();
				ecs->CreatePlaneEntity(entity);
				ecs->selected = entity;
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Camera"))
			{
				auto entity = ecs->CreateEntity();
				ecs->CreateCameraEntity(entity);
				ecs->selected = entity;
			}
			if (ImGui::MenuItem("TextMesh"))
			{
				auto entity = ecs->CreateEntity();
				ecs->CreateTextMeshEntity(entity);
				ecs->selected = entity;
			}
			ImGui::EndMenu();
		}
		ImGui::Separator();
		if (ecs->selected != entt::null)
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
			{
				ecs->GetComponent<GeneralComponent>(ecs->selected).Destroy();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Up"))
			{
				ecs->GetComponent<GeneralComponent>(ecs->selected).MoveUp();
			}
			if (ImGui::MenuItem("Down"))
			{
				ecs->GetComponent<GeneralComponent>(ecs->selected).MoveDown();
			}
		}
		else
		{
			ImGui::MenuItem("Copy", "", false, false);
			ImGui::MenuItem("Paste", "", false, false);
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::Separator();
			ImGui::MenuItem("Up", "", false, false);
			ImGui::MenuItem("Down", "", false, false);
		}
		ImGui::EndMenu();
	}
}

void Editor::RenderWindowMenuBar()
{
	if (ImGui::BeginMenu("Window"))
	{
		ImGui::MenuItem("Hierarchy", NULL, &hierarchyWindow->renderHierarchy);
		ImGui::MenuItem("Viewport", NULL, &viewportWindow->renderViewport);
		ImGui::MenuItem("Console", NULL, &consoleWindow->renderConsole);
		ImGui::MenuItem("Assets", NULL, &assetsWindow->renderAssets);
		ImGui::MenuItem("Entity", NULL, &inspectorWindow->renderInspector);
		ImGui::MenuItem("File", NULL, &fileWindow->renderFile);
		ImGui::EndMenu();
	}
}

void Editor::RenderHelpMenuBar()
{
	if (ImGui::BeginMenu("Help"))
	{
		ImGui::MenuItem("About", NULL, &aboutWindow->renderAbout);
		ImGui::EndMenu();
	}
}

void Editor::RenderScriptMenuBar()
{
	if (ImGui::BeginMenu("Script"))
	{
		if (ImGui::MenuItem("Recompile"))
		{
			auto view = ecs->registry.view<ScriptingComponent>();
			for (auto entity : view)
				ecs->GetComponent<ScriptingComponent>(entity).RecompileScripts();
		}
		ImGui::EndMenu();
	}
}

void Editor::UpdateTimer()
{
	end_time = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
	if (elapsed.count() >= 1)
	{
		TimedFunction();
		start_time = end_time;
	}
}

void Editor::TimedFunction()
{
	fps = (int)ImGui::GetIO().Framerate;
}