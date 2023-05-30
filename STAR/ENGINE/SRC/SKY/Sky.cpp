#include "Sky.h"

#include <fstream>
#include "../DX/DX.h"
#include "../EDITOR/Editor.h"
#include "../MODEL/AssimpLoader.h"
#include "../EDITOR/WINDOW/Assets.h"
#include "../EDITOR/WINDOW/Viewport.h"

static Sky sky;

Sky& SkyClass()
{
	return sky;
}

///////////////////////////////////////////////////////////////

static DX* dx = &DXClass();
static AssimpLoader* assimpLoader = &AssimpLoaderClass();

struct ConstantBuffer
{
	DirectX::XMMATRIX WVP = DirectX::XMMatrixIdentity();
	float exposure = 1.0f;
};

static ConstantBuffer cb;

bool Sky::Init()
{
	if (FAILED(StarHelpers::CompileShaderFromFile(L"DATA\\Shaders\\Sky\\vs.hlsl", "main", VS_VERSION, &VS)))
		return false;
	if (FAILED(StarHelpers::CompileShaderFromFile(L"DATA\\Shaders\\Sky\\ps.hlsl", "main", PS_VERSION, &PS)))
		return false;
	if (FAILED(dx->dxDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), nullptr, &pVS)))
		return false;
	if (FAILED(dx->dxDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), nullptr, &pPS)))
		return false;

	D3D11_INPUT_ELEMENT_DESC idx[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (UINT)IM_OFFSETOF(Vertex, texCoords), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(dx->dxDevice->CreateInputLayout(idx, ARRAYSIZE(idx), VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout)))
		return false;

	VS->Release();
	PS->Release();

	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(ConstantBuffer);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		if (FAILED(dx->dxDevice->CreateBuffer(&desc, nullptr, &pConstantBuffer)))
			return false;
	}

	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		if (FAILED(dx->dxDevice->CreateRasterizerState(&desc, &pCullNone)))
			return false;
	}

	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		if (FAILED(dx->dxDevice->CreateDepthStencilState(&desc, &pLessEqual)))
			return false;
	}

	{
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
	}

	sphereModel = assimpLoader->LoadRawModel("DATA\\Models\\Sphere.obj");

	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * sphereModel[0].vertices.size());
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &sphereModel[0].vertices[0];
		if (FAILED(dx->dxDevice->CreateBuffer(&desc, &data, &sphereVertexBuffer)))
			return false;
	}

	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * sphereModel[0].indices.size());
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &sphereModel[0].indices[0];
		if (FAILED(dx->dxDevice->CreateBuffer(&desc, &data, &sphereIndexBuffer)))
			return false;
	}

	return true;
}

void Sky::Render(DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
	if (!type == SkyType::SkyNone)
	{
		DirectX::XMMATRIX skyMatrix = DirectX::XMMatrixIdentity() * view * projection;
		cb.WVP = DirectX::XMMatrixTranspose(skyMatrix);
		cb.exposure = exposure;

		dx->dxDeviceContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
		dx->dxDeviceContext->VSSetShader(pVS, 0, 0);
		dx->dxDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer); // for WVP
		dx->dxDeviceContext->PSSetShader(pPS, 0, 0);
		dx->dxDeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer); // for mode
		dx->dxDeviceContext->IASetInputLayout(pLayout);
		dx->dxDeviceContext->RSSetState(pCullNone);
		dx->dxDeviceContext->OMSetDepthStencilState(pLessEqual, 0);
		dx->dxDeviceContext->PSSetSamplers(0, 1, &pSamplerState);

		if (type == SkyType::SkySphereMap)
		{
			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			dx->dxDeviceContext->IASetVertexBuffers(0, 1, &sphereVertexBuffer, &stride, &offset);
			dx->dxDeviceContext->IASetIndexBuffer(sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			dx->dxDeviceContext->PSSetShaderResources(0, 1, &sphere_texture);
			dx->dxDeviceContext->DrawIndexed((UINT)sphereModel[0].indices.size(), 0, 0);
		}
	}
}

void Sky::Shutdown()
{
	if (pVS)             pVS->Release();
	if (pPS)             pPS->Release();
	if (pLayout)         pLayout->Release();
	if (pConstantBuffer) pConstantBuffer->Release();
	if (pCullNone)       pCullNone->Release();
	if (pLessEqual)      pLessEqual->Release();
	if (pSamplerState)   pSamplerState->Release();

	if (sphere_texture)     sphere_texture->Release();
	if (sphereVertexBuffer) sphereVertexBuffer->Release();
	if (sphereIndexBuffer)  sphereIndexBuffer->Release();
}

bool Sky::SetSky(SkyFile sky)
{
	if (sky.GetType() == SkyType::SkySphereMap)
	{
		if (sky.GetSpherePath().empty()) return false;
		if (!std::filesystem::exists(sky.GetSpherePath().c_str())) return false;

		if (outCore)
		{
			outCore = false;
			std::thread thread(&Sky::OutCore, this, sky);
			thread.detach();
		}
	}

	return true;
}

void Sky::OutCore(SkyFile sky)
{
	type = SkyType::SkyNone;

	{
		if (sky.GetType() == SkyType::SkySphereMap)
		{
			if (sphere_texture) sphere_texture->Release();

			StarHelpers::AddLog("[Sky] -> Loading... %s", sky.GetSpherePath().c_str());

			size_t pos = sky.GetSpherePath().find_last_of(".");
			std::string buffer = sky.GetSpherePath().substr(pos);

			/*
			if (buffer == PNG || buffer == JPEG)
			{
				if (FAILED(DirectX::LoadFromWICFile(
					StarHelpers::ConvertString(sky.GetSpherePath()).c_str(),
					DirectX::WIC_FLAGS_NONE,
					nullptr,
					sphere_image)))
					return;
			}

			if (buffer == DDS)
			{
				if (FAILED(DirectX::LoadFromDDSFile(
					StarHelpers::ConvertString(sky.GetSpherePath()).c_str(),
					DirectX::DDS_FLAGS_NONE,
					nullptr,
					sphere_image)))
					return;
			}
			*/

			if (buffer == HDR)
			{
				DirectX::TexMetadata data;
				if (FAILED(DirectX::LoadFromHDRFile(
					StarHelpers::ConvertString(sky.GetSpherePath()).c_str(), &data, sphere_image)))
					return;

				if (FAILED(DirectX::CreateShaderResourceView(
					dx->dxDevice, sphere_image.GetImages(), sphere_image.GetImageCount(), data, &sphere_texture)))
					return;
			}

			sphere_image.Release();
		}

		outCore = true;
	}

	type = sky.GetType();
}