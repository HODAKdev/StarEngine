#pragma once

#include <physx/PxPhysicsAPI.h>
#include "../../XTK/MATH/SimpleMath.h"
using namespace DirectX::SimpleMath;

struct RigidBodyComponent
{
public:
	void Render();

public:
	void CreateActor();
	void UpdateActor();
	void ReleaseActor();

public:
	physx::PxRigidBody* GetRigidBody();

public:
	void SetMass(float value);
	float GetMass();
	void SetLinearVelocity(Vector3 value);
	Vector3 GetLinearVelocity();
	void SetAngularVelocity(Vector3 value);
	Vector3 GetAngularVelocity();
	void SetLinearDamping(float value);
	float GetLinearDamping();
	void SetAngularDamping(float value);
	float GetAngularDamping();
	void UseGravity(bool value);
	bool HasUseGravity();
	void SetKinematic(bool value);
	bool IsKinematic();
	void AddForce(Vector3 value);
	void AddTorque(Vector3 value);
	void ClearForce();
	void ClearTorque();
	void SetLinearLockX(bool value);
	bool GetLinearLockX();
	void SetLinearLockY(bool value);
	bool GetLinearLockY();
	void SetLinearLockZ(bool value);
	bool GetLinearLockZ();
	void SetAngularLockX(bool value);
	bool GetAngularLockX();
	void SetAngularLockY(bool value);
	bool GetAngularLockY();
	void SetAngularLockZ(bool value);
	bool GetAngularLockZ();
	void SetPosition(Vector3 xyz);
	void SetRotation(Quaternion quat);

public:
	void SetLock(physx::PxRigidDynamicLockFlag::Enum flag, bool value);
	bool GetLock(physx::PxRigidDynamicLockFlag::Enum flag);

public:
	void SetTransform(physx::PxTransform value);
	physx::PxTransform GetTransform();

private:
	physx::PxRigidBody* pxRigidBody = NULL;
};