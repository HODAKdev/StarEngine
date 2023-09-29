#include "ScriptingSystem.h"
#include "../HELPERS/Helpers.h"
#include "../EDITOR/WINDOW/Console.h"
#include <fstream>
#include <filesystem>
#include "../EDITOR/WINDOW/Assets.h"
#include "../ENTITY/COMPONENT/GeneralComponent.h"
#include "../GAME/Game.h"
#include "../ENTITY/COMPONENT/TransformComponent.h"
#include "../ENTITY/COMPONENT/CameraComponent.h"
#include "../ENTITY/COMPONENT/RigidbodyComponent.h"
#include "../ENTITY/COMPONENT/TextMeshComponent.h"

#define COMPONENT_ERROR "Failed to get %s because it was not found!"

static ScriptingSystem scriptingSystem;

ScriptingSystem& ScriptingSystemClass()
{
	return scriptingSystem;
}

/* ----------------------------------- */

static ConsoleWindow* consoleWindow = &ConsoleClass();
static Game* game = &GameClass();
static Entity* ecs = &EntityClass();

bool ScriptingSystem::Init()
{
	lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::package);
	lua.open_libraries(sol::lib::coroutine);
	lua.open_libraries(sol::lib::string);
	lua.open_libraries(sol::lib::os);
	lua.open_libraries(sol::lib::math);
	lua.open_libraries(sol::lib::table);
	lua.open_libraries(sol::lib::debug);
	lua.open_libraries(sol::lib::bit32);
	lua.open_libraries(sol::lib::io);

	//luaopen_socket_core(lua.lua_state());

	/* system */
	lua_add_vector2();
	lua_add_vector3();
	lua_add_vector4();
	lua_add_quaternion();
	lua_add_matrix();
	lua_add_console();
	lua_add_time();
	lua_add_input();
	lua_add_bounding_box();

	/* entity */
	lua_add_entity();
	lua_add_general_component();
	lua_add_transform_component();
	lua_add_camera_component();
	lua_add_rigidbody_component();
	lua_add_mesh_component();
	lua_add_text_mesh_component();

	return true;
}

/* system */
void ScriptingSystem::lua_add_vector2()
{
	typedef Vector2 VECTORX;

	sol::usertype<VECTORX> vector = lua.new_usertype<VECTORX>(
		"Vector2",
		sol::constructors<VECTORX(), VECTORX(float, float)>(),
		sol::meta_function::addition,       [](VECTORX x, VECTORX y)          { return x + y;                     },
		sol::meta_function::subtraction,    [](VECTORX x, VECTORX y)          { return x - y;                     },
		sol::meta_function::multiplication, [](VECTORX x, VECTORX y)          { return x * y;                     },
		sol::meta_function::division,       [](VECTORX x, VECTORX y)          { return x / y;                     },
		sol::meta_function::addition,       [](VECTORX x, float y)            { return VECTORX(x.x + y, x.y + y); },
		sol::meta_function::subtraction,    [](VECTORX x, float y)            { return VECTORX(x.x - y, x.y - y); },
		sol::meta_function::multiplication, [](VECTORX x, float y)            { return VECTORX(x.x * y, x.y * y); },
		sol::meta_function::division,       [](VECTORX x, float y)            { return VECTORX(x.x / y, x.y / y); });
	vector["Length"]     =                  [](VECTORX x)                     { return x.Length();                };
	vector["Dot"]        =                  [](VECTORX x, VECTORX y)          { return x.Dot(y);                  };
	vector["Cross"]      =                  [](VECTORX x, VECTORX y)          { return x.Cross(y);                };
	vector["Normalize"]  =                  [](VECTORX x)                     { x.Normalize();    return x;       };
	vector["Clamp"]      =                  [](VECTORX x, VECTORX y)          { x.Clamp(x, y);    return x;       };
	vector["Distance"]   =                  [](VECTORX x, VECTORX y)          { x.Distance(x, y); return x;       };
	vector["Min"]        =                  [](VECTORX x, VECTORX y)          { x.Min(x, y);      return x;       };
	vector["Max"]        =                  [](VECTORX x, VECTORX y)          { x.Max(x, y);      return x;       };
	vector["Lerp"]       =                  [](VECTORX x, VECTORX y, float z) { x.Lerp(x, y, z);  return x;       };
	vector["Zero"]       = sol::var(VECTORX::Zero);
	vector["One"]        = sol::var(VECTORX::One);
	vector["x"]          = &VECTORX::x;
	vector["y"]          = &VECTORX::y;
}
void ScriptingSystem::lua_add_vector3()
{
	typedef Vector3 VECTORX;

	sol::usertype<VECTORX> vector = lua.new_usertype<VECTORX>(
		"Vector3",
		sol::constructors<VECTORX(), VECTORX(float, float, float)>(),
		sol::meta_function::addition,       [](VECTORX x, VECTORX y)          { return x + y;                              },
		sol::meta_function::subtraction,    [](VECTORX x, VECTORX y)          { return x - y;                              },
		sol::meta_function::multiplication, [](VECTORX x, VECTORX y)          { return x * y;                              },
		sol::meta_function::division,       [](VECTORX x, VECTORX y)          { return x / y;                              },
		sol::meta_function::addition,       [](VECTORX x, float y)            { return VECTORX(x.x + y, x.y + y, x.z + y); },
		sol::meta_function::subtraction,    [](VECTORX x, float y)            { return VECTORX(x.x - y, x.y - y, x.z - y); },
		sol::meta_function::multiplication, [](VECTORX x, float y)            { return VECTORX(x.x * y, x.y * y, x.z * y); },
		sol::meta_function::division,       [](VECTORX x, float y)            { return VECTORX(x.x / y, x.y / y, x.z / y); });
	vector["Length"]    =                   [](VECTORX x)                     { return x.Length();                         };
	vector["Dot"]       =                   [](VECTORX x, VECTORX y)          { return x.Dot(y);                           };
	vector["Cross"]     =                   [](VECTORX x, VECTORX y)          { return x.Cross(y);                         };
	vector["Normalize"] =                   [](VECTORX x)                     { x.Normalize();    return x;                };
	vector["Clamp"]     =                   [](VECTORX x, VECTORX y)          { x.Clamp(x, y);    return x;                };
	vector["Distance"]  =                   [](VECTORX x, VECTORX y)          { x.Distance(x, y); return x;                };
	vector["Min"]       =                   [](VECTORX x, VECTORX y)          { x.Min(x, y);      return x;                };
	vector["Max"]       =                   [](VECTORX x, VECTORX y)          { x.Max(x, y);      return x;                };
	vector["Lerp"]      =                   [](VECTORX x, VECTORX y, float z) { x.Lerp(x, y, z);  return x;                };
	vector["Zero"]      = sol::var(VECTORX::Zero);
	vector["One"]       = sol::var(VECTORX::One);
	vector["Up"]        = sol::var(VECTORX::Up);
	vector["Down"]      = sol::var(VECTORX::Down);
	vector["Right"]     = sol::var(VECTORX::Right);
	vector["Left"]      = sol::var(VECTORX::Left);
	vector["Forward"]   = sol::var(VECTORX::Forward);
	vector["Backward"]  = sol::var(VECTORX::Backward);
	vector["x"]         = &VECTORX::x;
	vector["y"]         = &VECTORX::y;
	vector["z"]         = &VECTORX::z;
}
void ScriptingSystem::lua_add_vector4()
{
	typedef Vector4 VECTORX;

	sol::usertype<VECTORX> vector = lua.new_usertype<VECTORX>(
		"Vector4",
		sol::constructors<VECTORX(), VECTORX(float, float, float, float)>(),
		sol::meta_function::addition,       [](VECTORX x, VECTORX y) { return x + y;                                       },
		sol::meta_function::subtraction,    [](VECTORX x, VECTORX y) { return x - y;                                       },
		sol::meta_function::multiplication, [](VECTORX x, VECTORX y) { return x * y;                                       },
		sol::meta_function::division,       [](VECTORX x, VECTORX y) { return x / y;                                       },
		sol::meta_function::addition,       [](VECTORX x, float y)   { return VECTORX(x.x + y, x.y + y, x.z + y, x.w + y); },
		sol::meta_function::subtraction,    [](VECTORX x, float y)   { return VECTORX(x.x - y, x.y - y, x.z - y, x.w - y); },
		sol::meta_function::multiplication, [](VECTORX x, float y)   { return VECTORX(x.x * y, x.y * y, x.z * y, x.w * y); },
		sol::meta_function::division,       [](VECTORX x, float y)   { return VECTORX(x.x / y, x.y / y, x.z / y, x.w / y); });
	vector["Length"]    = [](VECTORX x)                              { return x.Length();                                  };
	vector["Dot"]       = [](VECTORX x, VECTORX y)                   { return x.Dot(y);                                    };
	vector["Cross"]     = [](VECTORX x, VECTORX y)                   { return x.Cross(x, y);                               };
	vector["Normalize"] = [](VECTORX x)                              { x.Normalize();    return x;                         };
	vector["Clamp"]     = [](VECTORX x, VECTORX y)                   { x.Clamp(x, y);    return x;                         };
	vector["Distance"]  = [](VECTORX x, VECTORX y)                   { x.Distance(x, y); return x;                         };
	vector["Min"]       = [](VECTORX x, VECTORX y)                   { x.Min(x, y);      return x;                         };
	vector["Max"]       = [](VECTORX x, VECTORX y)                   { x.Max(x, y);      return x;                         };
	vector["Lerp"]      = [](VECTORX x, VECTORX y, float z)          { x.Lerp(x, y, z);  return x;                         };
	vector["Zero"]      = sol::var(VECTORX::Zero);
	vector["One"]       = sol::var(VECTORX::One);
	vector["x"]         = &VECTORX::x;
	vector["y"]         = &VECTORX::y;
	vector["z"]         = &VECTORX::z;
	vector["w"]         = &VECTORX::w;
}
void ScriptingSystem::lua_add_quaternion()
{
	sol::usertype<Quaternion> quaternion = lua.new_usertype<Quaternion>(
		"Quaternion",
		sol::constructors<Quaternion(), Quaternion(float, float, float, float)>(),
		sol::meta_function::addition,       [](Quaternion x, Quaternion y) { return x + y; },
		sol::meta_function::subtraction,    [](Quaternion x, Quaternion y) { return x - y; },
		sol::meta_function::multiplication, [](Quaternion x, Quaternion y) { return x * y; },
		sol::meta_function::division,       [](Quaternion x, Quaternion y) { return x / y; });
	quaternion["Identity"] = sol::var(Quaternion::Identity);
	quaternion["x"]        = &Quaternion::x;
	quaternion["y"]        = &Quaternion::y;
	quaternion["z"]        = &Quaternion::z;
	quaternion["w"]        = &Quaternion::w;
}
void ScriptingSystem::lua_add_matrix()
{
	sol::usertype<Matrix> matrix = lua.new_usertype<Matrix>(
		"Matrix",
		sol::constructors<Matrix(), Matrix(
			float, float, float, float,
			float, float, float, float,
			float, float, float, float,
			float, float, float, float)>(),
		sol::meta_function::addition,       [](Matrix x, Matrix y)              { return x + y;                         },
		sol::meta_function::subtraction,    [](Matrix x, Matrix y)              { return x - y;                         },
		sol::meta_function::multiplication, [](Matrix x, Matrix y)              { return x * y;                         },
		sol::meta_function::division,       [](Matrix x, Matrix y)              { return x / y;                         });
	matrix["Up"]       =                    [](Matrix x)                        { return x.Up();                        };
	matrix["Down"]     =                    [](Matrix x)                        { return x.Down();                      };
	matrix["Right"]    =                    [](Matrix x)                        { return x.Right();                     };
	matrix["Left"]     =                    [](Matrix x)                        { return x.Left();                      };
	matrix["Forward"]  =                    [](Matrix x)                        { return x.Forward();                   };
	matrix["Backward"] =                    [](Matrix x)                        { return x.Backward();                  };
	matrix["Lerp"]     =                    [](Matrix x, Matrix y, float z)     { return x.Lerp(x, y, z);               };
	//matrix["createLookAt"] =              [](Vector3 x, Vector3 y, Vector3 z) { return Matrix::CreateLookAt(x, y, z); };
	matrix["Identity"] = sol::var(Matrix::Identity);
	matrix["_11"]      = &Matrix::_11;
	matrix["_12"]      = &Matrix::_12;
	matrix["_13"]      = &Matrix::_13;
	matrix["_14"]      = &Matrix::_14;
	matrix["_21"]      = &Matrix::_21;
	matrix["_22"]      = &Matrix::_22;
	matrix["_23"]      = &Matrix::_23;
	matrix["_24"]      = &Matrix::_24;
	matrix["_31"]      = &Matrix::_31;
	matrix["_32"]      = &Matrix::_32;
	matrix["_33"]      = &Matrix::_33;
	matrix["_34"]      = &Matrix::_34;
	matrix["_41"]      = &Matrix::_41;
	matrix["_42"]      = &Matrix::_42;
	matrix["_43"]      = &Matrix::_43;
	matrix["_44"]      = &Matrix::_44;
}
void ScriptingSystem::lua_add_console()
{
	lua["Console"] = sol::new_table();
	lua["Console"]["Info"]    = [](const char* message) { consoleWindow->AddInfoMessage(message);    };
	lua["Console"]["Warning"] = [](const char* message) { consoleWindow->AddWarningMessage(message); };
	lua["Console"]["Error"]   = [](const char* message) { consoleWindow->AddErrorMessage(message);   };
}
void ScriptingSystem::lua_add_time()
{
	lua["Time"] = sol::new_table();
	lua["Time"]["FrameTime"]   = []() { return game->GetFrameTime();   };
	lua["Time"]["DeltaTime"]   = []() { return game->GetDeltaTime();   };
	lua["Time"]["ElapsedTime"] = []() { return game->GetElapsedTime(); };
	lua["Time"]["FrameCount"]  = []() { return game->GetFrameCount();  };
}
void ScriptingSystem::lua_add_input()
{
	lua["Input"] = sol::new_table();

	/* Mouse */
	//lua["Input"]["ShowCursor"] = []() { StarHelpers::ShowCursor(true); };
	//lua["Input"]["HideCursor"] = []() { StarHelpers::ShowCursor(false); };
	lua["Input"]["HideCursor"]     = [](bool value) { game->HideCursor(value); };
	lua["Input"]["LockCursor"]     = [](bool value) { game->LockCursor(value); };
	lua["Input"]["IsCursorHidden"] = []() { return game->IsCursorHidden(); };
	lua["Input"]["IsCursorLocked"] = []() { return game->IsCursorLocked(); };
	lua["Input"]["GetCursorAxis"]  = []() { return game->GetCursorAxis(); };
	lua["Input"]["GetCursorAxisX"] = []() { return game->GetCursorAxis().x; };
	lua["Input"]["GetCursorAxisY"] = []() { return game->GetCursorAxis().y; };

	/* Keyboard */
	lua["Input"]["GetKeyDown"] = [](unsigned char key) { return game->InputGetKey(key); };
	lua["Input"]["KeyCode"] = sol::new_table();

	lua["Input"]["KeyCode"]["Q"] = DIK_Q;
	lua["Input"]["KeyCode"]["W"] = DIK_W;
	lua["Input"]["KeyCode"]["E"] = DIK_E;
	lua["Input"]["KeyCode"]["R"] = DIK_R;
	lua["Input"]["KeyCode"]["T"] = DIK_T;
	lua["Input"]["KeyCode"]["Y"] = DIK_Y;
	lua["Input"]["KeyCode"]["U"] = DIK_U;
	lua["Input"]["KeyCode"]["I"] = DIK_I;
	lua["Input"]["KeyCode"]["O"] = DIK_O;
	lua["Input"]["KeyCode"]["P"] = DIK_P;

	lua["Input"]["KeyCode"]["A"] = DIK_A;
	lua["Input"]["KeyCode"]["S"] = DIK_S;
	lua["Input"]["KeyCode"]["D"] = DIK_D;
	lua["Input"]["KeyCode"]["F"] = DIK_F;
	lua["Input"]["KeyCode"]["G"] = DIK_G;
	lua["Input"]["KeyCode"]["H"] = DIK_H;
	lua["Input"]["KeyCode"]["J"] = DIK_J;
	lua["Input"]["KeyCode"]["K"] = DIK_K;
	lua["Input"]["KeyCode"]["L"] = DIK_L;

	lua["Input"]["KeyCode"]["Z"] = DIK_Z;
	lua["Input"]["KeyCode"]["X"] = DIK_X;
	lua["Input"]["KeyCode"]["C"] = DIK_C;
	lua["Input"]["KeyCode"]["V"] = DIK_V;
	lua["Input"]["KeyCode"]["B"] = DIK_B;
	lua["Input"]["KeyCode"]["N"] = DIK_N;
	lua["Input"]["KeyCode"]["M"] = DIK_M;

	lua["Input"]["KeyCode"]["1"] = DIK_1;
	lua["Input"]["KeyCode"]["2"] = DIK_2;
	lua["Input"]["KeyCode"]["3"] = DIK_3;
	lua["Input"]["KeyCode"]["4"] = DIK_4;
	lua["Input"]["KeyCode"]["5"] = DIK_5;
	lua["Input"]["KeyCode"]["6"] = DIK_6;
	lua["Input"]["KeyCode"]["7"] = DIK_7;
	lua["Input"]["KeyCode"]["8"] = DIK_8;
	lua["Input"]["KeyCode"]["9"] = DIK_9;
	lua["Input"]["KeyCode"]["0"] = DIK_0;

	lua["Input"]["KeyCode"]["F1"] = DIK_F1;
	lua["Input"]["KeyCode"]["F2"] = DIK_F2;
	lua["Input"]["KeyCode"]["F3"] = DIK_F3;
	lua["Input"]["KeyCode"]["F4"] = DIK_F4;
	lua["Input"]["KeyCode"]["F5"] = DIK_F5;
	lua["Input"]["KeyCode"]["F6"] = DIK_F6;
	lua["Input"]["KeyCode"]["F7"] = DIK_F7;
	lua["Input"]["KeyCode"]["F8"] = DIK_F8;
	lua["Input"]["KeyCode"]["F9"] = DIK_F9;
	lua["Input"]["KeyCode"]["F10"] = DIK_F10;
	lua["Input"]["KeyCode"]["F11"] = DIK_F11;
	lua["Input"]["KeyCode"]["F12"] = DIK_F12;

	lua["Input"]["KeyCode"]["Up"] = DIK_UP;
	lua["Input"]["KeyCode"]["Down"] = DIK_DOWN;
	lua["Input"]["KeyCode"]["Left"] = DIK_LEFT;
	lua["Input"]["KeyCode"]["Right"] = DIK_RIGHT;

	lua["Input"]["KeyCode"]["LeftControl"] = DIK_LCONTROL;
	lua["Input"]["KeyCode"]["RightControl"] = DIK_RCONTROL;

	lua["Input"]["KeyCode"]["LeftShift"] = DIK_LSHIFT;
	lua["Input"]["KeyCode"]["RightShift"] = DIK_RSHIFT;

	lua["Input"]["KeyCode"]["LeftAlt"] = DIK_LALT;
	lua["Input"]["KeyCode"]["RightAlt"] = DIK_RALT;
}
void ScriptingSystem::lua_add_bounding_box()
{
	using namespace DirectX;

	sol::usertype<BoundingBox> boundingBox = lua.new_usertype<BoundingBox>(
		"BoundingBox",
		sol::constructors<BoundingBox(), BoundingBox(Vector3, Vector3)>());
}

/* entity */
void ScriptingSystem::lua_add_entity()
{
	sol::usertype<EntityX> entity = lua.new_usertype<EntityX>(
		"Entity");
	entity["CreateEntity"] = &EntityX::CreateEntity;
	entity["AddComponent"] = &EntityX::AddComponent;
	entity["GetComponent"] = &EntityX::GetComponent;
	entity["HasComponent"] = &EntityX::HasComponent;
	entity["RemoveComponent"] = &EntityX::RemoveComponent;
}
void ScriptingSystem::lua_add_general_component()
{
	sol::usertype<GeneralComponent> component = lua.new_usertype<GeneralComponent>(
		"GeneralComponent");
	component["SetName"] = &GeneralComponent::SetName;
	component["GetName"] = &GeneralComponent::GetName;
	component["SetTag"] = &GeneralComponent::SetTag;
	component["GetTag"] = &GeneralComponent::GetTag;
	component["SetActive"] = &GeneralComponent::SetActive;
	component["GetActive"] = &GeneralComponent::IsActive;
	component["SetStatic"] = &GeneralComponent::SetStatic;
	component["GetStatic"] = &GeneralComponent::IsStatic;
	component["MoveUp"] = &GeneralComponent::MoveUp;
	component["MoveDown"] = &GeneralComponent::MoveDown;
	component["GetParentEntity"] = &GeneralComponent::GetParent;
}
void ScriptingSystem::lua_add_transform_component()
{
	sol::usertype<TransformComponent> component = lua.new_usertype<TransformComponent>(
		"TransformComponent");
	component["SetBoundingBox"] = &TransformComponent::SetBoundingBox;
	component["GetBoundingBox"] = &TransformComponent::GetBoundingBox;
	component["SetPosition"] = &TransformComponent::SetPosition;
	component["SetRotationYawPitchRoll"] = &TransformComponent::SetRotationYawPitchRoll;
	component["SetRotationQuaternion"] = &TransformComponent::SetRotationQuaternion;
	component["SetScale"] = &TransformComponent::SetScale;
	component["SetTransform"] = &TransformComponent::SetTransform;
	component["AddPosition"] = &TransformComponent::AddPosition;
	component["AddRotationYawPitchRoll"] = &TransformComponent::AddRotationYawPitchRoll;
	component["AddRotationQuaternion"] = &TransformComponent::AddRotationQuaternion;
	component["AddScale"] = &TransformComponent::AddScale;
	component["AddTransform"] = &TransformComponent::AddTransform;
	component["GetPosition"] = &TransformComponent::GetPosition;
	component["GetRotationYawPitchRoll"] = &TransformComponent::GetRotationYawPitchRoll;
	component["GetRotationQuaternion"] = &TransformComponent::GetRotationQuaternion;
	component["GetScale"] = &TransformComponent::GetScale;
	component["GetTransform"] = &TransformComponent::GetTransform;
	component["GetLocalPosition"] = &TransformComponent::GetLocalPosition;
	component["GetLocalRotationYawPitchRoll"] = &TransformComponent::GetLocalRotationYawPitchRoll;
	component["GetLocalRotationQuaternion"] = &TransformComponent::GetLocalRotationQuaternion;
	component["GetLocalScale"] = &TransformComponent::GetLocalScale;
	component["GetLocalTransform"] = &TransformComponent::GetLocalTransform;
	component["LookAt"] = &TransformComponent::LookAt;
}
void ScriptingSystem::lua_add_camera_component()
{
	sol::usertype<CameraComponent> component = lua.new_usertype<CameraComponent>(
		"CameraComponent");
	component["SetFov"] = &CameraComponent::SetFov;
	component["SetNear"] = &CameraComponent::SetNear;
	component["SetFar"] = &CameraComponent::SetFar;
	component["GetFov"] = &CameraComponent::GetFov;
	component["GetNear"] = &CameraComponent::GetNear;
	component["GetFar"] = &CameraComponent::GetFar;
	component["SetAspect"] = &CameraComponent::SetAspect;
	component["GetAspect"] = &CameraComponent::GetAspect;
	component["SetScale"] = &CameraComponent::SetScale;
	component["GetScale"] = &CameraComponent::GetScale;
}
void ScriptingSystem::lua_add_rigidbody_component()
{
	sol::usertype<RigidBodyComponent> component = lua.new_usertype<RigidBodyComponent>(
		"RigidbodyComponent");

	component["SetMass"] = &RigidBodyComponent::SetMass;
	component["GetMass"] = &RigidBodyComponent::GetMass;
	component["SetLinearVelocity"] = &RigidBodyComponent::SetLinearVelocity;
	component["GetLinearVelocity"] = &RigidBodyComponent::GetLinearVelocity;
	component["SetAngularVelocity"] = &RigidBodyComponent::SetAngularVelocity;
	component["GetAngularVelocity"] = &RigidBodyComponent::GetAngularVelocity;
	component["SetLinearDamping"] = &RigidBodyComponent::SetLinearDamping;
	component["GetLinearDamping"] = &RigidBodyComponent::GetLinearDamping;
	component["SetAngularDamping"] = &RigidBodyComponent::SetAngularDamping;
	component["GetAngularDamping"] = &RigidBodyComponent::GetAngularDamping;
	component["SetGravity"] = &RigidBodyComponent::UseGravity;
	component["GetGravity"] = &RigidBodyComponent::HasUseGravity;
	component["SetKinematic"] = &RigidBodyComponent::SetKinematic;
	component["IsKinematic"] = &RigidBodyComponent::IsKinematic;
	component["AddForce"] = &RigidBodyComponent::AddForce;
	component["AddTorque"] = &RigidBodyComponent::AddTorque;
	component["ClearForce"] = &RigidBodyComponent::ClearForce;
	component["ClearTorque"] = &RigidBodyComponent::ClearTorque;
}
void ScriptingSystem::lua_add_mesh_component()
{
	sol::usertype<MeshComponent> component = lua.new_usertype<MeshComponent>(
		"MeshComponent");

	component["GetNumVertices"] = &MeshComponent::GetNumVertices;
	component["GetNumFaces"] = &MeshComponent::GetNumFaces;

	component["AddVertices"] = &MeshComponent::AddVertices;
	component["AddIndices"] = &MeshComponent::AddIndices;

	component["SetupMesh"] = &MeshComponent::SetupMesh;

	/* --- */

	sol::usertype<Vertex> vertex = lua.new_usertype<Vertex>(
		"Vertex");

	vertex["position"] = &Vertex::position;
	vertex["normal"] = &Vertex::normal;
	vertex["texCoords"] = &Vertex::texCoords;
}
void ScriptingSystem::lua_add_text_mesh_component()
{
	sol::usertype<TextMeshComponent> component = lua.new_usertype<TextMeshComponent>(
		"TextMeshComponent");

	component["SetText"]        = &TextMeshComponent::SetText;
	component["GetText"]        = &TextMeshComponent::GetText;
	component["SetQuality"]     = &TextMeshComponent::SetQuality;
	component["GetQuality"]     = &TextMeshComponent::GetQuality;
	component["SetFont"]        = &TextMeshComponent::SetFont;
	component["GetFont"]        = &TextMeshComponent::GetFont;
	component["SetAlign"]       = &TextMeshComponent::SetAlign;
	component["GetAlign"]       = &TextMeshComponent::GetAlign;
	component["SetMeshDepth"]   = &TextMeshComponent::SetMeshDepth;
	component["GetMeshDepth"]   = &TextMeshComponent::GetMeshDepth;
	component["SetCharSpacing"] = &TextMeshComponent::SetCharSpacing;
	component["GetCharSpacing"] = &TextMeshComponent::GetCharSpacing;
	component["SetLineSpacing"] = &TextMeshComponent::SetLineSpacing;
	component["GetLineSpacing"] = &TextMeshComponent::GetLineSpacing;
	component["SetWordSpacing"] = &TextMeshComponent::SetWordSpacing;
	component["GetWordSpacing"] = &TextMeshComponent::GetWordSpacing;
}

void ScriptingComponent::AddScript(const char* path)
{
	std::filesystem::path file(path);
	if (file.extension().string() != LUA)
		return;

	for (size_t i = 0; i < scripts.size(); i++)
		if (scripts[i].fileName == file.stem().string())
			return;

	ScriptBuffer scriptBuffer;

	sol::load_result result = scriptingSystem.GetState().load_file(path);
	if (!result.valid())
	{
		sol::error error = result;
		consoleWindow->AddErrorMessage("%s", error.what());
		scriptBuffer.error = true;
	}
	else scriptBuffer.error = false;

	{
		scriptBuffer.filePath = path;
		scriptBuffer.fileName = file.stem().string();
		scriptBuffer.fileNameToUpper = scriptBuffer.fileName;
		std::transform(
			scriptBuffer.fileNameToUpper.begin(),
			scriptBuffer.fileNameToUpper.end(),
			scriptBuffer.fileNameToUpper.begin(),
			[](unsigned char c) { return std::toupper(c); });
	}
	scripts.push_back(scriptBuffer);
}

void ScriptingComponent::Render()
{
	for (size_t i = 0; i < scripts.size(); i++)
	{
		ImGui::PushID((int)i);
		{
			ImGui::Checkbox("##SCRIPT", &scripts[i].activeComponent);
			ImGui::SameLine();

			std::string buffer = scripts[i].fileNameToUpper + " (SCRIPT)";
			if (ImGui::CollapsingHeader(buffer.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				/* code */
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Copy")) {}
				if (ImGui::MenuItem("Paste")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Remove"))
				{
					scripts.erase(scripts.begin() + i);
				}
				ImGui::EndPopup();
			}
		}
		ImGui::PopID();
	}
}

void ScriptingComponent::lua_call_start()
{
	lua_add_entity_from_component();

	for (size_t i = 0; i < scripts.size(); i++)
	{
		if (!scripts[i].activeComponent) continue;
		if (scripts[i].error) continue;

		std::string buffer = scripts[i].fileName;
		sol::function function = scriptingSystem.GetState()[buffer.c_str()]["Start"];
		if (function)
		{
			sol::protected_function_result result = function();
			if (!result.valid())
			{
				sol::error error = result;
				consoleWindow->AddErrorMessage("%s", error.what());
				game->StopGame();
			}
		}
	}
}
void ScriptingComponent::lua_call_update()
{
	lua_add_entity_from_component();

	for (size_t i = 0; i < scripts.size(); i++)
	{
		if (!scripts[i].activeComponent) continue;
		if (scripts[i].error) continue;

		std::string buffer = scripts[i].fileName;
		sol::function function = scriptingSystem.GetState()[buffer.c_str()]["Update"];
		if (function)
		{
			sol::protected_function_result result = function();
			if (!result.valid())
			{
				sol::error error = result;
				consoleWindow->AddErrorMessage("%s", error.what());
				game->StopGame();
			}
		}
	}
}

sol::state& ScriptingSystem::GetState()
{
	return lua;
}

void ScriptingComponent::RecompileScripts()
{
	for (size_t i = 0; i < scripts.size(); i++)
	{
		sol::load_result result = scriptingSystem.GetState().load_file(scripts[i].filePath.c_str());
		if (!result.valid())
		{
			sol::error error = result;
			consoleWindow->AddErrorMessage("%s", error.what());
			scripts[i].error = true;
		}
		else scripts[i].error = false;
	}
}

void ScriptingComponent::lua_add_entity_from_component()
{
	entt::entity entity = entt::to_entity(ecs->registry, *this);
	EntityX entityX; entityX.entity = entity;
	scriptingSystem.GetState().set("entity", entityX);
}

/*** --- EntityX --- ***/

void EntityX::CreateEntity()
{
	entity = ecs->CreateEntity();
	ecs->GetComponent<GeneralComponent>(ecs->root).AddChild(entity);
}

void EntityX::AddComponent(const char* component_name)
{
	if (strcmp(component_name, "MeshComponent") == 0)
	{
		if (!ecs->HasComponent<MeshComponent>(ecs->selected))
			ecs->AddComponent<MeshComponent>(ecs->selected);
	}
	else if (strcmp(component_name, "TextMeshComponent") == 0)
	{
		if (!ecs->HasComponent<TextMeshComponent>(ecs->selected))
			ecs->AddComponent<TextMeshComponent>(ecs->selected);
	}
	else if (strcmp(component_name, "CameraComponent") == 0)
	{
		if (!ecs->HasComponent<CameraComponent>(ecs->selected))
			ecs->AddComponent<CameraComponent>(ecs->selected);
	}
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
	{
		if (!ecs->HasComponent<RigidBodyComponent>(ecs->selected))
		{
			ecs->AddComponent<RigidBodyComponent>(ecs->selected);
			ecs->GetComponent<RigidBodyComponent>(ecs->selected).CreateActor();
		}
	}
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
}
sol::object EntityX::GetComponent(const char* component_name)
{
	sol::object component;

	if (strcmp(component_name, "GeneralComponent") == 0)
	{
		if (ecs->HasComponent<GeneralComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<GeneralComponent>(entity);
			component = sol::make_object(scriptingSystem.GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "TransformComponent") == 0)
	{
		if (ecs->HasComponent<TransformComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<TransformComponent>(entity);
			component = sol::make_object(scriptingSystem.GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "MeshComponent") == 0)
	{
		if (ecs->HasComponent<MeshComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<MeshComponent>(entity);
			component = sol::make_object(scriptingSystem.GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "TextMeshComponent") == 0)
	{
		if (ecs->HasComponent<TextMeshComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<TextMeshComponent>(entity);
			component = sol::make_object(scriptingSystem.GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "CameraComponent") == 0)
	{
		if (ecs->HasComponent<CameraComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<CameraComponent>(entity);
			component = sol::make_object(scriptingSystem.GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
	{
		if (ecs->HasComponent<RigidBodyComponent>(entity))
		{
			auto& entt_comp = ecs->GetComponent<RigidBodyComponent>(entity);
			component = sol::make_object(scriptingSystem.GetState(), &entt_comp);
		}
		else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
	}
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);

	return component;
}
bool EntityX::HasComponent(const char* component_name)
{
	if (strcmp(component_name, "MeshComponent") == 0)
		return ecs->HasComponent<MeshComponent>(entity);
	else if (strcmp(component_name, "TextMeshComponent") == 0)
		return ecs->HasComponent<TextMeshComponent>(entity);
	else if (strcmp(component_name, "CameraComponent") == 0)
		return ecs->HasComponent<CameraComponent>(entity);
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
		return ecs->HasComponent<RigidBodyComponent>(entity);
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);

	return false;
}
void EntityX::RemoveComponent(const char* component_name)
{
	if (strcmp(component_name, "MeshComponent") == 0)
		ecs->RemoveComponent<MeshComponent>(entity);
	else if (strcmp(component_name, "TextMeshComponent") == 0)
		ecs->RemoveComponent<TextMeshComponent>(entity);
	else if (strcmp(component_name, "CameraComponent") == 0)
		ecs->RemoveComponent<CameraComponent>(entity);
	else if (strcmp(component_name, "RigidbodyComponent") == 0)
		ecs->RemoveComponent<RigidBodyComponent>(entity);
	else consoleWindow->AddWarningMessage(COMPONENT_ERROR, component_name);
}

void ScriptingSystem::CreateScript(const char* filename, const char* name)
{
	std::string buffer = std::string(name) + " = {}\n"
		"\n"
		"-- Use this for initialization\n"
		"function " + std::string(name) + ":Start()\n"
		"end\n"
		"\n"
		"-- Update is called once per frame\n"
		"function " + std::string(name) + ":Update()\n"
		"end";

	std::ofstream script;
	script.open(filename);
	script << buffer;
	script.close();
}