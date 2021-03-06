// Maze.h

#ifndef _MAZE_h
#define _MAZE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Map.h"
#include "Movement.h"
#include <StackList\StackList\StackList.h>
#include <QueueList\QueueList.h>

enum Turn;

class SoftWareSerial;
class LedClass;
class MovementClass;
using namespace std;

class MazeClass
{
 public:
	 Cell maze[SIZE][SIZE];
	 int path[SIZE][SIZE];
	 Coordinate curLocation;
	 Coordinate nextLocation;
	 Dir curDirection;
	 Dir nextDirection;
	 double curDistance;
	 bool mapFlag;
	 int counter;
	 int loopCounter;
	 StackList<Coordinate> pastPath;

	 LedClass* led;
	 MovementClass* move;

	 MazeClass(LedClass* a, MovementClass* b);

	 void updateMap();
	 bool getWall(const Dir &a);
	 bool getWallDir(const Turn &a);
	 void mapping();

	 void floodFill(const Coordinate &end);
	 
	 int command(bool force);

	 Turn getTurnDir(const Dir &next);

	 String printMap();

	 String printPath();

	 String printFloodFill();

	 void stopAlign();
};



#endif

