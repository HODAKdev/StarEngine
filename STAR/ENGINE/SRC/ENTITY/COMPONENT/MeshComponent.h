#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../DX/DX.h"
#include "../../IMGUI/imgui.h"
#include "../../ENTITY/Entity.h"
#include "../../XTK/MATH/SimpleMath.h"
#include "../../IMGUI/imgui_stdlib.h"
#include <yaml-cpp/yaml.h>
#include <wrl/client.h>

using namespace DirectX::SimpleMath;

struct Vertex
{
	Vector3 position = Vector3(0.0f);
	Vector3 normal = Vector3(0.0f);
	Vector2 texCoords = Vector2(0.0f);
	//Vector3 tangent = Vector3(0.0f);
	//Vector3 bitangent = Vector3(0.0f);
};

struct MaterialBuffer
{
	std::string DiffusePath;
};

struct MeshComponent
{
public:
	void Render();
	void DrawMesh(DirectX::XMMATRIX view, DirectX::XMMATRIX projection);
	bool SetupMesh();

	~MeshComponent()
	{
		vertexBuffer.Reset();
		indexBuffer.Reset();
	}

private:
	void ClearCache();

private:
	bool activeComponent = true;

public:
	void SetActive(bool arg);
	bool IsActive();

private:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

public:
	UINT GetNumVertices();
	UINT GetNumFaces();

private:
	std::string fileName;
	std::string meshName;
public:
	void SetFileName(std::string name);
	std::string GetFileName();
	void SetMeshName(std::string name);
	std::string GetMeshName();

public:
	void AddVertices(Vertex add);
	void AddIndices(UINT add);

public:
	const std::vector<Vertex>& GetVertices();
	const std::vector<UINT>& GetIndices();

private:
	void CreateBoundingBox();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

public:
	bool GetState();

private:
	ID3D11ShaderResourceView* diffuse_texture = nullptr;
public:
	void AddMeshMaterial(std::string path);
	std::string material_name = "None";

public:
	void SerializeComponent(YAML::Emitter& out);
};