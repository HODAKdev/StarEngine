#include "RigidBodyComponent.h"
#include "../../IMGUI/imgui.h"
#include <entt/entt.hpp>
#include "../../ENTITY/Entity.h"
#include "../../ENTITY/COMPONENT/TransformComponent.h"
#include "../../EDITOR/WINDOW/Console.h"
#include "../../SYSTEM/PhysicsSystem.h"

static Entity* ecs = &EntityClass();
static ConsoleWindow* consoleWindow = &ConsoleClass();
static PhysicsSystem* physicsSystem = &PhysicsSystemClass();

void RigidBodyComponent::Render()
{
	if (ImGui::CollapsingHeader("RIGIDBODY", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("RigidBodyComponentTable", 2, ImGuiTableFlags_Resizable))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("Mass");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Linear Damping");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Angular Damping");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Use Gravity");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Is Kinematic");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("Freeze");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("	Position");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4);
				ImGui::Text("	Rotation");
			}
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			{
				float mass = GetMass();
				if (ImGui::DragFloat("##MassRigidBodyComponent", &mass, 0.1f, 0.0f, FLT_MAX))
					SetMass(mass);

				float linearDamping = GetLinearDamping();
				if (ImGui::DragFloat("##LinearDampingRigidBodyComponent", &linearDamping, 0.1f, 0.0f, FLT_MAX))
					SetLinearDamping(linearDamping);

				float angularDamping = GetAngularDamping();
				if (ImGui::DragFloat("##AngularDampingRigidBodyComponent", &angularDamping, 0.1f, 0.0f, FLT_MAX))
					SetAngularDamping(angularDamping);

				bool useGravity = HasUseGravity();
				if (ImGui::Checkbox("##UseGravityRigidBodyComponent", &useGravity))
					UseGravity(useGravity);

				bool getKinematic = IsKinematic();
				if (ImGui::Checkbox("##IsKinematicRigidBodyComponent", &getKinematic))
					SetKinematic(getKinematic);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::Text("");
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);

				bool px = GetLinearLockX();
				if (ImGui::Checkbox("X##PositionRigidBodyComponent", &px))
					SetLinearLockX(px);
				ImGui::SameLine();
				bool py = GetLinearLockY();
				if (ImGui::Checkbox("Y##PositionRigidBodyComponent", &py))
					SetLinearLockY(py);
				ImGui::SameLine();
				bool pz = GetLinearLockZ();
				if (ImGui::Checkbox("Z##PositionRigidBodyComponent", &pz))
					SetLinearLockZ(pz);

				bool rx = GetAngularLockX();
				if (ImGui::Checkbox("X##RotationRigidBodyComponent", &rx))
					SetAngularLockX(rx);
				ImGui::SameLine();
				bool ry = GetAngularLockY();
				if (ImGui::Checkbox("Y##RotationRigidBodyComponent", &ry))
					SetAngularLockY(ry);
				ImGui::SameLine();
				bool rz = GetAngularLockZ();
				if (ImGui::Checkbox("Z##RotationRigidBodyComponent", &rz))
					SetAngularLockZ(rz);
			}
			ImGui::PopItemWidth();
			ImGui::EndTable();
		}
	}
}

void RigidBodyComponent::UpdateActor()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);

	if (pxRigidBody)
	{
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			physx::PxTransform pxTransform = pxRigidBody->getGlobalPose();
			auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
			transformComponent.SetPosition(StarHelpers::physics_to_vector3(pxTransform.p));
			transformComponent.SetRotationQuaternion(StarHelpers::physics_to_quat(pxTransform.q));
		}
	}
}

void RigidBodyComponent::CreateActor()
{
	if (pxRigidBody) pxRigidBody->release();
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	if (ecs->HasComponent<TransformComponent>(entity))
	{
		auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
		physx::PxTransform pxTransform = physx::PxTransform(
			StarHelpers::vector3_to_physics(transformComponent.GetPosition()),
			StarHelpers::quat_to_physics(transformComponent.GetRotationQuaternion()));
		pxRigidBody = physicsSystem->GetPhysics()->createRigidDynamic(pxTransform);
		if (!pxRigidBody) consoleWindow->AddWarningMessage("[RigidBody] -> Failed to create the RigidBody!");
		physicsSystem->GetScene()->addActor(*pxRigidBody); /*<*>*/

		if (ecs->HasComponent<PhysicsComponent>(entity))
		{
			auto& physicsComponent = ecs->GetComponent<PhysicsComponent>(entity);
			for (size_t i = 0; i < physicsComponent.GetBoxColliders().size(); i++)
				if (physicsComponent.GetBoxColliders()[i].GetShape())
					pxRigidBody->attachShape(*physicsComponent.GetBoxColliders()[i].GetShape());
				else
					consoleWindow->AddWarningMessage("[RigidBody] -> Failed to attach shape to the RigidBody!");
		}
	}
}

void RigidBodyComponent::SetMass(float value)
{
	if (value < 0.0f) return;
	pxRigidBody->setMass(value);
}
float RigidBodyComponent::GetMass()
{
	return pxRigidBody->getMass();
}
void RigidBodyComponent::SetLinearVelocity(Vector3 value)
{
	pxRigidBody->setLinearVelocity(StarHelpers::vector3_to_physics(value));
}
Vector3 RigidBodyComponent::GetLinearVelocity()
{
	return StarHelpers::physics_to_vector3(pxRigidBody->getLinearVelocity());
}
void RigidBodyComponent::SetLinearDamping(float value)
{
	if (value < 0.0f) return;
	pxRigidBody->setLinearDamping(value);
}
float RigidBodyComponent::GetLinearDamping()
{
	return pxRigidBody->getLinearDamping();
}
void RigidBodyComponent::SetAngularDamping(float value)
{
	if (value < 0.0f) return;
	pxRigidBody->setAngularDamping(value);
}
float RigidBodyComponent::GetAngularDamping()
{
	return pxRigidBody->getAngularDamping();
}
void RigidBodyComponent::SetAngularVelocity(Vector3 value)
{
	pxRigidBody->setAngularVelocity(StarHelpers::vector3_to_physics(value));
}
Vector3 RigidBodyComponent::GetAngularVelocity()
{
	return StarHelpers::physics_to_vector3(pxRigidBody->getAngularVelocity());
}
void RigidBodyComponent::UseGravity(bool value)
{
	pxRigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !value);
}
bool RigidBodyComponent::HasUseGravity()
{
	return !pxRigidBody->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_GRAVITY);
}
void RigidBodyComponent::SetKinematic(bool value)
{
	pxRigidBody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, value);
}
bool RigidBodyComponent::IsKinematic()
{
	return pxRigidBody->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC);
}
void RigidBodyComponent::AddForce(Vector3 value)
{
	pxRigidBody->addForce(StarHelpers::vector3_to_physics(value));
}
void RigidBodyComponent::AddTorque(Vector3 value)
{
	pxRigidBody->addTorque(StarHelpers::vector3_to_physics(value));
}
void RigidBodyComponent::ClearForce()
{
	pxRigidBody->clearForce();
}
void RigidBodyComponent::ClearTorque()
{
	pxRigidBody->clearTorque();
}
void RigidBodyComponent::SetLinearLockX(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X, value);
}
bool RigidBodyComponent::GetLinearLockX()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X);
}
void RigidBodyComponent::SetLinearLockY(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y, value);
}
bool RigidBodyComponent::GetLinearLockY()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y);
}
void RigidBodyComponent::SetLinearLockZ(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z, value);
}
bool RigidBodyComponent::GetLinearLockZ()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z);
}
void RigidBodyComponent::SetAngularLockX(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X, value);
}
bool RigidBodyComponent::GetAngularLockX()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_X);
}
void RigidBodyComponent::SetAngularLockY(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y, value);
}
bool RigidBodyComponent::GetAngularLockY()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Y);
}
void RigidBodyComponent::SetAngularLockZ(bool value)
{
	SetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z, value);
}
bool RigidBodyComponent::GetAngularLockZ()
{
	return GetLock(physx::PxRigidDynamicLockFlag::Enum::eLOCK_ANGULAR_Z);
}
void RigidBodyComponent::SetLock(physx::PxRigidDynamicLockFlag::Enum flag, bool value)
{
	return;
}
bool RigidBodyComponent::GetLock(physx::PxRigidDynamicLockFlag::Enum flag)
{
	return false;
}
void RigidBodyComponent::SetTransform(physx::PxTransform value)
{
	pxRigidBody->setGlobalPose(value);
}
physx::PxTransform RigidBodyComponent::GetTransform()
{
	return pxRigidBody->getGlobalPose();
}
void RigidBodyComponent::ReleaseActor()
{
	if (pxRigidBody) pxRigidBody->release();
}
physx::PxRigidBody* RigidBodyComponent::GetRigidBody()
{
	return pxRigidBody;
}