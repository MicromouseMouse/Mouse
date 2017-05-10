// 
// 
// 
#include "Maze.h"
#include "Led.h"
#include "Movement.h"
#include <SoftwareSerial.h>



SoftwareSerial bt(7, 8);

MazeClass::MazeClass(LedClass* a, MovementClass* b)
{
	counter = 1;
	led = a;
	move = b;
	for (int j = SIZE -1; j >= 0 ; j--)
	{
		for (int i = 0; i < SIZE  ; i++)
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
	curDistance = 0;
}

void MazeClass::updateMap()
{
	bool front = (led->getLed(LEFT_REAR) + led->getLed(RIGHT_REAR) > 15000);
	bool left =  led->left_diagonal > 8000;
	bool right = led->right_diagonal > 8000;

	switch (curDirection)
	{
	case NORTH:
		curLocation = Coordinate(curLocation.x, curLocation.y + 1);
		maze[curLocation.x][curLocation.y].wallNorth = front;
		maze[curLocation.x][curLocation.y].wallEast = right;
		maze[curLocation.x][curLocation.y].wallWest = left;
		maze[curLocation.x][curLocation.y].visited = true;

		if (curLocation.y < SIZE - 1)									// cell north
			maze[curLocation.x][curLocation.y + 1].wallSouth = front;
		if (curLocation.x > 0)											// cell west
			maze[curLocation.x - 1][curLocation.y].wallEast = left;
		if (curLocation.x < SIZE - 1)									// cell east
			maze[curLocation.x + 1][curLocation.y].wallWest = right;

		break;
	case SOUTH:
		curLocation = Coordinate(curLocation.x, curLocation.y - 1);
		maze[curLocation.x][curLocation.y].wallSouth = front;
		maze[curLocation.x][curLocation.y].wallWest = right;
		maze[curLocation.x][curLocation.y].wallEast = left;
		maze[curLocation.x][curLocation.y].visited = true;

		if (curLocation.y > 0)											// cell south
			maze[curLocation.x][curLocation.y - 1].wallNorth = front;
		if (curLocation.x > 0)											// cell west
			maze[curLocation.x - 1][curLocation.y].wallEast = right;
		if (curLocation.x < SIZE - 1)									// cell east
			maze[curLocation.x + 1][curLocation.y].wallWest = left;

		break;
	case EAST:
		curLocation = Coordinate(curLocation.x + 1, curLocation.y);
		maze[curLocation.x][curLocation.y].wallEast = front;
		maze[curLocation.x][curLocation.y].wallSouth = right;
		maze[curLocation.x][curLocation.y].wallNorth = left;
		maze[curLocation.x][curLocation.y].visited = true;

		//cell east
		if (curLocation.x < SIZE - 1)
			maze[curLocation.x + 1][curLocation.y].wallWest = front;
		//cell north
		if (curLocation.y < SIZE - 1)
			maze[curLocation.x][curLocation.y + 1].wallSouth = left;
		//cell south
		if (curLocation.y > 0)
			maze[curLocation.x][curLocation.y - 1].wallNorth = right;

		break;
	case WEST:
		curLocation = Coordinate(curLocation.x - 1, curLocation.y);
		maze[curLocation.x][curLocation.y].wallWest = front;
		maze[curLocation.x][curLocation.y].wallNorth = right;
		maze[curLocation.x][curLocation.y].wallSouth = left;
		maze[curLocation.x][curLocation.y].visited = true;

		//cell west
		if (curLocation.x > 0)
			maze[curLocation.x -1][curLocation.y].wallEast = front;
		//cell south
		if (curLocation.y  > 0)
			maze[curLocation.x][curLocation.y - 1].wallNorth = left;
		//cell north
		if (curLocation.y < SIZE - 1)
			maze[curLocation.x][curLocation.y + 1].wallSouth = right;

		break;
	}
}

void MazeClass::mapping()
{
	if (move->getDistanceTravel() > 18.0*counter )
	{
		++counter;
		updateMap();
		bt.println("update map");
	}
}

String MazeClass::printMap()
{
	Serial.begin(9600);
	String s = "";
	for (int j = SIZE - 1; j >= 0; j--)
	{
		for (int i = 0; i < SIZE; i++)
		{
			if (maze[i][j].wallNorth) s += "N";
			else s += "0";
			if (maze[i][j].wallEast) s += "E";
			else s += "0";
			if (maze[i][j].wallSouth) s += "S";
			else s += "0";
			if (maze[i][j].wallWest) s += "W";
			else s += "0";
			if (maze[i][j].visited) s += "#";
			else s += "0";
			s += " ";
		}
		s += "\n";
	}
	return s;
}
/*
void MazeClass::floodFill(const Coordinate &end)
{
	queue<Coordinate> way;
	way.push(curLocation);
	while (!way.Empty())
	{
		Coordinate temp = way.front();
		way.pop();
		if (curLocation.y < SIZE - 1 && maze[curLocation.x][curLocation.y].wallNorth == false)
			way.push(Coordinate(curLocation.x,curLocation.y + 1));
		
		if (curLocation.x < SIZE - 1 && maze[curLocation.x][curLocation.y].wallEast == false)
			way.push(Coordinate(curLocation.x + 1, curLocation.y));
		
		if (curLocation.y > 0 && maze[curLocation.x][curLocation.y].wallSouth == false)
			way.push(Coordinate(curLocation.x, curLocation.y - 1));
		
		if (curLocation.x > 0 && maze[curLocation.x][curLocation.y].wallNorth == false)
			way.push(Coordinate(curLocation.x - 1, curLocation.y));
	}

}
*/