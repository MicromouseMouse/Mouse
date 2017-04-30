#include "Map.h"

Coordinate getCoord(const Coordinate &a, const Dir &dir)
{
	switch (dir)
	{
	case NORTH:
		if (a.y >= SIZE - 1) return NULL_COORD;
		else return Coordinate(a.x, a.y + 1);
	case SOUTH:
		if (a.y <= 0) return NULL_COORD;
		else return Coordinate(a.x, a.y - 1);
	case EAST:
		if (a.x >= SIZE - 1) return NULL_COORD;
		else return Coordinate(a.x + 1, a.y);
	case WEST:
		if (a.x <= 0) return NULL_COORD;
		else return Coordinate(a.x - 1, a.y);
	default:
		return NULL_COORD;
	}
}

Dir getDir(const Coordinate & now, const Coordinate &next)
{
	if (now.x == next.x)
	{
		if (next.y == now.y + 1) return NORTH;
		else return SOUTH;
	}
	else
	{
		if (next.x == now.x + 1) return EAST;
		else return WEST;
	}
}