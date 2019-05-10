#include "ModelLoader.h"
#include <fstream>
#include <iostream>
#include <queue>

using std::ifstream;
using std::string;
using std::vector;

std::string ModelLoader::directory = "";


// Checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
// INFO: Only loads in one texture as no shaders support multiple textures of the same time (at the moment).

Entity * ModelLoader::Load(const std::string & path)
{
	// read file via assimp 
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |	// reformat vertices
		aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes);						// minimize nodes (and entities)

	// check for errors 
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Failed to read file. Assimp Error: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	// retrieve directory (used to load in other files such as textures)
	ModelLoader::directory = path.substr(0, path.find_last_of('/') + 1);

	// process root node recursively (gather all children)
	auto rootEntity = EntityManager::Instance().Create();

	// Entity* rootEntity = new Entity();
	ProcessNode(rootEntity, scene->mRootNode, scene);

	// cleanup and return 
	ModelLoader::directory = "";
	return rootEntity;
}

std::shared_ptr<Mesh> ModelLoader::LoadMesh(const std::string & path)
{
	// read file via assimp 
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |	// reformat vertices
		aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes);						// minimize nodes (and entities)

	// check for errors 
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Failed to read file. Assimp Error: " << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	// retrieve directory (used to load in other files such as textures)
	ModelLoader::directory = path.substr(0, path.find_last_of('/') + 1);

	std::shared_ptr<Mesh> mesh = nullptr;

	// BFS traversal 
	std::queue<aiNode*> aiNodes;
	aiNodes.push(scene->mRootNode);
	while (aiNodes.empty())
	{
		auto* node = aiNodes.front();
		aiNodes.pop();

		if (node->mNumMeshes > 0)
		{
			mesh = ProcessMeshVertices((aiMesh*)node->mMeshes[0], scene);
			break;
		}
		
		for (int i = 0; i < node->mNumChildren; i++)
			aiNodes.push((aiNode*)node->mChildren[i]);
	}

	// cleanup and return 
	ModelLoader::directory = "";
	return mesh;
}

void ModelLoader::ProcessNode(Entity * entity, aiNode * node, const aiScene * scene)
{
	// check if there is anything to draw for this node 
	if (node->mNumMeshes > 0)
	{
		// add a render component 
		auto c_render = ComponentFactory::Create<RenderComponent>();
		entity->AddComponent(c_render);
		// process each mesh located at the current node & add renderable packages 
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			c_render->addRenderable(Processmesh(mesh, scene));
		}
	}

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		Entity* newEntity = EntityManager::Instance().Create();
		newEntity->transform.setLocalTransformation(aiMatrix4x4ToGlm(node->mTransformation));
		entity->AddChild(newEntity);
		ProcessNode(newEntity, node->mChildren[i], scene);
	}
}

std::shared_ptr<Renderable> ModelLoader::Processmesh(aiMesh * mesh, const aiScene * scene)
{
	// create a renderable & material to house data in
	std::shared_ptr<Renderable> renderable = std::make_shared<Renderable>();
	renderable->mesh = ProcessMeshVertices(mesh, scene);
	renderable->material = ProcessMeshMaterial(mesh, scene);
	// return a mesh object created from the extracted mesh data
	return renderable;
}

std::shared_ptr<Mesh> ModelLoader::ProcessMeshVertices(aiMesh * mesh, const aiScene * scene)
{
	// data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// Note: a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
		// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
		Vertex vertex;
		vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		vertex.Uv = (mesh->mTextureCoords[0]) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f);
		vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
		vertex.Bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
		vertices.push_back(vertex);
	}

	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		// TODO: concerned this will cause all models to be low poly visuals (loading indicies more than once)
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Material> ModelLoader::ProcessMeshMaterial(aiMesh * mesh, const aiScene * scene)
{
	std::vector<TextureShaderInfo> textures;
	auto mat = std::make_shared<Material>();

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	aiColor4D aiColor;

	// process materials
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// diffuse color 
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &aiColor))
		mat->SetVec3(SHADER_DIFFUSE.c_str(), aiColor4DToVec3(aiColor));
	// specular color 
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &aiColor))
		mat->SetVec3(SHADER_SPECULAR.c_str(), aiColor4DToVec3(aiColor));
	// ambient color 
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &aiColor))
		mat->SetVec3(SHADER_AMBIENT.c_str(), aiColor4DToVec3(aiColor));
	// emissive color 
	if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &aiColor))
		mat->SetVec3(SHADER_EMISSIVE.c_str(), aiColor4DToVec3(aiColor));
	// shininess strength 
	float shininess;
	if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS_STRENGTH, &shininess))
		mat->SetFloat(SHADER_SHININESS.c_str(), shininess);

	// 1. diffuse maps
	std::vector<TextureShaderInfo> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, SHADER_TEX_DIFFUSE);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<TextureShaderInfo> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, SHADER_TEX_SPECULAR);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<TextureShaderInfo> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, SHADER_TEX_NORMAL);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<TextureShaderInfo> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, SHADER_TEX_HEIGHT);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	// add texture to material 
	mat->AddTextures(textures);

	return mat;
}

std::vector<TextureShaderInfo> ModelLoader::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
	std::vector<TextureShaderInfo> textures;

	if (mat->GetTextureCount(type) > 0)
	{
		aiString str;
		mat->GetTexture(type, 0, &str);
		std::string filePath = ModelLoader::directory + str.C_Str();
		textures.push_back({ typeName, TextureLoader::Load(filePath) });
	}

	/* To load all textures
	textures.reserve(mat->GetTextureCount(type));
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
	aiString str;
	mat->GetTexture(type, i, &str);
	textures.emplace_back(typeName + std::to_string(i), TextureLoader::Load(str.C_Str()));
	}
	*/

	return textures;
}

Entity * ModelLoader::LoadFromJson(json json)
{
	return ModelLoader::Load(json["path"].get<std::string>());
}

EntityRegistrar ModelLoader::reg("model", &ModelLoader::LoadFromJson);