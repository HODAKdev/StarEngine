#include "CameraComponent.h"
#include "../../GAME/Game.h"
#include "TransformComponent.h"
#include <entt/entt.hpp>
#include "../Entity.h"

static Game* game = &GameClass();
static Entity* ecs = &EntityClass();

void CameraComponent::Render()
{
	ImGui::Checkbox("##CAMERA", &activeComponent);
	ImGui::SameLine();

	if (ImGui::CollapsingHeader("CAMERA", ImGuiTreeNodeFlags_DefaultOpen))
	{
		/* [CAMERA COMPONENT SECTION] */

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		if (ImGui::BeginTable("CameraComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Type");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				if (cameraType == CameraType::Perspective) ImGui::Text("Fov");
				if (cameraType == CameraType::Orthographic) ImGui::Text("Scale");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Near");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Far");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Aspect");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				if (ImGui::Combo("##TypeCameraComponent", &item_current, items, IM_ARRAYSIZE(items)))
				{
					if (item_current == 0) cameraType = CameraType::Perspective;
					if (item_current == 1) cameraType = CameraType::Orthographic;
				}

				if (cameraType == CameraType::Perspective)
				{
					float _Fov = fovView;
					if (ImGui::SliderFloat("##FovCamComp", &_Fov, MIN_FOV, MAX_FOV))
						SetFov(_Fov);
				}

				if (cameraType == CameraType::Orthographic)
				{
					float _Scale = scale;
					if (ImGui::DragFloat("##ScaleCamComp", &_Scale, 0.1f, 0.0f, FLT_MAX))
						SetScale(_Scale);
				}

				float _Near = nearView;
				if (ImGui::DragFloat("##NearCamComp", &_Near, 0.1f, 0.0f, farView))
					SetNear(_Near);

				float _Far = farView;
				if (ImGui::DragFloat("##FarCamComp", &_Far, 0.1f, nearView, FLT_MAX))
					SetFar(_Far);

				ImGui::Checkbox("##AutomaticAspect", &automaticAspect);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Auto");
				ImGui::SameLine();
				if (automaticAspect) ImGui::BeginDisabled();
				{
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
					{
						Vector2 _Aspect = aspectView;
						if (ImGui::DragFloat2("##AspectCamComp", (float*)&_Aspect, 0.1f, -1.0f, FLT_MAX))
							SetAspect(_Aspect);
					}
					ImGui::PopItemWidth();
				}
				if (automaticAspect) ImGui::EndDisabled();
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

void CameraComponent::SetFov(float _Fov)
{
	if (_Fov < MIN_FOV) return;
	if (_Fov > MAX_FOV) return;
	fovView = _Fov;
}
float CameraComponent::GetFov()
{
	return fovView;
}
void CameraComponent::SetNear(float _Near)
{
	if (_Near < 0.0f) return;
	if (_Near > farView) return;
	nearView = _Near;
}
float CameraComponent::GetNear()
{
	return nearView;
}
void CameraComponent::SetFar(float _Far)
{
	if (_Far < nearView) return;
	farView = _Far;
}
float CameraComponent::GetFar()
{
	return farView;
}
void CameraComponent::SetCameraType(unsigned char _Type)
{
	cameraType = _Type;
}
unsigned char CameraComponent::GetCameraType()
{
	return cameraType;
}
void CameraComponent::SetActive(bool _Active)
{
	activeComponent = _Active;
}
bool CameraComponent::IsActive()
{
	return activeComponent;
}
void CameraComponent::SetScale(float _Scale)
{
	if (_Scale < 0.0f) return;
	scale = _Scale;
}
float CameraComponent::GetScale()
{
	return scale;
}
void CameraComponent::SetAspect(Vector2 _Aspect)
{
	if (_Aspect.x < -1.0f) return;
	if (_Aspect.y < -1.0f) return;
	aspectView = _Aspect;
}
Vector2 CameraComponent::GetAspect()
{
	return aspectView;
}
Matrix CameraComponent::GetViewMatrix()
{
	CreateNewViewMatrix();
	return viewMatrix;
}
Matrix CameraComponent::GetProjectionMatrix()
{
	CreateNewProjectionMatrix();
	return projectionMatrix;
}

void CameraComponent::CreateNewViewMatrix()
{
	using namespace DirectX;
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->HasComponent<TransformComponent>(entity))
	{
		auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
		Vector3 eye = transformComponent.GetTransform().Translation();
		Vector3 target = transformComponent.GetTransform().Backward();
		Vector3 up = transformComponent.GetTransform().Up();
		viewMatrix = XMMatrixLookAtLH(eye, eye + target, up);
	}
}
void CameraComponent::CreateNewProjectionMatrix()
{
	if (cameraType == CameraType::Perspective)
	{
		float aspect = (GetFixedProjectionValue(CameraType::Perspective).x / GetFixedProjectionValue(CameraType::Perspective).y);
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fovView), aspect, nearView, farView);
	}

	if (cameraType == CameraType::Orthographic)
	{
		Vector2 xy = GetFixedProjectionValue(CameraType::Orthographic);
		projectionMatrix = DirectX::XMMatrixOrthographicLH(xy.x, xy.y, nearView, farView);
	}
}
Vector2 CameraComponent::GetFixedProjectionValue(unsigned char _Type)
{
	if (_Type == CameraType::Perspective)
	{
		if (automaticAspect)
			return Vector2((float)game->GameGetContextWidth(),
				(float)game->GameGetContextHeight());
		else return aspectView;
	}

	if (_Type == CameraType::Orthographic)
	{
		if (automaticAspect)
			return Vector2((float)game->GameGetContextWidth() * (scale * smoothOrthographic),
				(float)game->GameGetContextHeight() * (scale * smoothOrthographic));
		else return aspectView;
	}

	return Vector2(0.0f, 0.0f);
}