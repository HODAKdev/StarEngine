#include "Entity.h"
#include <vector>
#include "../HELPERS/Helpers.h"
#include "COMPONENT/MeshComponent.h"
#include "../XTK/WICTextureLoader11.h"
#include "COMPONENT/GeneralComponent.h"
#include "COMPONENT/TransformComponent.h"
#include "COMPONENT/RigidbodyComponent.h"
#include "COMPONENT/CameraComponent.h"
#include "COMPONENT/TextMeshComponent.h"
#include "../SYSTEM/ProjectSceneSystem.h"
#include "../SYSTEM/ScriptingSystem.h"
#include "../SYSTEM/PhysicsSystem.h"
#include "../MODEL/AssimpLoader.h"

#define CUBE_MODEL "DATA\\Models\\Cube.obj"
#define SPHERE_MODEL "DATA\\Models\\Sphere.obj"
#define CAPSULE_MODEL "DATA\\Models\\Capsule.obj"
#define PLANE_MODEL "DATA\\Models\\Plane.obj"

Entity entity;

Entity& EntityClass()
{
	return entity;
}

static AssimpLoader* assimpLoader = &AssimpLoaderClass();
static ProjectSceneSystem* projectSceneSystem = &ProjectSceneSystemClass();

static std::vector<Mesh> cube;
static std::vector<Mesh> sphere;
static std::vector<Mesh> capsule;
static std::vector<Mesh> plane;

bool Entity::Init()
{
    root = CreateEntity();
	GetComponent<GeneralComponent>(root).SetName("Scene");
    projectSceneSystem->NewScene();
    return true;
}

void Entity::CreateEmptyEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Empty");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateCubeEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Cube");
	AddComponent<MeshComponent>(entity);
	auto& meshComponent = GetComponent<MeshComponent>(entity);

    for (size_t i = 0; i < cube[0].vertices.size(); i++)
    {
        Vertex vertex;
        vertex.position.x = cube[0].vertices[i].position.x;
        vertex.position.y = cube[0].vertices[i].position.y;
        vertex.position.z = cube[0].vertices[i].position.z;
        vertex.normal.x = cube[0].vertices[i].normal.x;
        vertex.normal.y = cube[0].vertices[i].normal.y;
        vertex.normal.z = cube[0].vertices[i].normal.z;
        vertex.texCoords.x = cube[0].vertices[i].texCoords.x;
        vertex.texCoords.y = cube[0].vertices[i].texCoords.y;
        meshComponent.AddVertices(vertex);
    }

    for (size_t i = 0; i < cube[0].indices.size(); i++)
        meshComponent.AddIndices(cube[0].indices[i]);

    meshComponent.SetupMesh();
    meshComponent.SetFileName(CUBE_MODEL);
    meshComponent.SetMeshName("Cube");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateSphereEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Sphere");
    AddComponent<MeshComponent>(entity);
    auto& meshComponent = GetComponent<MeshComponent>(entity);

    for (size_t i = 0; i < sphere[0].vertices.size(); i++)
    {
        Vertex vertex;
        vertex.position.x = sphere[0].vertices[i].position.x;
        vertex.position.y = sphere[0].vertices[i].position.y;
        vertex.position.z = sphere[0].vertices[i].position.z;
        vertex.normal.x = sphere[0].vertices[i].normal.x;
        vertex.normal.y = sphere[0].vertices[i].normal.y;
        vertex.normal.z = sphere[0].vertices[i].normal.z;
        vertex.texCoords.x = sphere[0].vertices[i].texCoords.x;
        vertex.texCoords.y = sphere[0].vertices[i].texCoords.y;
        meshComponent.AddVertices(vertex);
    }

    for (size_t i = 0; i < sphere[0].indices.size(); i++)
        meshComponent.AddIndices(sphere[0].indices[i]);

    meshComponent.SetupMesh();
    meshComponent.SetFileName(SPHERE_MODEL);
    meshComponent.SetMeshName("Sphere");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateCapsuleEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Capsule");
    AddComponent<MeshComponent>(entity);
    auto& meshComponent = GetComponent<MeshComponent>(entity);

    for (size_t i = 0; i < capsule[0].vertices.size(); i++)
    {
        Vertex vertex;
        vertex.position.x = capsule[0].vertices[i].position.x;
        vertex.position.y = capsule[0].vertices[i].position.y;
        vertex.position.z = capsule[0].vertices[i].position.z;
        vertex.normal.x = capsule[0].vertices[i].normal.x;
        vertex.normal.y = capsule[0].vertices[i].normal.y;
        vertex.normal.z = capsule[0].vertices[i].normal.z;
        vertex.texCoords.x = capsule[0].vertices[i].texCoords.x;
        vertex.texCoords.y = capsule[0].vertices[i].texCoords.y;
        meshComponent.AddVertices(vertex);
    }

    for (size_t i = 0; i < capsule[0].indices.size(); i++)
        meshComponent.AddIndices(capsule[0].indices[i]);

    meshComponent.SetupMesh();
    meshComponent.SetFileName(CAPSULE_MODEL);
    meshComponent.SetMeshName("Capsule");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreatePlaneEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Plane");
    AddComponent<MeshComponent>(entity);
    auto& meshComponent = GetComponent<MeshComponent>(entity);

    for (size_t i = 0; i < plane[0].vertices.size(); i++)
    {
        Vertex vertex;
        vertex.position.x = plane[0].vertices[i].position.x;
        vertex.position.y = plane[0].vertices[i].position.y;
        vertex.position.z = plane[0].vertices[i].position.z;
        vertex.normal.x = plane[0].vertices[i].normal.x;
        vertex.normal.y = plane[0].vertices[i].normal.y;
        vertex.normal.z = plane[0].vertices[i].normal.z;
        vertex.texCoords.x = plane[0].vertices[i].texCoords.x;
        vertex.texCoords.y = plane[0].vertices[i].texCoords.y;
        meshComponent.AddVertices(vertex);
    }

    for (size_t i = 0; i < plane[0].indices.size(); i++)
        meshComponent.AddIndices(plane[0].indices[i]);

    meshComponent.SetupMesh();
    meshComponent.SetFileName(PLANE_MODEL);
    meshComponent.SetMeshName("Plane");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateCameraEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("Camera");
    AddComponent<CameraComponent>(entity);
    GetComponent<GeneralComponent>(root).AddChild(entity);
}
void Entity::CreateTextMeshEntity(entt::entity entity)
{
    GetComponent<GeneralComponent>(entity).SetName("TextMesh");
    AddComponent<TextMeshComponent>(entity);
    GetComponent<TextMeshComponent>(entity).SetText("hello");
    GetComponent<GeneralComponent>(root).AddChild(entity);
}

void Entity::LoadCoreModels()
{
    cube    = assimpLoader->LoadRawModel(CUBE_MODEL);
    sphere  = assimpLoader->LoadRawModel(SPHERE_MODEL);
    capsule = assimpLoader->LoadRawModel(CAPSULE_MODEL);
    plane   = assimpLoader->LoadRawModel(PLANE_MODEL);
}

entt::entity Entity::CreateEntity()
{
    entt::entity entity = entt::null;
    entity = registry.create();
    AddComponent<GeneralComponent>(entity);
    AddComponent<TransformComponent>(entity);
    AddComponent<ScriptingComponent>(entity);
    AddComponent<PhysicsComponent>(entity);
    return entity;
}
void Entity::SetSelectedEntity(entt::entity entity)
{
    selected = entity;
}
entt::entity Entity::GetSelectedEntity()
{
    return selected;
}
void Entity::SetRootEntity(entt::entity entity)
{
    root = entity;
}
entt::entity Entity::GetRootEntity()
{
    return root;
}
entt::registry& Entity::GetRegistry()
{
    return registry;
}