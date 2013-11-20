#include "func.h"

namespace math2d
{
	float point_distance(const point2d_t &p1, const point2d_t &p2)
	{
		return sqrtf(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
	}
}