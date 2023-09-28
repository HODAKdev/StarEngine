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
- Async Loading
- Bounding Box
- Tone Mapping
- DLL Plugin Scripting
- And more...
## Entity API
```cpp
auto entity = ecs->CreateEntity();
ecs->CreateCubeEntity(entity);
ecs->GetComponent<TransformComponent>(entity).SetPosition(Vector3(1.0f, 1.0f, 1.0f));
/* or */
auto& transformComponent = ecs->GetComponent<TransformComponent>(entity);
transformComponent.SetPosition(Vector3(1.0f, 1.0f, 1.0f));

```
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
