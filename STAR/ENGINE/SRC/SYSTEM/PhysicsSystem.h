#pragma once

#include <entt/entt.hpp>
#include <physx/PxPhysicsAPI.h>
#include "../XTK/MATH/SimpleMath.h"
#define PVD_HOST "127.0.0.1"
#define PVD_PORT 5425

using namespace DirectX::SimpleMath;

/* GRAVITY TYPES */
#define EARTH_GRAVITY physx::PxVec3(0.0f, -9.81f, 0.0f)
#define MOON_GRAVITY physx::PxVec3(0.0f, -1.62f, 0.0f)
#define NONE_GRAVITY physx::PxVec3(0.0f, 0.0f, 0.0f)
/* DEFAULT MATERIAL */
#define STATIC_FRICTION 0.5f
#define DYNAMIC_FRICTION 0.5f
#define RESTITUTION 0.1f

enum PhysicsProcesor : unsigned char
{
	xCPU = 0x10,
	xGPU = 0x20,
};

struct PhysicsSystem
{
public:
	bool Init();
	void Update();
	void Shutdown();

private:
	physx::PxDefaultAllocator	   gAllocator;
	physx::PxDefaultErrorCallback  gErrorCallback;
	physx::PxFoundation*           gFoundation = NULL;
	physx::PxPhysics*              gPhysics = NULL;
	physx::PxDefaultCpuDispatcher* gDispatcher = NULL;
	physx::PxScene*                gScene = NULL;
	physx::PxPvd*                  gPvd = NULL;
	physx::PxPvdTransport*         transport = NULL;
	physx::PxCudaContextManager*   gCudaContextManager = NULL;

private:
	unsigned char physicsProcesor = PhysicsProcesor::xGPU;

public:
	physx::PxPhysics* GetPhysics();
	physx::PxScene* GetScene();
};

PhysicsSystem& PhysicsSystemClass();

struct BoxColliderBuffer
{
public:
	bool activeComponent = true;

public:
	void CreateShape(entt::entity entity);
	void CreateMaterial();

	Vector3 size = Vector3(1.0f, 1.0f, 1.0f);

private:
	physx::PxShape* pxShape = NULL;
	physx::PxMaterial* pxMaterial = NULL;

public:
	physx::PxShape* GetShape() const;
	physx::PxMaterial* GetMaterial() const;
	void SetStaticFriction(float value);
	float GetStaticFriction();
	void SetDynamicFriction(float value);
	float GetDynamicFriction();
	void SetRestitution(float value);
	float GetRestitution();
	void SetCenter(Vector3 value);
	Vector3 GetCenter() const;
	void SetSize(Vector3 value);
	Vector3 GetSize() const;
};

struct PhysicsComponent
{
public:
	void Render();
	void AddBoxCollider();

public:
	const std::vector<BoxColliderBuffer>& GetBoxColliders();

private:
	std::vector<BoxColliderBuffer> box_colliders;
};