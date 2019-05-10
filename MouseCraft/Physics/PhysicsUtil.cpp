#include "PhysicsUtil.h"

glm::vec2 b2gl2(b2Vec2 vec)
{
	return glm::vec2(vec.x, vec.y);
}

glm::vec3 b2gl3(b2Vec2 vec)
{
	return glm::vec3(vec.x, 0, vec.y);
}

b2Vec2 gl2b2(glm::vec2 vec)
{
	return b2Vec2(vec.x, vec.y);
}

b2Vec2 gl3b2(glm::vec3 vec)
{
	return b2Vec2(vec.x, vec.z);
}
