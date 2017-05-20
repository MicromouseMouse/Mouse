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
	 Dir curDirection;
	 Dir nextDirection;
	 double curDistance;
	 bool mapFlag;
	 int counter;
	 QueueList<Coordinate> realPath;

	 LedClass* led;
	 MovementClass* move;

	 MazeClass(LedClass* a, MovementClass* b);

	 void updateMap();
	 
	 void mapping();

	 void floodFill(const Coordinate &end);

	 void command();

	 Turn getTurnDir(const Dir &next);

	 String printMap();

	 String printPath();

	 String printFloodFill();

	 void testMaze();
	 void simpleFill();
	 void simpleMap();

};



#endif

