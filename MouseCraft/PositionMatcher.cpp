#include "PositionMatcher.h"



PositionMatcher::PositionMatcher()
{
}


PositionMatcher::~PositionMatcher()
{
}

void PositionMatcher::FixedUpdate(float dt, int steps)
{
	if (target)
	{
		GetEntity()->t().pos(
			glm::lerp(GetEntity()->t().pos(), target->t().pos(), dt * speed));
	}
}
