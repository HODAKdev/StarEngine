#include "TransformComponent.h"
#include "GeneralComponent.h"
#include "RigidbodyComponent.h"

static Entity* ecs = &EntityClass();
static ViewportWindow* viewportWindow = &ViewportClass();

/* ------------------------------------------------------------ */

void TransformComponent::Render()
{
	if (ImGui::CollapsingHeader("TRANSFORM", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Copy")) {}
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			ImGui::MenuItem("Remove", "", false, false);
			ImGui::EndPopup();
		}

		/////////////////////////////////////////////////////////////

		if (ImGui::BeginTable("TransformComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Position");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Rotation");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Scale");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				Vector3 position = localTransform.position;
				if (ImGui::DragFloat3("##PositionTransformComponent", (float*)&position, viewportWindow->useSnap ? viewportWindow->snap : 0.1f))
					SetPosition(position);

				Vector3 rotation = StarHelpers::ToDegrees(localTransform.rotation.ToEuler());
				if (ImGui::DragFloat3("##RotationTransformComponent", (float*)&rotation, viewportWindow->useSnap ? viewportWindow->snap : 0.1f))
					SetRotationYawPitchRoll(StarHelpers::ToRadians(rotation));

				Vector3 scale = localTransform.scale;
				if (ImGui::DragFloat3("##ScaleTransformComponent", (float*)&scale, viewportWindow->useSnap ? viewportWindow->snap : 0.1f, 0.0f, FLT_MAX))
					SetScale(scale);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

/* ------------------------------------------------------------ */

void TransformComponent::SetBoundingBox(DirectX::BoundingBox boundingBox)
{
	localTransform.boundingBox = boundingBox;
}

DirectX::BoundingBox TransformComponent::GetBoundingBox()
{
	localTransform.boundingBox.Transform(globalTransform.boundingBox, GetTransform());
	return globalTransform.boundingBox;
}

/* ------------------------------------------------------------ */

void TransformComponent::SetPosition(Vector3 position)
{
	localTransform.position = position;
	UpdateTransformFromPositionRotationScale();
	isDirty = true;
}

void TransformComponent::SetRotationYawPitchRoll(Vector3 rotation)
{
	if (StarHelpers::RadToDeg(rotation.x) > 90.0f) rotation.x = StarHelpers::DegToRad(90.0f);
	if (StarHelpers::RadToDeg(rotation.x) < -90.0f) rotation.x = StarHelpers::DegToRad(-90.0f);

	localTransform.rotation = Quaternion::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
	UpdateTransformFromPositionRotationScale();
	isDirty = true;
}

void TransformComponent::SetRotationQuaternion(Quaternion rotation)
{
	localTransform.rotation = rotation;
	UpdateTransformFromPositionRotationScale();
	isDirty = true;
}

void TransformComponent::SetScale(Vector3 scale)
{
	localTransform.scale = scale;
	UpdateTransformFromPositionRotationScale();
	isDirty = true;
}

void TransformComponent::SetTransform(Matrix transform)
{
	localTransform.transform = transform;
	UpdatePositionRotationScaleFromTransform(transform);
	isDirty = true;
}

/* ------------------------------------------------------------ */

Vector3 TransformComponent::GetPosition()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.position;
}

Vector3 TransformComponent::GetRotationYawPitchRoll()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.rotation.ToEuler();
}

Quaternion TransformComponent::GetRotationQuaternion()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.rotation;
}

Vector3 TransformComponent::GetScale()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.scale;
}

Matrix TransformComponent::GetTransform()
{
	if (isDirty)
		UpdateTransform();
	isDirty = false;
	return globalTransform.transform;
}

/* ------------------------------------------------------------ */

void TransformComponent::UpdateTransformFromPositionRotationScale()
{
	localTransform.transform = Matrix::Identity;
	localTransform.transform = Matrix::CreateTranslation(localTransform.position) * localTransform.transform;
	localTransform.transform = Matrix::CreateFromQuaternion(localTransform.rotation) * localTransform.transform;
	localTransform.transform = Matrix::CreateScale(localTransform.scale) * localTransform.transform;
}

void TransformComponent::UpdatePositionRotationScaleFromTransform(Matrix transform)
{
	transform.Decompose(localTransform.scale, localTransform.rotation, localTransform.position);
}

void TransformComponent::UpdateTransform()
{
	globalTransform.transform = localTransform.transform * parent;
	globalTransform.transform.Decompose(globalTransform.scale, globalTransform.rotation, globalTransform.position);
}

void TransformComponent::SetParentTransform(Matrix matrix)
{
	parent = matrix;
	isDirty = true;
}

Matrix TransformComponent::GetParentTransform()
{
	return parent;
}

/* ------------------------------------------------------------ */

Vector3 TransformComponent::GetLocalPosition()
{
	return localTransform.position;
}
Vector3 TransformComponent::GetLocalRotationYawPitchRoll()
{
	return localTransform.rotation.ToEuler();
}
Quaternion TransformComponent::GetLocalRotationQuaternion()
{
	return localTransform.rotation;
}
Vector3 TransformComponent::GetLocalScale()
{
	return localTransform.scale;
}
Matrix TransformComponent::GetLocalTransform()
{
	return localTransform.transform;
}

void TransformComponent::AddPosition(Vector3 position)
{
	SetPosition(Vector3(
		GetLocalPosition().x + position.x,
		GetLocalPosition().y + position.y,
		GetLocalPosition().z + position.z));
}

void TransformComponent::AddRotationYawPitchRoll(Vector3 rotation)
{
	SetRotationYawPitchRoll(Vector3(
		GetLocalRotationYawPitchRoll().x + rotation.x,
		GetLocalRotationYawPitchRoll().y + rotation.y,
		GetLocalRotationYawPitchRoll().z + rotation.z));
}

void TransformComponent::AddRotationQuaternion(Quaternion rotation)
{
	SetRotationQuaternion(Quaternion::Concatenate(rotation, GetLocalRotationQuaternion()));
}

void TransformComponent::AddScale(Vector3 scale)
{
	SetScale(Vector3(
		GetLocalScale().x + scale.x,
		GetLocalScale().y + scale.y, 
		GetLocalScale().z + scale.z));
}

void TransformComponent::AddTransform(Matrix transform)
{
	SetTransform(GetLocalTransform() * transform);
}

void TransformComponent::SerializeComponent(YAML::Emitter& out)
{
	out << YAML::Key << "TransformComponent";
	out << YAML::BeginMap;
	{
		out << YAML::Key << "Position"; StarHelpers::SerializeVector3(out, GetLocalPosition());
		out << YAML::Key << "Rotation"; StarHelpers::SerializeQuaternion(out, GetLocalRotationQuaternion());
		out << YAML::Key << "Scale"; StarHelpers::SerializeVector3(out, GetLocalScale());
	}
	out << YAML::EndMap;
}

void TransformComponent::DeserializeComponent(YAML::Node& in)
{
	auto position = in["Position"];
	SetPosition(StarHelpers::DeserializeVector3(position));
	auto rotation = in["Rotation"];
	SetRotationQuaternion(StarHelpers::DeserializeQuaternion(rotation));
	auto scale = in["Scale"];
	SetScale(StarHelpers::DeserializeVector3(scale));
}

void TransformComponent::LookAt(TransformComponent transform, Vector3 up)
{
	Matrix lookAt = Matrix::CreateLookAt(GetPosition(), transform.GetPosition(), up);
	SetTransform(lookAt);
}