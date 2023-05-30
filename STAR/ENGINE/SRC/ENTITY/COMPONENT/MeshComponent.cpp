#include "MeshComponent.h"
#include "../../HELPERS/Helpers.h"
#include "../../EDITOR/WINDOW/Viewport.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../EDITOR/WINDOW/Assets.h"
#include "../../SYSTEM/ModelSystem.h"

static DX* dx = &DXClass();
static Entity* ecs = &EntityClass();
static ViewportWindow* viewportWindow = &ViewportClass();
static AssetsWindow* assetsWindow = &AssetsClass();
static ModelSystem* modelSystem = &ModelSystemClass();

struct ConstantBuffer
{
	DirectX::XMMATRIX sProjection;
	DirectX::XMMATRIX sView;
	DirectX::XMMATRIX sModel;
};

static ConstantBuffer cb;

void MeshComponent::Render()
{
	ImGui::Checkbox("##MESH", &activeComponent);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("MESH", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Remove"))
			{
				entt::entity entity = entt::to_entity(ecs->registry, *this);
				ecs->registry.get<MeshComponent>(entity).ClearCache();
				ecs->registry.remove<MeshComponent>(entity);
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("MeshComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Vertices");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Faces");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Material");
			}
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("%u", GetNumVertices());

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("%u", GetNumFaces());

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Button(material_name.c_str());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_ASS"))
					{
						FILEs payload_n = *(FILEs*)payload->Data;
						if (payload_n.file_type == MAT)
						{
							std::string buffer = assetsWindow->GetNowDirPath() + "\\" + payload_n.file_name;
							AddMeshMaterial(buffer);
						}
					}
					ImGui::EndDragDropTarget();
				}
			}
			ImGui::EndTable();
		}
	}
}

void MeshComponent::AddVertices(Vertex add)
{
	vertices.push_back(add);
}
void MeshComponent::AddIndices(UINT add)
{
	indices.push_back(add);
}
UINT MeshComponent::GetNumVertices()
{
	return (UINT)vertices.size();
}
UINT MeshComponent::GetNumFaces()
{
	return (UINT)indices.size() / 3;
}
void MeshComponent::ClearCache()
{
	vertexBuffer.Reset();
	indexBuffer.Reset();
}

bool MeshComponent::SetupMesh()
{
	CreateBoundingBox();

	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertices[0];

		if (FAILED(dx->dxDevice->CreateBuffer(&desc, &data, vertexBuffer.GetAddressOf())))
			return false;
	}

	//////////////////////////////////////////////////////////////

	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indices[0];

		if (FAILED(dx->dxDevice->CreateBuffer(&desc, &data, indexBuffer.GetAddressOf())))
			return false;
	}

	//////////////////////////////////////////////////////////////

	return true;
}

void MeshComponent::DrawMesh(DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
	if (!modelSystem->pVS) return;
	if (!modelSystem->pPS) return;

	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->registry.any_of<TransformComponent>(entity))
	{
		auto& transComp = ecs->registry.get<TransformComponent>(entity);

		cb.sProjection = DirectX::XMMatrixTranspose(projection);
		cb.sView = DirectX::XMMatrixTranspose(view);
		cb.sModel = DirectX::XMMatrixTranspose(transComp.GetTransform());

		dx->dxDeviceContext->UpdateSubresource(modelSystem->pConstantBuffer, 0, nullptr, &cb, 0, 0);
		dx->dxDeviceContext->VSSetShader(modelSystem->pVS, 0, 0);
		dx->dxDeviceContext->VSSetConstantBuffers(0, 1, &modelSystem->pConstantBuffer);
		dx->dxDeviceContext->PSSetShader(modelSystem->pPS, 0, 0);
		dx->dxDeviceContext->PSSetSamplers(0, 1, &modelSystem->pSamplerState);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		dx->dxDeviceContext->IASetInputLayout(modelSystem->pLayout);
		dx->dxDeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		dx->dxDeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dx->dxDeviceContext->PSSetShaderResources(0, 1, &diffuse_texture);

		dx->dxDeviceContext->DrawIndexed((UINT)indices.size(), 0, 0);
	}
}

void MeshComponent::SetActive(bool arg)
{
	activeComponent = arg;
}
bool MeshComponent::IsActive()
{
	return activeComponent;
}

///////////////////////////////////////////////////////////////

const std::vector<Vertex>& MeshComponent::GetVertices()
{
	return vertices;
}
const std::vector<UINT>& MeshComponent::GetIndices()
{
	return indices;
}

void MeshComponent::CreateBoundingBox()
{
	float min_x = 0.0f; float min_y = 0.0f; float min_z = 0.0f;
	float max_x = 0.0f; float max_y = 0.0f; float max_z = 0.0f;

	min_x = max_x = vertices.at(0).position.x;
	min_y = max_y = vertices.at(0).position.y;
	min_z = max_z = vertices.at(0).position.z;

	for (size_t i = 0; i < vertices.size(); i++)
	{
		// x-axis
		if (vertices.at(i).position.x < min_x)
			min_x = vertices.at(i).position.x;
		if (vertices.at(i).position.x > max_x)
			max_x = vertices.at(i).position.x;

		// y-axis
		if (vertices.at(i).position.y < min_y)
			min_y = vertices.at(i).position.y;
		if (vertices.at(i).position.y > max_y)
			max_y = vertices.at(i).position.y;

		// z-axis
		if (vertices.at(i).position.z < min_z)
			min_z = vertices.at(i).position.z;
		if (vertices.at(i).position.z > max_z)
			max_z = vertices.at(i).position.z;
	}

	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->registry.any_of<TransformComponent>(entity))
	{
		DirectX::BoundingBox boundingBox = DirectX::BoundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
		Vector3 min = Vector3(min_x, min_y, min_z);
		Vector3 max = Vector3(max_x, max_y, max_z);
		Vector3 center = (max + min) / 2.0f;
		boundingBox.Center = center;
		boundingBox.Extents = (max - min) / 2.0f;
		boundingBox.Extents.x = abs(boundingBox.Extents.x);
		boundingBox.Extents.y = abs(boundingBox.Extents.y);
		boundingBox.Extents.z = abs(boundingBox.Extents.z);
		ecs->registry.get<TransformComponent>(entity).SetBoundingBox(boundingBox);
	}
}

bool MeshComponent::GetState()
{
	if (vertexBuffer.Get() && indexBuffer.Get()) return true; return false;
}

void MeshComponent::AddMeshMaterial(std::string path)
{
	MaterialBuffer mat_buff;
	assetsWindow->OpenMaterialFile(path, mat_buff);
	material_name = std::filesystem::path(path).stem().string();
	path = std::filesystem::path(path).parent_path().string(); /* remove filename */

	while (true)
	{
		std::size_t found = mat_buff.DiffusePath.find("..\\");
		if (found != std::string::npos)
		{
			mat_buff.DiffusePath.erase(0, 3);
			path = std::filesystem::path(path).parent_path().string();
		}
		else
		{
			std::size_t found = mat_buff.DiffusePath.find("\\"); /* dir? */
			if (found != std::string::npos)
			{
				std::string dir = mat_buff.DiffusePath;
				dir.resize(found);
				path.append("\\");
				path.append(dir);
				mat_buff.DiffusePath.erase(0, (found + 1));
			}
			else
			{
				break;
			}
		}
	}

	path.append("\\");
	path.append(mat_buff.DiffusePath);

	size_t pos = path.find_last_of(".");
	if (pos != -1)
	{
		std::string buffer = path.substr(pos);

		if (buffer == PNG || buffer == JPEG)
		{
			if (diffuse_texture) diffuse_texture->Release();
			DirectX::CreateWICTextureFromFile(
				dx->dxDevice,
				dx->dxDeviceContext,
				StarHelpers::ConvertString(path).c_str(),
				nullptr,
				&diffuse_texture);
		}

		if (buffer == DDS)
		{
			if (diffuse_texture) diffuse_texture->Release();
			DirectX::CreateDDSTextureFromFile(
				dx->dxDevice,
				dx->dxDeviceContext,
				StarHelpers::ConvertString(path).c_str(),
				nullptr,
				&diffuse_texture);
		}
	}
	else
	{
		StarHelpers::AddLog("Error!");
	}
}

void MeshComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::Key << "MeshComponent";
	out << YAML::BeginMap;
	{
		out << YAML::Key << "FileName" << YAML::Value << fileName;
		out << YAML::Key << "MeshName" << YAML::Value << meshName;
	}
	out << YAML::EndMap;
}

void MeshComponent::SetFileName(std::string name)
{
	fileName = name;
}
std::string MeshComponent::GetFileName()
{
	return fileName;
}
void MeshComponent::SetMeshName(std::string name)
{
	meshName = name;
}
std::string MeshComponent::GetMeshName()
{
	return meshName;
}