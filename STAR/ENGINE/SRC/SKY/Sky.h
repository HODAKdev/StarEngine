#pragma once

#include "../MODEL/AssimpLoader.h"
#include "../XTK/TEX/DirectXTex.h"

enum SkyType : unsigned char
{
	SkyNone = 0x00,
	SkySolidColor = 0x01,
	SkySphereMap = 0x02,
};

using namespace DirectX::SimpleMath;

struct SkyFile
{
private:
	//// TYPE ////
	unsigned char type = SkyType::SkyNone;
	//// SOLID_COLOR ////
	Vector3 solidColor = Vector3(0.0f);
	//// SPHERE_MAP ////
	std::string spherePath;

public:
	unsigned char GetType()
	{
		return type;
	}

	Vector3 GetSolidColor()
	{
		return solidColor;
	}

	std::string GetSpherePath()
	{
		return spherePath;
	}

public:
	void SetSolidColor(Vector3 rgb)
	{
		type = SkyType::SkySolidColor;
		solidColor = rgb;
	}

	void SetSphereMap(std::string path)
	{
		type = SkyType::SkySphereMap;
		spherePath = path;
	}
};

class Sky
{
public:
	bool Init();
	void Render(DirectX::XMMATRIX view, DirectX::XMMATRIX projection);
	void Shutdown();

public:
	bool SetSky(SkyFile sky);

private:
	ID3DBlob*                VS              = nullptr;   // raw shader data
	ID3DBlob*                PS              = nullptr;   // raw shader data
	ID3D11VertexShader*      pVS             = nullptr;   // shader options
	ID3D11PixelShader*       pPS             = nullptr;   // shader options
	ID3D11InputLayout*       pLayout         = nullptr;   // -//-
	ID3D11Buffer*            pConstantBuffer = nullptr;   // bridge for data (CPU -> GPU)
	ID3D11RasterizerState*   pCullNone       = nullptr;   // disable culling
	ID3D11DepthStencilState* pLessEqual      = nullptr;   // view other vertices
	ID3D11SamplerState*      pSamplerState   = nullptr;   // -//-

private:
	unsigned char type = SkyType::SkyNone;

private:
	DirectX::ScratchImage sphere_image;
	ID3D11ShaderResourceView* sphere_texture = nullptr;
	ID3D11Buffer* sphereVertexBuffer = nullptr;
	ID3D11Buffer* sphereIndexBuffer = nullptr;
	std::vector<Mesh> sphereModel;

public:
	float exposure = 1.0f;

private:
	void OutCore(SkyFile sky);
	bool outCore = true;
};

Sky& SkyClass();