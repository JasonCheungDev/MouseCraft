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
		if (smooth)
			GetEntity()->t().pos(glm::lerp(GetEntity()->t().wPos(), target->t().wPos(), dt * speed));
		else
			GetEntity()->t().pos(target->transform.wPos());
	}
}
