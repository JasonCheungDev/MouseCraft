#include "ModelLoader.h"
#include <fstream>
#include <iostream>
#include "ObjFileParser.h"
#include "../Graphics/BoneData.h"
#include "../Graphics/BonedGeometry.h"
#include "../Graphics/AnimatedRenderable.h"
#include "../AnimationData.h"
#include "../Core/EntityManager.h"
#include "../Core/Entity.h"
#include "../Core/ComponentManager.h"

using std::ifstream;
using std::string;
using std::vector;

glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from)
{
	glm::mat4 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

Model* ModelLoader::loadModel(string filename) {
	/*
	/// Old model loading code

	ObjFileParser parser = ObjFileParser();
	ifstream file;
	file.open(filename);
	if (!file.is_open()) {
		std::cerr << "Problem opening file " << filename << std::endl;
		return nullptr;
	}
	Geometry* g = parser.parse(file);
	file.close();
	return new Model(g);
	*/

	// Model loading adapted from https://learnopengl.com/Model-Loading/Model

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(
		filename,
		aiProcess_Triangulate
		| aiProcess_GenSmoothNormals
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Problem loading file " << filename << ": " << importer.GetErrorString() << std::endl;
		return nullptr;
	}
	string dir = filename.substr(0, filename.find_last_of('/'));
	Geometry* g = new Geometry();
	vector<aiMesh*> meshes = processNode(scene->mRootNode, scene);
	processMeshes(meshes, scene, g);
	return new Model(g);
}

Entity * ModelLoader::loadAnimatedModel(std::string filename)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(
		filename,
		aiProcess_Triangulate
		| aiProcess_GenSmoothNormals
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Problem loading file " << filename << ": " << importer.GetErrorString() << std::endl;
		return nullptr;
	}
	
	auto e = EntityManager::Instance().Create();
	auto c_ar = ComponentManager<AnimatedRenderable>::Instance().Create<AnimatedRenderable>();
	e->AddComponent(c_ar);

	BonedGeometry* g = new BonedGeometry();
	vector<BoneData> boneDatas;

	vector<aiMesh*> meshes = processNode(scene->mRootNode, scene);
	
	processAnimatedMeshes(meshes, g, boneDatas);
	
	buildBoneHierarchy(scene->mRootNode, scene, boneDatas, e, c_ar);

	buildAnimations(scene, c_ar);

	g->constructVAO();
	c_ar->SetBones(boneDatas);
	c_ar->SetGeometry(g);
	
	return e;
}

vector<aiMesh*> ModelLoader::processNode(aiNode* node, const aiScene *scene) {
	vector<aiMesh*> meshes;
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(mesh);
		std::cout << "bones: " << mesh->mNumBones << std::endl;
	}
	for (int i = 0; i < node->mNumChildren; i++) {
		vector<aiMesh*> childMesh = processNode(node->mChildren[i], scene);
		meshes.insert(meshes.end(), childMesh.begin(), childMesh.end());
	}
	return meshes;
}

void ModelLoader::processMeshes(vector<aiMesh*>& meshes, const aiScene* scene, Geometry* g) {
	std::vector<GLfloat> vertPositions;
	std::vector<GLfloat> vertNormals;
	std::vector<GLfloat> vertTexCoord;
	std::vector<GLuint> indices;
	int meshStartIndex = 0;
	for (aiMesh* mesh : meshes) {
		bool hasTex = mesh->mTextureCoords[0];
		for (int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D v = mesh->mVertices[i];
			aiVector3D nv = mesh->mNormals[i];
			vertPositions.push_back(v.x);
			vertPositions.push_back(v.y);
			vertPositions.push_back(v.z);
			vertNormals.push_back(nv.x);
			vertNormals.push_back(nv.y);
			vertNormals.push_back(nv.z);
			
			if (hasTex) {
				aiVector3D tv = mesh->mTextureCoords[0][i];
				vertTexCoord.push_back(tv.x);
				vertTexCoord.push_back(tv.y);
			}
			else {
				vertTexCoord.push_back(0.0f);
				vertTexCoord.push_back(0.0f);
			}
		}
		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j] + meshStartIndex);
			}
		}


		meshStartIndex = indices.size();
	}
	g->setVertexData(vertPositions);
	g->setNormalData(vertNormals);
	g->setTexCoordData(vertTexCoord);
	g->setIndices(indices);
}

void ModelLoader::processAnimatedMeshes(std::vector<aiMesh*>& meshes, BonedGeometry* g, std::vector<BoneData>& boneDatas)
{
	std::vector<BonedVertex> vertices;
	std::vector<GLuint> indices;
	std::set<std::string> foundBones;

	int boneIndex = 0;
	int meshStartIndex = 0;
	for (aiMesh* mesh : meshes) {
		bool hasTex = mesh->mTextureCoords[0];
		for (int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D v = mesh->mVertices[i];
			aiVector3D nv = mesh->mNormals[i];

			BonedVertex bvert;
			bvert.pos = glm::vec3(v.x, v.y, v.z);
			bvert.nrm = glm::vec3(nv.x, nv.y, nv.z);
			
			if (hasTex)
			{
				aiVector3D tv = mesh->mTextureCoords[0][i];
				bvert.uv = glm::vec2(tv.x, tv.y);
			}
			else
			{
				bvert.uv = glm::vec2();
			}

			bvert.bones = glm::ivec4();
			bvert.weights = glm::vec4();

			vertices.push_back(bvert);
		}

		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j] + meshStartIndex);
			}
		}

		for (int i = 0; i < mesh->mNumBones; i++)
		{
			auto bone = mesh->mBones[i];

			if (foundBones.find(bone->mName.C_Str()) != foundBones.end())
				continue;
			else
				foundBones.insert(bone->mName.C_Str());

			// construct bone 
			BoneData bd;
			bd.name = bone->mName.C_Str();
			bd.id = boneIndex;
			boneDatas.push_back(bd);

			// update affected vertices 
			for (int j = 0; j < bone->mNumWeights; ++j)
			{
				auto weight = bone->mWeights[j];
				auto vertex = &vertices[weight.mVertexId];
				// find the first free bone slot 
				for (int k = 0; k < 4; ++k)
				{
					if (vertex->weights[k] == 0.0f)
					{
						// found
						vertex->bones[k] = boneIndex;	// bone id 
						vertex->weights[k] = weight.mWeight;
						break;
					}
				}
			}
			boneIndex++;
		}

		meshStartIndex = indices.size();
	}

	// reformat vertex struct data into individual data
	std::vector<GLfloat> vertPositions;
	std::vector<GLfloat> vertNormals;
	std::vector<GLfloat> vertTexCoord;
	std::vector<GLint> vertBones;
	std::vector<GLfloat> vertBoneWeights;

	for (auto& bv : vertices)
	{
		vertPositions.push_back(bv.pos.x);
		vertPositions.push_back(bv.pos.y);
		vertPositions.push_back(bv.pos.z);
		vertNormals.push_back(bv.nrm.x);
		vertNormals.push_back(bv.nrm.y);
		vertNormals.push_back(bv.nrm.z);
		vertTexCoord.push_back(bv.uv.x);
		vertTexCoord.push_back(bv.uv.y);
		vertBones.push_back(bv.bones[0]);
		vertBones.push_back(bv.bones[1]);
		vertBones.push_back(bv.bones[2]);
		vertBones.push_back(bv.bones[3]);
		vertBoneWeights.push_back(bv.weights[0]);
		vertBoneWeights.push_back(bv.weights[1]);
		vertBoneWeights.push_back(bv.weights[2]);
		vertBoneWeights.push_back(bv.weights[3]);
	}

	g->setVertexData(vertPositions);
	g->setNormalData(vertNormals);
	g->setTexCoordData(vertTexCoord);
	g->setBoneData(vertBones);
	g->setBoneWeightData(vertBoneWeights);
	g->setIndices(indices);
}

void ModelLoader::buildBoneHierarchy(aiNode * node, const aiScene * scene, std::vector<BoneData>& boneDatas, Entity * root, AnimatedRenderable * animatedRenderable)
{
	if (node->mName.length > 0)
	{
		// this node is a bone 
		auto it = std::find_if(boneDatas.begin(), boneDatas.end(), [node](const BoneData& val)
		{
			return val.name.compare(node->mName.C_Str()) == 0;
		});

		if (it != boneDatas.end())
		{
			// build control entity 
			Entity* childBone = EntityManager::Instance().Create();
			root->AddChild(childBone);
			root = childBone;

			// finalize the bone data 
			std::cout << "build bone hierarchy: " << node->mName.C_Str() << std::endl;
			it->transform = childBone;
			it->offsetMatrix = aiMatrix4x4ToGlm(node->mTransformation);
		}
	}

	for (int i = 0; i < node->mNumChildren; ++i)
	{
		buildBoneHierarchy(node->mChildren[i], scene, boneDatas, root, animatedRenderable);
	}
}

void ModelLoader::buildAnimations(const aiScene * scene, AnimatedRenderable * animatedRenderable)
{
	std::vector<Animation> animations;

	// go thru each animation clip 
	for (int i = 0; i < scene->mNumAnimations; ++i)
	{
		auto aiAnim = scene->mAnimations[i];
		
		Animation anim;
		anim.name = aiAnim->mName.C_Str();
		anim.duration = aiAnim->mDuration;
		anim.ticksPerSecond = aiAnim->mTicksPerSecond;

		// go thru each bone frames 
		for (int j = 0; j < aiAnim->mNumChannels; ++j)
		{
			auto channel = aiAnim->mChannels[j];
			BoneAnim bAnim;

			for (int k = 0; k < channel->mNumPositionKeys; ++k)
			{
				auto t = channel->mPositionKeys[k].mTime;
				auto v = channel->mPositionKeys[k].mValue;
				bAnim.positions.push_back(std::make_pair(t, glm::vec3(v.x, v.y, v.z)));
			}
			for (int k = 0; k < channel->mNumRotationKeys; ++k)
			{
				auto t = channel->mRotationKeys[k].mTime;
				auto v = channel->mRotationKeys[k].mValue;
				bAnim.rotations.push_back(std::make_pair(t, glm::quat(v.w, v.x, v.y, v.z)));
			}
			for (int k = 0; k < channel->mNumScalingKeys; ++k)
			{
				auto t = channel->mScalingKeys[k].mTime;
				auto v = channel->mScalingKeys[k].mValue;
				bAnim.scales.push_back(std::make_pair(t, glm::vec3(v.x, v.y, v.z)));
			}

			anim.frames.push_back(bAnim);
		}

		animations.push_back(anim);
	}

	animatedRenderable->SetAnimations(animations);
}
