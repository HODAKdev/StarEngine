#include "Main.h"
#include "../SPLASHSCREEN/SplashScreen.h"
#include <thread>
#include "../GAME/Game.h"
#include <iostream>

//#define XGAME

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

#if defined(_DEBUG)
#pragma comment(lib, "Debug\\PhysX_64.lib")
#pragma comment(lib, "Debug\\PhysXFoundation_64.lib")
#pragma comment(lib, "Debug\\PhysXCommon_64.lib")
#pragma comment(lib, "Debug\\PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "Debug\\PhysXCooking_64.lib")
#pragma comment(lib, "Debug\\PhysXExtensions_static_64.lib")
#pragma comment(lib, "Debug\\yaml-cppd.lib")
#pragma comment(lib, "Debug\\lua-5.4.4.lib")
#pragma comment(lib, "Debug\\assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "Release\\PhysX_64.lib")
#pragma comment(lib, "Release\\PhysXFoundation_64.lib")
#pragma comment(lib, "Release\\PhysXCommon_64.lib")
#pragma comment(lib, "Release\\PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "Release\\PhysXCooking_64.lib")
#pragma comment(lib, "Release\\PhysXExtensions_static_64.lib")
#pragma comment(lib, "Release\\yaml-cpp.lib")
#pragma comment(lib, "Release\\lua-5.4.4.lib")
#pragma comment(lib, "Release\\assimp-vc143-mt.lib")
#endif

static Game* game = &GameClass();
static SplashScreen* splashScreen = &SplashScreenClass();

int StartEngine(HINSTANCE& hInstance, HINSTANCE& hPrevInstance, PWSTR& pCmdLine, int& nCmdShow)
{
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	DX11SetReference(hInstance, hPrevInstance, pCmdLine, nCmdShow);

	if (!splashScreen->ShowSplashScreen())
		return 0;

	if (!DX11CreateWindow(L"StarEngine", 1280, 720))
		return 0;

	if (!DX11CreateContext())
		return 0;

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
		return 0;

	EngineStart();

	splashScreen->HideSplashScreen();

	EngineUpdate();
	EngineShutdown();
	CoUninitialize();

	return 0;
}

int StartGame(HINSTANCE& hInstance, HINSTANCE& hPrevInstance, PWSTR& pCmdLine, int& nCmdShow)
{
	DX11SetReference(hInstance, hPrevInstance, pCmdLine, nCmdShow);

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
		return 0;

	{
		game->StartGame(NULL);

		EngineStart();
		EngineUpdate();
		EngineShutdown();
	}

	CoUninitialize();

	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	freopen("log.txt", "w", stdout);
	std::cout << "..." << std::endl;

#if defined(XGAME)
	return StartGame(hInstance, hPrevInstance, pCmdLine, nCmdShow);
#else
	return StartEngine(hInstance, hPrevInstance, pCmdLine, nCmdShow);
#endif
	return 0;
}