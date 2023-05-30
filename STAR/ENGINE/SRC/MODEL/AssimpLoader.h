#pragma once

#include <string>
#include <vector>
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include "../ENTITY/COMPONENT/MeshComponent.h"

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
};

class AssimpLoader
{
public:
	std::vector<Mesh> LoadRawModel(std::string path);
	bool LoadModel(std::string path, entt::entity entity);

private:
	void ProcessRawNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& arg);
	Mesh ProcessRawMesh(aiNode* node, aiMesh* mesh, const aiScene* scene);
	void ProcessNode(aiNode* node, const aiScene* scene, entt::entity entity, std::string path);
	void ProcessMesh(aiNode* node, aiMesh* mesh, const aiScene* scene, entt::entity entity, std::string path);
};

AssimpLoader& AssimpLoaderClass();