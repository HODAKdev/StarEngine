#include "ModelSystem.h"
#include "../HELPERS/Helpers.h"
#include "../DX/DX.h"

static ModelSystem modelSystem;

ModelSystem& ModelSystemClass()
{
	return modelSystem;
}

/* ----------------------------------- */

static DX* dx = &DXClass();

struct ConstantBuffer
{
	DirectX::XMMATRIX sProjection;
	DirectX::XMMATRIX sView;
	DirectX::XMMATRIX sModel;
};

bool ModelSystem::Init()
{
	ID3DBlob* VS = nullptr;
	ID3DBlob* PS = nullptr;

	if (FAILED(StarHelpers::CompileShaderFromFile(L"DATA\\Shaders\\Model\\vs.hlsl", ENTRY_POINT, VS_VERSION, &VS)))
		return false;
	if (FAILED(StarHelpers::CompileShaderFromFile(L"DATA\\Shaders\\Model\\ps.hlsl", ENTRY_POINT, PS_VERSION, &PS)))
		return false;
	if (FAILED(dx->dxDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &pVS)))
		return false;
	if (FAILED(dx->dxDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &pPS)))
		return false;

	D3D11_INPUT_ELEMENT_DESC idx[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(dx->dxDevice->CreateInputLayout(idx, ARRAYSIZE(idx), VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout)))
		return false;

	///////////////////////////////////////////////////////////

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	if (FAILED(dx->dxDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer)))
		return false;

	///////////////////////////////////////////////////////////

	VS->Release();
	PS->Release();

	///////////////////////////////////////////////////////////

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(dx->dxDevice->CreateSamplerState(&samplerDesc, &pSamplerState)))
		return false;

	///////////////////////////////////////////////////////////

	return true;
}

void ModelSystem::Shutdown()
{
	if (pVS)             pVS->Release();
	if (pPS)             pPS->Release();
	if (pLayout)         pLayout->Release();
	if (pConstantBuffer) pConstantBuffer->Release();
	if (pSamplerState)   pSamplerState->Release();
}