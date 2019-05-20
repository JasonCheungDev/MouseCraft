#include "PostProcess.h"

bool PostProcess::ComparePriority(const PostProcess * lhs, const PostProcess * rhs)
{
	return rhs->priority < lhs->priority;
}
