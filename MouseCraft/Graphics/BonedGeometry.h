#pragma once

#include "BoneData.h"
#include "Geometry.h"
#include <glm/glm.hpp>

class BonedGeometry : public Geometry
{
public:
	BonedGeometry();
	~BonedGeometry();

	std::vector<GLint>& getBoneData() { return _boneIdData; }

	void setBoneData(std::vector<GLint>& boneData) { _boneIdData = boneData; }

	std::vector<GLfloat>& getBoneWeightData() { return _boneWeightData; }

	void setBoneWeightData(std::vector<GLfloat>& boneWeightData) { _boneWeightData = boneWeightData; }

	/// <summary>
	/// Retrieve the VAO ID for this Geometry. -1 if it does not exists.
	/// </summary>
	/// <todo>May want to move this into Geometry</todo>
	GLuint getVAO() { return _vao; }

	/// <summary>
	/// Builds a VAO (storing it on the GPU) with whatever data is currently set.
	/// </summary>
	/// <todo>May want to move this into Geometry</todo>
	void constructVAO();

private:
	std::vector<GLint> _boneIdData;

	std::vector<GLfloat> _boneWeightData;

	std::vector<BonedVertex> _vertexList;

	/// <summary>
	/// A bindable VAO for this geometry. -1 if it does not exist.
	/// </summary>
	/// <todo>May want to move this into Geometry</todo>
	GLuint _vao = -1;
};

