// Map.h

#ifndef _MAP_h
#define _MAP_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

using namespace std;

const unsigned short SIZE = 6;
const double CELL = 18;

enum Dir {
	NORTH = 0,
	EAST,
	SOUTH,
	WEST,
	INVALID
};

inline Dir opposite(Dir d) {
	switch (d) {
	case NORTH:
		return SOUTH;
	case SOUTH:
		return NORTH;
	case EAST:
		return WEST;
	case WEST:
		return EAST;
	case INVALID:
	default:
		return INVALID;
	}
}

inline Dir rightDir(Dir d) {
	switch (d) {
	case NORTH:
		return EAST;
	case SOUTH:
		return WEST;
	case EAST:
		return SOUTH;
	case WEST:
		return NORTH;
	case INVALID:
	default:
		return INVALID;
	}
}

inline Dir leftDir(Dir d) {
	switch (d) {
	case NORTH:
		return WEST;
	case SOUTH:
		return EAST;
	case EAST:
		return NORTH;
	case WEST:
		return SOUTH;
	case INVALID:
	default:
		return INVALID;
	}
}

struct Coordinate
{
	int8_t x;
	int8_t y;
	Coordinate(const int8_t &a = 0, const int8_t &b = 0):x(a),y(b){}
};

const Coordinate NULL_COORD = Coordinate(-1, -1);

inline bool operator<(const Coordinate & a, const Coordinate &b)
{
	if (a.x == b.x) return a.y < b.y;
	else return a.x < b.x;
}

inline bool operator==(const Coordinate & a, const Coordinate &b)
{
	return (a.x == b.x) && (a.y == b.y);
}

inline bool operator!=(const Coordinate & a, const Coordinate &b)
{
	return (a.x != b.x) || (a.y != b.y);
}

Coordinate getCoord(const Coordinate &a, const Dir &dir);
	

Dir getDir(const Coordinate & now, const Coordinate &next);

struct Cell
{
	bool wallNorth;
	bool wallEast;
	bool wallSouth;
	bool wallWest;

	bool visited;

	Cell(bool N = false, bool E = false, bool S = false, bool W = false, bool V = false)
		:wallNorth(N), wallEast(E), wallSouth(S), wallWest(W), visited(V) {}
};


#endif

