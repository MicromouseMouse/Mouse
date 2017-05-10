// Maze.h

#ifndef _MAZE_h
#define _MAZE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Map.h"



class SoftWareSerial;
class LedClass;
class MovementClass;
using namespace std;

class MazeClass
{
 public:
	 Cell maze[SIZE][SIZE];
	 Coordinate curLocation;
	 Dir curDirection;
	 Dir nextDirection;
	 double curDistance;

	 int counter;
	 //stack<Coordinate> path;

	 LedClass* led;
	 MovementClass* move;

	 MazeClass(LedClass* a, MovementClass* b);

	 void updateMap();
	 
	 void mapping();

	 //void floodFill(const Coordinate &end);

	 String printMap();

};



#endif

