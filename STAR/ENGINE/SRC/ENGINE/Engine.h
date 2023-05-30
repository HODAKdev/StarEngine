#pragma once

#include <string>
#include <Windows.h>
#include "../DX/DX.h"
#include "../SKY/Sky.h"
#include "../EDITOR/Editor.h"
#include "../HELPERS/Helpers.h"

void DX11SetReference(HINSTANCE& hInstance, HINSTANCE& hPrevInstance, PWSTR& pCmdLine, int& nCmdShow);
bool DX11CreateWindow(std::wstring name, int width, int height);
LRESULT CALLBACK DX11WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool DX11CreateContext();
bool DX11CreateRenderTargetView();
bool DX11CreateDepthStencilView();
bool DX11ResizeBuffer();
void SetRenderTarget(Vector4 color);
void EngineStart();
void EngineUpdate();
void EngineProcess();
void EngineShutdown();
void UpdateTransform(entt::entity entity);
void GamePlayUpdate();
void RenderToMainBuffer();
void RenderEnvironment(Matrix projectionMatrix, Matrix viewMatrix, Vector4 color, ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView, D3D11_VIEWPORT viewport, IDXGISwapChain* swapChain);
bool FindGoodCamera(Matrix& projectionMatrix, Matrix& viewMatrix);