#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <Windows.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "../XTK/MATH/SimpleMath.h"
#include <physx/PxPhysicsAPI.h>
#include <yaml-cpp/yaml.h>

using namespace DirectX::SimpleMath;

#define VS_VERSION "vs_4_0"
#define PS_VERSION "ps_4_0"
#define ENTRY_POINT "main"
#define LEFT_SPACING 32
#define RIGHT_SPACING 84
#define FIXED_SPACING 64
#define BUTTON_SPACING 26

namespace StarHelpers
{
	RECT GetClientRect();
	UINT GetDisplayWidth();
	UINT GetDisplayHeight();
	UINT GetContextWidth();
	UINT GetContextHeight();
	void AddLog(const char* text, ...);
	HRESULT CompileShaderFromFile(std::wstring srcFile, std::string entryPoint, std::string profile, ID3DBlob** blob);
	HRESULT CompileShaderFromSource(const char* data, std::string entryPoint, std::string profile, ID3DBlob** blob);
    std::wstring ConvertString(std::string buffer);
    /* --- GameInput API --- */
    bool InitInput();
	bool InputAcquire();
    bool InputGetKey(unsigned char key);
	Vector2 InputGetMouse();
    /* --------------------- */
	Vector3 ToRadians(Vector3 rotation);
	Vector3 ToDegrees(Vector3 rotation);
	float RadToDeg(float value);
	float DegToRad(float value);
	physx::PxVec3 vector3_to_physics(Vector3 value);
	physx::PxQuat quat_to_physics(Quaternion value);
	Vector3 physics_to_vector3(physx::PxVec3 value);
	Quaternion physics_to_quat(physx::PxQuat value);
	physx::PxMat44 matrix_to_physics(Matrix value);
	Matrix physics_to_matrix(physx::PxMat44 value);
	physx::PxTransform position_rotation_to_physics(Vector3 position, Quaternion rotation);
	/* --------------------- */
	const char* OpenFileDialog(LPCWSTR dir, LPCWSTR filter, LPCWSTR title);
	/* --------------------- */
	void SerializeVector2(YAML::Emitter& out, Vector2 value);
	void SerializeVector3(YAML::Emitter& out, Vector3 value);
	void SerializeVector4(YAML::Emitter& out, Vector4 value);
	void SerializeQuaternion(YAML::Emitter& out, Quaternion value);
	Vector2 DeserializeVector2(YAML::Node& in);
	Vector3 DeserializeVector3(YAML::Node& in);
	Vector4 DeserializeVector4(YAML::Node& in);
	Quaternion DeserializeQuaternion(YAML::Node& in);
}