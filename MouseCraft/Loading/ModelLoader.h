#pragma once
#include "../Core/Entity.h"
#include "../Graphics/Model.h"
#include "../Graphics/BoneData.h"
#include "../Graphics/BonedGeometry.h"
#include "../Graphics/AnimatedRenderable.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <string>
#include <vector>

class ModelLoader
{
public:
	static Model* loadModel(std::string filename);
	static Entity* loadAnimatedModel(std::string filename);
private:
	static void processMeshes(std::vector<aiMesh*>& meshes, const aiScene* scene, Geometry* g);
	
	// accumulates all the meshes for a node 
	static std::vector<aiMesh*> processNode(aiNode* node, const aiScene* scene);

	// builds the vertex data for the mesh
	static void processAnimatedMeshes(std::vector<aiMesh*>& meshes, BonedGeometry* g, std::vector<BoneData>& boneDatas);
	
	// builds the bone hierarcy
	static void buildBoneHierarchy(aiNode* node, const aiScene* scene, std::vector<BoneData>& boneDatas, Entity* root, AnimatedRenderable* animatedRenderable);

	static void buildAnimations(const aiScene* scene, AnimatedRenderable* animatedRenderable);
};

