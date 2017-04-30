// Maze.h

#ifndef _MAZE_h
#define _MAZE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Map.h"
#include "Led.h"
#include "Movement.h"
using namespace std;

class MazeClass
{
 public:
	 Cell maze[SIZE][SIZE];
	 Coordinate curLocation;
	 Dir curDirection;
	 Dir nextDirection;
	 double curDistance;

	 LedClass* led;
	 MovementClass* move;

	 MazeClass(LedClass* a, MovementClass* b)
	 {
		 led = a;
		 move = b;
		 for (short int i = 0; i < SIZE; i++)
		 {
			 for (short int j = 0; j < SIZE; j++)
			 {
				 maze[i][j] = Cell();

				 //left most
				 if (i == 0) maze[i][j].wallWest = true;

				 //right most
				 if (i == (SIZE - 1)) maze[i][j].wallEast = true;

				 //bot most
				 if (j == 0) maze[i][j].wallSouth = true;

				 //top most
				 if (j == (SIZE - 1)) maze[i][j].wallNorth = true;

			 }
		 }
		 //start cell
		 maze[0][0] = Cell(false, true, true, true, true);
		 curLocation = Coordinate(0, 0);
		 curDirection = NORTH;
		 nextDirection = NORTH;
	 }

};



#endif

