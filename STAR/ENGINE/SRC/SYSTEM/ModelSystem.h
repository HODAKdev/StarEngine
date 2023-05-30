#pragma once

#include <d3d11.h>

class ModelSystem
{
public:
	bool Init();
	void Shutdown();
public:
	ID3D11VertexShader* pVS = nullptr;
	ID3D11PixelShader* pPS = nullptr;
	ID3D11InputLayout* pLayout = nullptr;
	ID3D11Buffer* pConstantBuffer = nullptr;
	ID3D11SamplerState* pSamplerState = nullptr;
};

ModelSystem& ModelSystemClass();