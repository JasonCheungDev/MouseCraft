#pragma once

#include <string>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../Core/Entity.h"
#include "../Core/ComponentFactory.h"
#include "../Core/EntityManager.h"
#include "../Loading/TextureLoader.h"
#include "../Loading/ResourceCache.h"
#include "../Loading/PrefabLoader.h"
#include "../Rendering/Mesh.h"
#include "../Rendering/Material.h"
#include "../Rendering/RenderComponent.h"
#include "../Rendering/Constants.h"
#include "../Rendering/Texture.h"

class ModelLoader
{
public:
	// Loads RenderComponents, Renderables, Mesh, and Materials into a screne graph hierarchy.
	static Entity* Load(const std::string& path);

	// Loads only the first Mesh found in a model. 
	static std::shared_ptr<Mesh> LoadMesh(const std::string& path);

	// DEBUG
	// Use specified material instead of the NEXT file's model.
	// TODO: Override policy { REPLACE, DEFAULT, OVERRIDE } 
	static void SetOverrideMaterial(std::shared_ptr<Material> material);
	// Use specified shader instead for the NEXT file. 
	static void SetOverrideShader(Shader* shader);

private:
	static std::string directory;

	static std::shared_ptr<Material> overrideMaterial;

	static Shader* overrideShader;

	static Entity* ProcessNode(Entity* entity, aiNode* node, const aiScene* scene);

	// Helper function to create a Renderable from an assimp node (Mesh + Material)
	static std::shared_ptr<Renderable> Processmesh(aiMesh* mesh, const aiScene* scene);

	// Helper function to create a Mesh from an assimp node
	static std::shared_ptr<Mesh> ProcessMeshVertices(aiMesh* mesh, const aiScene* scene);

	// Helper function to create a Material from an assimp node
	static std::shared_ptr<Material> ProcessMeshMaterial(aiMesh* mesh, const aiScene* scene);

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	// INFO: Only loads in one texture as no shaders support multiple textures of the same time (at the moment).
	static std::vector<TextureShaderInfo> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

	static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static glm::vec3 aiColor4DToVec3(const aiColor4D &from)
	{
		return glm::vec3(from.r, from.g, from.b);
	}

	static glm::vec3 aiVec3ToVec3(const aiVector3D &from)
	{
		return glm::vec3(from.x, from.y, from.x);
	}

#pragma region Prefab Loading

	static Entity* LoadFromJson(json json);
	static EntityRegistrar reg;

#pragma endregion
};
