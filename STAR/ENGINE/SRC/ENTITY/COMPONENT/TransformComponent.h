#pragma once

#include "../../HELPERS/Helpers.h"
#include "../../XTK/MATH/SimpleMath.h"
#include "../../EDITOR/WINDOW/Viewport.h"
#include <yaml-cpp/yaml.h>

struct Transform
{
	Vector3    position = Vector3(0.0f);
	Quaternion rotation = Quaternion::Identity;
	Vector3    scale = Vector3(1.0f);
	Matrix     transform = Matrix::Identity;
	DirectX::BoundingBox boundingBox = DirectX::BoundingBox(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
};

struct TransformComponent
{
private:
	Transform localTransform;
	Transform globalTransform;

private:
	Matrix parent = Matrix::Identity;

private:
	bool isDirty = true;

public:
	void Render();

public:
	void SetBoundingBox(DirectX::BoundingBox boundingBox);
	DirectX::BoundingBox GetBoundingBox();

public:
	void SetPosition(Vector3 position);
	void SetRotationYawPitchRoll(Vector3 rotation);
	void SetRotationQuaternion(Quaternion rotation);
	void SetScale(Vector3 scale);
	void SetTransform(Matrix transform);

public:
	void AddPosition(Vector3 position);
	void AddRotationYawPitchRoll(Vector3 rotation);
	void AddRotationQuaternion(Quaternion rotation);
	void AddScale(Vector3 scale);
	void AddTransform(Matrix transform);

public:
	Vector3    GetPosition();
	Vector3    GetRotationYawPitchRoll();
	Quaternion GetRotationQuaternion();
	Vector3    GetScale();
	Matrix     GetTransform();

public:
	Vector3    GetLocalPosition();
	Vector3    GetLocalRotationYawPitchRoll();
	Quaternion GetLocalRotationQuaternion();
	Vector3    GetLocalScale();
	Matrix     GetLocalTransform();

public:
	void SetParentTransform(Matrix matrix);
	Matrix GetParentTransform();

private:
	void UpdateTransform();
	void UpdateTransformFromPositionRotationScale(); /* TRS to Matrix */
	void UpdatePositionRotationScaleFromTransform(Matrix transform); /* Matrix to TRS */

public:
	void SerializeComponent(YAML::Emitter& out);
	void DeserializeComponent(YAML::Node& in);

public:
	void LookAt(TransformComponent transform, Vector3 up);
};