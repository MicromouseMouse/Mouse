#include "Maze.h"
#include "Led.h"
#include "Movement.h"


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

int shortestDistance(const Coordinate &a, const Coordinate &b)
{
	return absolute(a.x - b.x) + absolute(a.y - b.y);
}

Coordinate getCellDir(const Coordinate &cur, const Dir &dir)
{
	switch (dir)
	{
	case NORTH:
		if (cur.y < SIZE - 1) return Coordinate(cur.x, cur.y + 1);
		else return NULL_COORD;
		break;
	case SOUTH:
		if (cur.y > 0) return Coordinate(cur.x, cur.y - 1);
		else return NULL_COORD;
		break;
	case EAST:
		if (cur.x < SIZE - 1) return Coordinate(cur.x + 1, cur.y);
		else return NULL_COORD;
		break;
	case WEST:
		if (cur.x > 0) return Coordinate(cur.x - 1, cur.y);
		else return NULL_COORD;
		break;
	default:
		return NULL_COORD;
	}
}

Dir getDirToGo(const Coordinate &cur, const Coordinate &next)
{
	if (cur.x + 1 == next.x) return EAST;
	else if (cur.x - 1 == next.x) return WEST;
	else if (cur.y + 1 == next.y) return NORTH;
	else return SOUTH;
}