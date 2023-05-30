#include "Game.h"
#include "../HELPERS/Helpers.h"
#include "../DX/DX.h"
#include "../ENTITY/Entity.h"
#include "../SYSTEM/ScriptingSystem.h"

static Game game;

Game& GameClass()
{
	return game;
}

///////////////////////////////////////////////////////////////

static DX* dx = &DXClass();
static Entity* ecs = &EntityClass();

bool Game::StartGame(HWND parent)
{
    if (!GameCreateWindow())
        return false;

    if (!InitInput())
        return false;

    if (!GameCreateContext(parent))
        return false;

    gameState = GameState::GamePlay;

    /* start frameTime, deltaTime, elapsedTime and frameCount */
    StartTime();

    auto view = ecs->registry.view<ScriptingComponent>();
    for (auto entity : view)
        ecs->registry.get<ScriptingComponent>(entity).lua_call_start();

    return true;
}

LRESULT CALLBACK GameWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        game.StopGame();
        break;
    case WM_SIZE:
        game.GameResizeBuffer();
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return NULL;
}

bool Game::GameCreateWindow()
{
    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof(WNDCLASSEX));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = GameWindowProc;
    wcex.cbClsExtra = NULL;
    wcex.cbWndExtra = NULL;
    wcex.hInstance = *dx->hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(46, 46, 46));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = name.c_str();
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
        return false;

    ///////////////////////////////////////////////////////////

    int x = (StarHelpers::GetDisplayWidth() - width) / 2;
    int y = (StarHelpers::GetDisplayHeight() - height) / 2;

    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    hwnd = CreateWindowEx(
        NULL,
        name.c_str(),
        name.c_str(),
        dwStyle,
        x, y,
        width, height,
        NULL,
        NULL,
        *dx->hInstance,
        NULL
    );

    if (!hwnd)
        return false;

    if (hide_window) ShowWindow(hwnd, SW_HIDE);
    else ShowWindow(hwnd, SW_NORMAL);

    ///////////////////////////////////////////////////////////

    return true;
}

void Game::StopGame()
{
    gameState = GameState::GameNone;

    /* stop frameTime, deltaTime, elapsedTime and frameCount */
    StopTime();
    ResetTime();

    ReleaseInput();
    DestroyWindow(hwnd);
    UnregisterClass(name.c_str(), *dx->hInstance);

    if (IsCursorHidden()) HideCursor(false);
    if (IsCursorLocked()) LockCursor(false);

    if (gameSwapChain) gameSwapChain->Release();
    if (gameRenderTargetView) gameRenderTargetView->Release();
    if (gameDepthStencilView) gameDepthStencilView->Release();

    hwnd = NULL;
}

bool Game::GameCreateContext(HWND parent)
{
    if (parent)
    {
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));

        sd.BufferCount = 2;
        sd.BufferDesc.Width = GameGetContextWidth();
        sd.BufferDesc.Height = GameGetContextHeight();
        sd.BufferDesc.Format = dx->dxFormat;
        //sd.BufferDesc.RefreshRate.Numerator = 60;
        //sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = dx->dxSwapEffect;

        UINT flags = 0;

#if defined(_DEBUG)
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        IDXGIDevice* device = NULL;
        dx->dxDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&device);
        IDXGIAdapter* adapter = NULL;
        device->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter);
        IDXGIFactory* factory = NULL;
        adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);

        factory->CreateSwapChain(dx->dxDevice, &sd, &gameSwapChain);

        if (device) device->Release();
        if (adapter) adapter->Release();
        if (factory) factory->Release();
    }
    else
    {
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));

        sd.BufferCount = 2;
        //sd.BufferDesc.Width = 0;
        //sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = dx->dxFormat;
        //sd.BufferDesc.RefreshRate.Numerator = 60;
        //sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = dx->dxSwapEffect;

        UINT flags = 0;

#if defined(_DEBUG)
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        ///////////////////////////////////////////////////////////

        if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, dx->dxDriverType, NULL, flags, NULL, NULL,
            D3D11_SDK_VERSION, &sd, &gameSwapChain, &dx->dxDevice,
            &dx->dxFeatureLevel, &dx->dxDeviceContext)))
            return false;
    }

    ///////////////////////////////////////////////////////////

    if (!GameCreateRenderTargetView())
        return false;

    ///////////////////////////////////////////////////////////

    if (!GameCreateDepthStencilView())
        return false;

    ///////////////////////////////////////////////////////////

    return true;
}

bool Game::GameCreateRenderTargetView()
{
    ID3D11Texture2D* tex = nullptr;
    if (FAILED(gameSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&tex)))
        return false;

    ///////////////////////////////////////////////////////////

    if (FAILED(dx->dxDevice->CreateRenderTargetView(tex, NULL, &gameRenderTargetView)))
        return false;

    ///////////////////////////////////////////////////////////

    tex->Release();
    return true;
}

bool Game::GameCreateDepthStencilView()
{
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(D3D11_TEXTURE2D_DESC));

    descDepth.Width = GameGetContextWidth();
    descDepth.Height = GameGetContextHeight();
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    ID3D11Texture2D* tex = nullptr;
    if (FAILED(dx->dxDevice->CreateTexture2D(&descDepth, NULL, &tex)))
        return false;

    ///////////////////////////////////////////////////////////

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    if (FAILED(dx->dxDevice->CreateDepthStencilView(tex, &descDSV, &gameDepthStencilView)))
        return false;

    ///////////////////////////////////////////////////////////

    tex->Release();
    return true;
}

unsigned char Game::GetGameState()
{
    return gameState;
}

UINT Game::GameGetContextWidth()
{
    /*
    if (hwnd == NULL)
        return width;

    RECT rc;
    GetClientRect(hwnd, &rc);
    UINT width = rc.right - rc.left;
    return width;
    */
    return width;
}

UINT Game::GameGetContextHeight()
{
    /*
    if (hwnd == NULL)
        return height;

    RECT rc;
    GetClientRect(hwnd, &rc);
    UINT height = rc.bottom - rc.top;
    return height;
    */
    return height;
}

bool Game::InitInput()
{
    if (FAILED(DirectInput8Create(*dx->hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&gDirectInput,
        NULL)))
        return false;

    if (FAILED(gDirectInput->CreateDevice(GUID_SysKeyboard,
        &DIKeyboard,
        NULL)))
        return false;

    if (FAILED(gDirectInput->CreateDevice(GUID_SysMouse,
        &DIMouse,
        NULL)))
        return false;

    if (FAILED(DIKeyboard->SetDataFormat(&c_dfDIKeyboard)))
        return false;

    if (FAILED(DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
        return false;

    if (FAILED(DIMouse->SetDataFormat(&c_dfDIMouse)))
        return false;

    if (FAILED(DIMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
        return false;

    return true;
}
bool Game::InputGetKey(unsigned char key)
{
    if (DIKeyboard)
    {
        DIKeyboard->Acquire();
        uint8_t keyboardState[256];
        ZeroMemory(&keyboardState, sizeof(uint8_t));
        DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
        if (keyboardState[key] & 0x80) return true;
    }

    return false;
}
Vector2 Game::GetCursorAxis()
{
    if (DIMouse)
    {
        DIMouse->Acquire();
        DIMOUSESTATE mouseState;
        ZeroMemory(&mouseState, sizeof(DIMOUSESTATE));
        DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
        return Vector2((float)mouseState.lX, (float)mouseState.lY);
    }

    return Vector2::Zero;
}
void Game::ReleaseInput()
{
    if (gDirectInput) gDirectInput->Release();
    if (DIKeyboard) DIKeyboard->Release(); DIKeyboard = NULL;
    if (DIMouse) DIMouse->Release(); DIKeyboard = NULL;
}

void Game::InitTime()
{
    QueryPerformanceFrequency(&frequency);
}
void Game::BeginTime()
{
    if (!timeState) return;
    QueryPerformanceCounter(&t1);
}
void Game::EndTime()
{
    if (!timeState) return;

    QueryPerformanceCounter(&t2);
    frameTime = (t2.QuadPart - t1.QuadPart) * 1000.0f / frequency.QuadPart;
    deltaTime = frameTime / 1000.0f;
    elapsedTime += deltaTime;
    frameCount++;
    float fps = 1000.0f / frameTime;
}
void Game::StartTime()
{
    timeState = true;
}
void Game::StopTime()
{
    timeState = false;
}
void Game::ResetTime()
{
    frameTime = 0.0f;
    deltaTime = 0.0f;
    elapsedTime = 0.0f;
    frameCount = 0;
}
float Game::GetFrameTime()
{
    return frameTime;
}
float Game::GetDeltaTime()
{
    return deltaTime;
}
float Game::GetElapsedTime()
{
    return elapsedTime;
}
unsigned int Game::GetFrameCount()
{
    return frameCount;
}

void Game::HideCursor(bool value)
{
    ShowCursor(!value);
}
void Game::LockCursor(bool value)
{
    if (value)
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        ClientToScreen(hwnd, (POINT*)&rect.left);
        ClientToScreen(hwnd, (POINT*)&rect.right);
        ClipCursor(&rect);
        isCursorLocked = true;
    }
    else
    {
        ClipCursor(NULL);
        isCursorLocked = false;
    }
}
bool Game::IsCursorHidden()
{
    CURSORINFO cursorInfo;
    cursorInfo.cbSize = sizeof(CURSORINFO);
    if (GetCursorInfo(&cursorInfo))
        return cursorInfo.flags != CURSOR_SHOWING;
    return false;
}
bool Game::IsCursorLocked()
{
    return isCursorLocked;
}

D3D11_VIEWPORT Game::GetViewport()
{
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.Width = (float)GameGetContextWidth();
    viewport.Height = (float)GameGetContextHeight();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    return viewport;
}

ID3D11RenderTargetView* Game::GetRenderTargetView()
{
    return gameRenderTargetView;
}

ID3D11DepthStencilView* Game::GetDepthStencilView()
{
    return gameDepthStencilView;
}

IDXGISwapChain* Game::GetSwapChain()
{
    return gameSwapChain;
}

bool Game::GameResizeBuffer()
{
    return true;
}

void Game::SetWindowState(unsigned int state)
{
    ShowWindow(hwnd, state);
}