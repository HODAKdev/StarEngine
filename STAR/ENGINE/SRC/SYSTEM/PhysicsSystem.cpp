#include "PhysicsSystem.h"
#include "../IMGUI/imgui.h"
#include "../ENTITY/Entity.h"
#include "../ENTITY/COMPONENT/RigidbodyComponent.h"
#include "../HELPERS/Helpers.h"
#include "../EDITOR/WINDOW/Console.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"

static PhysicsSystem physicsSystem;

PhysicsSystem& PhysicsSystemClass()
{
	return physicsSystem;
}

/* ----------------------------------- */

static Entity* ecs = &EntityClass();
static ConsoleWindow* consoleWindow = &ConsoleClass();
static DX* dx = &DXClass();

bool PhysicsSystem::Init()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, PVD_PORT, 10);
	if (!gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL))
		StarHelpers::AddLog("[PhysX] -> PhysX Visual Debugger is not connected!\n[PhysX] -> HOST %s PORT %i", PVD_HOST, PVD_PORT);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);

	physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = NONE_GRAVITY;

	if (physicsProcesor == PhysicsProcesor::xCPU)
	{
		gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	}
	else if (physicsProcesor == PhysicsProcesor::xGPU)
	{
		physx::PxCudaContextManagerDesc cudaContextManagerDesc;
		cudaContextManagerDesc.interopMode = physx::PxCudaInteropMode::D3D11_INTEROP;
		cudaContextManagerDesc.graphicsDevice = dx->dxDevice;
		gCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc);
		if (gCudaContextManager) if (!gCudaContextManager->contextIsValid()) StarHelpers::AddLog("[PhysX] -> Cuda Context Manager Error!");

		gDispatcher = physx::PxDefaultCpuDispatcherCreate(4);
		sceneDesc.gpuDispatcher = gCudaContextManager->getGpuDispatcher();

		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_STABILIZATION;
		sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;
		sceneDesc.gpuMaxNumPartitions = 8;
	}
	else
	{
		StarHelpers::AddLog("[PhysX] -> Processor Error!");
	}

	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	/***
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPvd = PxCreatePvd(*gFoundation);
	transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, PVD_PORT, 10);
	if (!gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL))
		StarHelpers::AddLog("[PhysX] -> PhysX Visual Debugger is not connected!\n[PhysX] -> HOST %s PORT %i", PVD_HOST, PVD_PORT);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);
	physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = NONE_GRAVITY;
	gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);
	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	***/

	return true;
}

void PhysicsSystem::Update()
{
	//gScene->simulate(1.0f / 60.0f);
	gScene->simulate(ImGui::GetIO().DeltaTime);
	gScene->fetchResults(true);
}

void PhysicsSystem::Shutdown()
{
	if (gScene)              gScene->release();
	if (gDispatcher)         gDispatcher->release();
	if (gPhysics)            gPhysics->release();
	if (gPvd)                gPvd->release();
	if (transport)           transport->release();
	if (gCudaContextManager) gCudaContextManager->release();
	if (gFoundation)         gFoundation->release();
}

physx::PxPhysics* PhysicsSystem::GetPhysics()
{
	return gPhysics;
}
physx::PxScene* PhysicsSystem::GetScene()
{
	return gScene;
}

void PhysicsComponent::AddBoxCollider()
{
	BoxColliderBuffer boxColliderBuffer;
	{
		boxColliderBuffer.CreateMaterial();
		entt::entity entity = entt::to_entity(ecs->registry, *this);
		boxColliderBuffer.CreateShape(entity);

		if (ecs->registry.any_of<RigidBodyComponent>(entity))
		{
			auto& rigidBodyComponent = ecs->registry.get<RigidBodyComponent>(entity);
			if (rigidBodyComponent.GetRigidBody())
				rigidBodyComponent.GetRigidBody()->attachShape(*boxColliderBuffer.GetShape());
		}
	}
	box_colliders.push_back(boxColliderBuffer);
}

void PhysicsComponent::Render()
{
	for (size_t i = 0; i < box_colliders.size(); i++)
	{
		ImGui::PushID((int)i);
		{
			ImGui::Checkbox("##BOXCOLLIDER", &box_colliders[i].activeComponent);
			ImGui::SameLine();
			if (ImGui::CollapsingHeader("BOXCOLLIDER", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Copy")) {}
					if (ImGui::MenuItem("Paste")) {}
					ImGui::Separator();
					if (ImGui::MenuItem("Remove"))
					{
						if (ecs->registry.any_of<RigidBodyComponent>(ecs->selected))
							ecs->registry.get<RigidBodyComponent>(ecs->selected).GetRigidBody()->detachShape(*box_colliders[i].GetShape());
						box_colliders.erase(box_colliders.begin() + i);
					}
					ImGui::EndPopup();
				}

				if (ImGui::BeginTable("PhysicsComponentTable", 2, ImGuiTableFlags_Resizable))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					{
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
						ImGui::Text("Static Friction");
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
						ImGui::Text("Dynamic Friction");
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
						ImGui::Text("Restitution");
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
						ImGui::Text("Center");
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
						ImGui::Text("Size");
					}
					ImGui::TableNextColumn();
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
					{
						float _StaticFriction = box_colliders[i].GetStaticFriction();
						if (ImGui::DragFloat("##StaticFrictionPhysicsComponent", &_StaticFriction, 0.1f, 0.0f, FLT_MAX))
							box_colliders[i].SetStaticFriction(_StaticFriction);

						float _DynamicFriction = box_colliders[i].GetDynamicFriction();
						if (ImGui::DragFloat("##DynamicFrictionPhysicsComponent", &_DynamicFriction, 0.1f, 0.0f, FLT_MAX))
							box_colliders[i].SetDynamicFriction(_DynamicFriction);


						float _Restitution = box_colliders[i].GetRestitution();
						if (ImGui::DragFloat("##RestitutionPhysicsComponent", &_Restitution, 0.1f, 0.0f, FLT_MAX))
							box_colliders[i].SetRestitution(_Restitution);

						Vector3 _Center = box_colliders[i].GetCenter();
						if (ImGui::DragFloat3("##CenterPhysicsComponent", (float*)&_Center, 0.1f))
							box_colliders[i].SetCenter(_Center);

						Vector3 _Size = box_colliders[i].size;
						if (ImGui::DragFloat3("##Size", (float*)&_Size, 0.1f))
							box_colliders[i].size = _Size;
					}
					ImGui::PopItemWidth();
					ImGui::EndTable();
				}
			}
		}
		ImGui::PopID();
	}
}

physx::PxShape* BoxColliderBuffer::GetShape() const
{
	return pxShape;
}
physx::PxMaterial* BoxColliderBuffer::GetMaterial() const
{
	return pxMaterial;
}
void BoxColliderBuffer::SetStaticFriction(float value)
{
	if (value < 0.0f) return;
	if (pxMaterial) pxMaterial->setStaticFriction(value);
}
float BoxColliderBuffer::GetStaticFriction()
{
	if (!pxMaterial) return 0.0f;
	return pxMaterial->getStaticFriction();
}
void BoxColliderBuffer::SetDynamicFriction(float value)
{
	if (value < 0.0f) return;
	if (pxMaterial) pxMaterial->setDynamicFriction(value);
}
float BoxColliderBuffer::GetDynamicFriction()
{
	if (!pxMaterial) return 0.0f;
	return pxMaterial->getDynamicFriction();
}
void BoxColliderBuffer::SetRestitution(float value)
{
	if (value < 0.0f) return;
	if (pxMaterial) pxMaterial->setRestitution(value);
}
float BoxColliderBuffer::GetRestitution()
{
	if (!pxMaterial) return 0.0f;
	return pxMaterial->getRestitution();
}
void BoxColliderBuffer::SetCenter(Vector3 value)
{
	if (pxShape) pxShape->setLocalPose(physx::PxTransform(value.x, value.y, value.z));
}
Vector3 BoxColliderBuffer::GetCenter() const
{
	physx::PxTransform pxTransform;
	if (pxShape) pxTransform = pxShape->getLocalPose();
	return Vector3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
}
void BoxColliderBuffer::SetSize(Vector3 value)
{
	physx::PxBoxGeometry geometry;
	if (pxShape) pxShape->getBoxGeometry(geometry);
	geometry.halfExtents = physx::PxVec3(value.x, value.y, value.z);
	if (pxShape) pxShape->setGeometry(geometry);
}
Vector3 BoxColliderBuffer::GetSize() const
{
	physx::PxBoxGeometry geometry;
	if (pxShape) pxShape->getBoxGeometry(geometry);
	return Vector3(geometry.halfExtents.x, geometry.halfExtents.y, geometry.halfExtents.z);
}

const std::vector<BoxColliderBuffer>& PhysicsComponent::GetBoxColliders()
{
	return box_colliders;
}

void BoxColliderBuffer::CreateShape(entt::entity entity)
{
	if (pxShape) pxShape->release();
	if (ecs->registry.any_of<TransformComponent>(entity))
	{
		auto& transformComponent = ecs->registry.get<TransformComponent>(entity);
		pxShape = physicsSystem.GetPhysics()->createShape(physx::PxBoxGeometry(
			transformComponent.GetScale().x,
			transformComponent.GetScale().y,
			transformComponent.GetScale().z), *pxMaterial, true);
		if (!pxShape) StarHelpers::AddLog("[PhysX] -> Failed to create the box shape!");
	}
}
void BoxColliderBuffer::CreateMaterial()
{
	if (pxMaterial) pxMaterial->release();
	pxMaterial = physicsSystem.GetPhysics()->createMaterial(
		STATIC_FRICTION,
		DYNAMIC_FRICTION,
		RESTITUTION);
	if (!pxMaterial) StarHelpers::AddLog("[PhysX] -> Failed to create the material!");
}