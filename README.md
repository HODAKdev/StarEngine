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
- Physics Collider Visualization
- Parent, Child Transformation
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
## Entity C++ API
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
generalComponent.SetTag(std::string);
generalComponent.SetActive(bool);
generalComponent.SetStatic(bool);
generalComponent.AddChild(entt::entity);
/* not all, add more */

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
Qwantani (Pure Sky) https://polyhaven.com/a/qwantani_puresky \
Font Awesome https://fontawesome.com/ \
![](/images/entt.png)
![](/images/physx.png)
![](/images/lua.png)
## License
StarEngine is licensed under the MIT License, see [LICENSE](/LICENSE) for more information.
