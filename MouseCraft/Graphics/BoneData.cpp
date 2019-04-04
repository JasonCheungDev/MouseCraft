#include "BoneData.h"
#include <deque>


BoneData::BoneData()
{
}


BoneData::~BoneData()
{
}

glm::mat4 BoneData::GetTransformation()
{
	std::deque<glm::mat4> transformations;

	auto parent = transform;
	while (parent != nullptr)
	{
		transformations.push_front(parent->transform.getLocalTransformation());
		parent = parent->GetParent();
	}

	// remove root 
	transformations.pop_front();

	// calculate
	glm::mat4 finalTransformation = glm::mat4(1.0);
	while (transformations.size() > 0)
	{
		finalTransformation *= transformations.front();
		transformations.pop_front();
	}

	finalTransformation *= offsetMatrix;
	return finalTransformation;
}
