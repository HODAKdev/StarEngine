#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include "../../DX/DX.h"
#include "../../IMGUI/imgui.h"
#include "../../IMGUIZMO/ImGuizmo.h"
#include "../../ENTITY/COMPONENT/MeshComponent.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"

struct TransformComponent;

#define MIN_XY 16
#define DEFAULT_CAM_POS Vector3(0.0f, 0.0f, -5.0f)
#define DEFAULT_CAM_ROT Vector3(0.0f, 0.0f, 0.0f)

class ViewportWindow
{
	//// - Viewport System ///////////////////////////////////////
public:
	void Render();
	void ReleaseBuffer();
	bool renderViewport = true;

public:
	D3D11_VIEWPORT GetViewport();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	ID3D11RenderTargetView* s_RenderTargetView = NULL;
	ID3D11DepthStencilView* s_DepthStencilView = NULL;
	ID3D11ShaderResourceView* s_ShaderResourceView = NULL;

private:
	bool IsBufferResized();
	bool cursorOnWindow = false;
	bool CreateBuffer(ImVec2 resolution);
	ImVec2 bufferSize = ImVec2(NULL, NULL);
	ImVec2 windowPos = ImVec2(NULL, NULL);
	ImVec2 windowSize = ImVec2(NULL, NULL);
	ImVec2 windowSizeAvail = ImVec2(NULL, NULL);
	ImVec2 windowCursorPos = ImVec2(NULL, NULL);

public:
	ImVec2 GetBufferSize();

	//// - Camera System /////////////////////////////////////////
private:
	Matrix projection = DirectX::XMMatrixIdentity();
	Matrix view = DirectX::XMMatrixIdentity();

	void SetPerspectiveProjectionMatrix(ImVec2 size, float fov, float NearZ, float FarZ);

public:
	Matrix GetPerspectiveProjectionMatrix();
	Matrix GetPerspectiveViewMatrix();

public:
	Vector3 GetCamPosition();
	Vector3 GetCamTarget();
	Vector3 GetCamUp();

public:
	void SetCamPosition(Vector3 position);
	void SetCamRotation(Vector3 rotation);

	POINT cursorPosition = { 0 };

public:
	void UpdateMovement();

private:
	float camNearZ = 0.1f;
	float camFarZ = 1000.0f;
	float camFov = 45.0f;
	float camSpeed = 8.0f;
	float camBoostSpeed = 16.0f;
	float camSensitivity = 0.001f;

private:
	Matrix     matrix   = Matrix::Identity;
	Vector3    position = DEFAULT_CAM_POS;
	Vector3    rotation = DEFAULT_CAM_ROT;
	Vector3    lPosition = position;
	Vector3    lrotation = rotation;

	//// - Guizmo System //////////////////////////////////////
private:
	void RenderWidgets();
	/* --- */
	void RenderCameraFrustumWidget(TransformComponent& transformComponent);
	void RenderBoundingBoxWidget(TransformComponent& transformComponent);
	void RenderBoxColliderWidget(TransformComponent& transformComponent);
	void RenderManipulateWidget(TransformComponent& transformComponent);
	void RenderGridWidget();
	/* --- */
	Matrix CreateNoScaleMatrix(TransformComponent& transformComponent);

private:
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode = ImGuizmo::LOCAL;

public:
	void SetOperation(ImGuizmo::OPERATION arg);
	void SetMode(ImGuizmo::MODE arg);
	ImGuizmo::OPERATION GetOperation();
	ImGuizmo::MODE GetMode();

public:
	bool useSnap = false;
	float snap = 1.0f;

	//// - Ray System //////////////////////////////////////
private:
	void RayVector(float mouseX, float mouseY, DirectX::XMVECTOR& pickRayInWorldSpacePos, DirectX::XMVECTOR& pickRayInWorldSpaceDir);
	bool PointInTriangle(DirectX::XMVECTOR& triV1, DirectX::XMVECTOR& triV2, DirectX::XMVECTOR& triV3, DirectX::XMVECTOR& point);
	float PickMesh(DirectX::XMVECTOR pickRayInWorldSpacePos, DirectX::XMVECTOR pickRayInWorldSpaceDir, const std::vector<Vertex>& vertPosArray, const std::vector<UINT>& indexPosArray, DirectX::XMMATRIX worldSpace);
	void RunRay();
};

ViewportWindow& ViewportClass();