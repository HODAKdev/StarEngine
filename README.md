![](/images/banner_logo.png)
## Introduction
An Advanced C++ DirectX 11 Game Engine.
## Features
- Lua Scripting
- Physics
- Entity Component System
- UI Editor
- Panorama Sphere Sky (HDRI)
- Rendering Modes (Pointlist, Linelist, Trianglelist)
- Guizmo & Object Picking
- Text To Mesh Text
- Mesh Model With Textures
- Async Loading
- Bounding Box
- Tone Mapping
- DLL Plugin Scripting
- And more...
## Build
- Visual Studio 2022
- Windows SDK Version: 10.0 (latest installed version)
- Platform Toolset: Visual Studio 2022 (v143)
- C++ Language Standard: C++ 2017
- Configuration: Debug or Release
- Platform: x64
## Community
Discord https://discord.gg/sMYwRaR9FG \
YouTube https://www.youtube.com/@starengine
## Gallery
![](/images/engine_1.png)
![](/images/engine_4.png)
![](/images/engine_2.png)
![](/images/engine_3.png)
![](/images/engine_5.png)
![](/images/engine_6.png)
![](/images/engine_7.png)
![](/images/engine_8.png)
![](/images/1_scan.png)
![](/images/2_scan.png)
![](/images/3_scan.png)
![](/images/4_scan.png)
![](/images/5_scan.png)
![](/images/6_scan.png)
## C++ API
```cpp
/* Entity */
auto entity = ecs->CreateEntity();
ecs->CreateEmptyEntity(entt::entity);
ecs->CreateCubeEntity(entt::entity);
ecs->CreateSphereEntity(entt::entity);
ecs->CreateCapsuleEntity(entt::entity);
ecs->CreatePlaneEntity(entt::entity);
ecs->CreateCameraEntity(entt::entity);
ecs->CreateTextMeshEntity(entt::entity);
ecs->AddComponent<SomeComponent>(entt::entity);
auto& someComponent ecs->GetComponent<SomeComponent>(entt::entity);
if (ecs->HasComponent<SomeComponent>(entt::entity)) {}
ecs->RemoveComponent<SomeComponent>(entt::entity);
ecs->SetSelectedEntity(entt::entity);
auto selectedEntity = ecs->GetSelectedEntity();
ecs->SetRootEntity(entt::entity);
auto rootEntity = ecs->GetRootEntity();
auto registry = ecs->GetRegistry();

/* GeneralComponent */
auto& generalComponent = ecs->GetComponent<GeneralComponent>(entt::entity);
generalComponent.SetName(std::string);
std::string name = generalComponent.GetName();
generalComponent.SetTag(std::string);
std::string tag = generalComponent.GetTag();
generalComponent.SetActive(bool);
bool isActive = generalComponent.IsActive();
generalComponent.SetStatic(bool);
bool isStatic = generalComponent.IsStatic();
generalComponent.AddChild(entt::entity);
generalComponent.MoveUp();
generalComponent.MoveDown();
auto parent = generalComponent.GetParent();
auto children = generalComponent.GetChildren();
bool hasChildren = generalComponent.HasChildren();
generalComponent.Destroy();
generalComponent.DestroyChildren();

/* TextMeshComponent */
auto& textMeshComponent = ecs->GetComponent<TextMeshComponent>(entt::entity);
textMeshComponent.SetText(const char*);
const char* text = textMeshComponent.GetText();
textMeshComponent.SetQuality(unsigned char);
unsigned char quality = textMeshComponent.GetQuality();
textMeshComponent.SetFont(const char*);
const char* font = textMeshComponent.GetFont();
textMeshComponent.SetAlign(unsigned char);
unsigned char align = textMeshComponent.GetAlign();
textMeshComponent.SetMeshDepth(float);
float meshDepth = textMeshComponent.GetMeshDepth();
textMeshComponent.SetCharSpacing(float);
float charSpacing = textMeshComponent.GetCharSpacing();
textMeshComponent.SetLineSpacing(float);
float lineSpacing = textMeshComponent.GetLineSpacing();
textMeshComponent.SetWordSpacing(float);
float wordSpacing = textMeshComponent.GetWordSpacing();

/* TransformComponent */
auto& transformComponent = ecs->GetComponent<TransformComponent>(entt::entity);
transformComponent.SetBoundingBox(DirectX::BoundingBox);
DirectX::BoundingBox boundingBox = transformComponent.GetBoundingBox();
transformComponent.SetPosition(Vector3);
transformComponent.SetRotationYawPitchRoll(Vector3);
transformComponent.SetRotationQuaternion(Quaternion);
transformComponent.SetScale(Vector3);
transformComponent.SetTransform(Matrix);
transformComponent.AddPosition(Vector3);
transformComponent.AddRotationYawPitchRoll(Vector3);
transformComponent.AddRotationQuaternion(Quaternion);
transformComponent.AddScale(Vector3);
transformComponent.AddTransform(Matrix);
Vector3 position = transformComponent.GetPosition();
Vector3 rotationYawPitchRoll = transformComponent.GetRotationYawPitchRoll();
Quaternion rotationQuaternion = transformComponent.GetRotationQuaternion();
Vector3 scale = transformComponent.GetScale();
Matrix matrix = transformComponent.GetTransform();
Vector3 localPosition = transformComponent.GetLocalPosition();
Vector3 localRotationYawPitchRoll = transformComponent.GetLocalRotationYawPitchRoll();
Quaternion localRotationQuaternion = transformComponent.GetLocalRotationQuaternion();
Vector3 localScale = transformComponent.GetLocalScale();
Matrix localTransform = transformComponent.GetLocalTransform();

/* CameraComponent */
auto& cameraComponent = ecs->GetComponent<CameraComponent>(entt::entity);
cameraComponent.SetFov(float);
float fov = cameraComponent.GetFov();
cameraComponent.SetNear(float);
float near = cameraComponent.GetNear();
cameraComponent.SetFar(float);
float far = cameraComponent.GetFar();
cameraComponent.SetCameraType(unsigned char);
unsigned char cameraType = cameraComponent.GetCameraType();
cameraComponent.SetActive(bool);
bool isActive = cameraComponent.IsActive();
cameraComponent.SetScale(float);
float scale = cameraComponent.GetScale();
cameraComponent.SetAspect(Vector2);
Vector2 aspect = cameraComponent.GetAspect();

/* RigidBodyComponent */
auto& rigidBodyComponent = ecs->GetComponent<RigidBodyComponent>(entt::entity);
rigidBodyComponent.SetMass(float);
float mass = rigidBodyComponent.GetMass();
rigidBodyComponent.SetLinearVelocity(Vector3);
Vector3 linearVelocity = rigidBodyComponent.GetLinearVelocity();
rigidBodyComponent.SetAngularVelocity(Vector3);
Vector3 angularVelocity = rigidBodyComponent.GetAngularVelocity();
rigidBodyComponent.SetLinearDamping(float);
float linearDamping = rigidBodyComponent.GetLinearDamping();
rigidBodyComponent.SetAngularDamping(float);
float angularDamping = rigidBodyComponent.GetAngularDamping();
rigidBodyComponent.UseGravity(bool);
bool hasUseGravity = rigidBodyComponent.HasUseGravity();
rigidBodyComponent.SetKinematic(bool);
bool isKinematic = rigidBodyComponent.IsKinematic();
rigidBodyComponent.AddForce(Vector3);
rigidBodyComponent.AddTorque(Vector3);
rigidBodyComponent.ClearForce();
rigidBodyComponent.ClearTorque();
rigidBodyComponent.SetLinearLockX(bool);
bool linearLockX = rigidBodyComponent.GetLinearLockX();
rigidBodyComponent.SetLinearLockY(bool);
bool linearLockY = rigidBodyComponent.GetLinearLockY();
rigidBodyComponent.SetLinearLockZ(bool);
bool linearLockZ = rigidBodyComponent.GetLinearLockZ();
rigidBodyComponent.SetAngularLockX(bool);
bool angularLockX = rigidBodyComponent.GetAngularLockX();
rigidBodyComponent.SetAngularLockY(bool);
bool angularLockY = rigidBodyComponent.GetAngularLockY();
rigidBodyComponent.SetAngularLockZ(bool);
bool angularLockZ = rigidBodyComponent.GetAngularLockZ();

/* MeshComponent */
auto& meshComponent = ecs->GetComponent<MeshComponent>(entt::entity);
meshComponent.SetActive(bool);
bool isActive = meshComponent.IsActive();
meshComponent.SetupMesh();
UINT numVertices = meshComponent.GetNumVertices();
UINT numFaces = meshComponent.GetNumFaces();
meshComponent.SetFileName(std::string);
std::string fileName = meshComponent.GetFileName();
meshComponent.SetMeshName(std::string);
std::string meshName = meshComponent.GetMeshName();
meshComponent.AddVertices(Vertex);
meshComponent.AddIndices(UINT);
auto vertices = meshComponent.GetVertices();
auto indices = meshComponent.GetIndices();
meshComponent.CreateBoundingBox();
bool state = meshComponent.GetState();

/* BoxColliderComponent */
auto physicsComponent = ecs->registry.get<PhysicsComponent>(entt::entity);
auto boxColliders = physicsComponent.GetBoxColliders();
boxColliders[0].SetStaticFriction(float);
float staticFriction = boxColliders[0].GetStaticFriction();
boxColliders[0].SetDynamicFriction(float);
float dynamicFriction = boxColliders[0].GetDynamicFriction();
boxColliders[0].SetRestitution(float);
float restitution = boxColliders[0].GetRestitution();
boxColliders[0].SetCenter(Vector3);
Vector3 center = boxColliders[0].GetCenter();
boxColliders[0].SetSize(Vector3);
Vector3 size = boxColliders[0].GetSize();

/* Sky */
SkyFile skyFile;
skyFile.SetSpherePath(std::string);
skyFile.SetSolidColor(Vector3);
sky->SetSky(skyFile);
unsigned char type = skyFile.GetType();
std::string spherePath = skyFile.GetSpherePath();
Vector3 solidColor = skyFile.GetSolidColor();
```
## Lua API
```lua
-- comming soon
```
## Credits
Dear ImGui https://github.com/ocornut/imgui \
assimp https://github.com/assimp/assimp \
EnTT https://github.com/skypjack/entt \
Lua https://github.com/lua/lua \
PhysX https://github.com/NVIDIAGameWorks/PhysX \
sol2 https://github.com/ThePhD/sol2 \
yaml-cpp https://github.com/jbeder/yaml-cpp \
ttf2mesh https://github.com/fetisov/ttf2mesh \
ImGuizmo https://github.com/CedricGuillemet/ImGuizmo \
ImGuiWidgets https://github.com/HODAKdev/ImGuiWidgets \
DirectXTex https://github.com/microsoft/DirectXTex \
ACES Filmic https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/ \
Font Awesome https://fontawesome.com/ \
![](/images/entt.png)
![](/images/physx.png)
![](/images/lua.png)
## License
StarEngine is licensed under the MIT License, see [LICENSE](/LICENSE) for more information.
