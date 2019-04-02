#include "BonedGeometry.h"

#include "BoneData.h"

BonedGeometry::BonedGeometry()
{
}


BonedGeometry::~BonedGeometry()
{
}

// constructing VAO manually here as there are some non-supported datatypes with the abstracted version (eg. bones use integer time)
void BonedGeometry::constructVAO()
{
	auto vertexCount = getVertexData().size() / 3;
	
	// create data for VBO 
	_vertexList.clear();
	_vertexList.reserve(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
	{
		BonedVertex v {
			glm::vec3(getVertexData()[i * 3], getVertexData()[i * 3 + 1], getVertexData()[i * 3 + 2]),
			glm::vec3(getNormalData()[i * 3], getNormalData()[i * 3 + 1], getNormalData()[i * 3 + 2]),
			glm::vec2(getTexCoordData()[i * 2], getTexCoordData()[i * 2 + 1]),
			glm::ivec4(getBoneData()[i * 4], getBoneData()[i * 4 + 1],
				getBoneData()[i * 4 + 2],getBoneData()[i * 4 + 3]),
			glm::vec4(getBoneWeightData()[i * 4], getBoneWeightData()[i * 4 + 1], 
				getBoneWeightData()[i * 4 + 2], getBoneWeightData()[i * 4 + 3])
		};
		_vertexList.push_back(v);
	}

	// VAO 
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// VBO 
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BonedVertex) * vertexCount, &_vertexList[0], GL_STATIC_DRAW);
	// position attribute 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BonedVertex), (void*)(0));
	glEnableVertexAttribArray(0);
	// normal attribute 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BonedVertex), (void*)(offsetof(BonedVertex, nrm)));
	glEnableVertexAttribArray(1);
	// uv attribute 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BonedVertex), (void*)(offsetof(BonedVertex, uv)));
	glEnableVertexAttribArray(2);
	// bone attribute 
	glVertexAttribIPointer(3, 4, GL_INT,		    sizeof(BonedVertex), (void*)(offsetof(BonedVertex, bones)));	// <-- ... attrib I pointer
	glEnableVertexAttribArray(3);
	// bone weight attribute 
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(BonedVertex), (void*)(offsetof(BonedVertex, weights)));
	glEnableVertexAttribArray(4);

	// EBO 
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * getIndices().size(), &getIndices()[0], GL_STATIC_DRAW);
	
	// finished - unbind
	glBindVertexArray(0);

}
