#include "OrientationMatcher.h"



OrientationMatcher::OrientationMatcher()
{
}


OrientationMatcher::~OrientationMatcher()
{
}

void OrientationMatcher::FixedUpdate(float dt, int steps)
{
	if (target)
	{
		GetEntity()->t().setLocalRotation(
			glm::slerp(
				GetEntity()->t().getLocalQuaternion(), 
				target->t().getLocalQuaternion(), 
				dt * speed));
	}
}
