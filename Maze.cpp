// 
// 
// 
#include "Maze.h"
#include "Led.h"
//#include "Movement.h"
#include <SoftwareSerial.h>


SoftwareSerial bt(7, 8);

struct mazeCoordinate
{
	int x;
	int y;
	int value;
	mazeCoordinate(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

const int SIZEX = 16;
const int SIZEY = 16;
const int centerx = 7;
const int centery = 7;
const int centerx2 = 8;
const int centery2 = 7;
const int centerx3 = 8;
const int centery3 = 7;
const int centerx4 = 8;
const int centery4 = 8;

void MazeClass::testMaze()
{
	mazeCoordinate center1(centerx, centery);
	mazeCoordinate center2(centerx2, centery2);
	mazeCoordinate center3(centerx3, centery2);
	mazeCoordinate center4(centerx4, centery4);
	int temp1[SIZEX][SIZEY];
	int temp2[SIZEX][SIZEY];

	for (int i = 0; i < SIZEY; i++)
	{
		for (int j = 0; j < SIZEX; j++)
		{
			temp1[j][i] = 0;
			temp2[j][i] = 0;
		}
	}

	for (int i = 0; i < SIZEY; i++)
		for (int j = 0; j < center1.x; j++)
			temp1[j][i] = absolute(center1.x - j);
	for (int i = 0; i < SIZEY; i++)
		for (int j = center2.x; j < SIZEX; j++)
			temp1[j][i] = absolute(center2.x - j);


	for (int i = 0; i < SIZEX; i++)
		for (int j = 0; j < center1.y; j++)
			temp2[i][j] = absolute(center1.y - j);
	for (int i = 0; i < SIZEX; i++)
		for (int j = center4.y; j < SIZEY; j++)
			temp2[i][j] = absolute(center4.y - j);

	for (int i = 0; i < SIZEY; i++)
		for (int j = 0; j < SIZEX; j++)
			path[j][i] = temp1[j][i] + temp2[j][i];


}
MazeClass::MazeClass(LedClass* a, MovementClass* b)
{
	mapFlag = false;
	counter = 1;
	led = a;
	move = b;
	curDistance = 0;
	for (int j = SIZE -1; j >= 0 ; j--)
	{
		for (int i = 0; i < SIZE  ; i++)
		{
			maze[i][j] = Cell();
			path[i][j] = 0;
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

void MazeClass::updateMap()
{
	bool front = (led->getLed(LEFT_REAR) + led->getLed(RIGHT_REAR) > 6000);
	bool left =  led->left_diagonal > led->leftThreshold*0.5;
	bool right = led->right_diagonal > led->rightThreshold*0.85;

	switch (curDirection)
	{
	case NORTH:
		curLocation = Coordinate(curLocation.x, curLocation.y + 1);
		if (maze[curLocation.x][curLocation.y].visited == true) break;
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
		if (maze[curLocation.x][curLocation.y].visited == true) break;
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
		if (maze[curLocation.x][curLocation.y].visited == true) break;
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
		if (maze[curLocation.x][curLocation.y].visited == true) break;
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
	default:
		exit(1);
	}
}

void MazeClass::mapping()
{
	if (move->getDistanceTravel() > 18.0*counter - 7 && mapFlag == false)
	{
		mapFlag = true;
		updateMap();
		floodFill(Coordinate(8,8));
		//command();
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

String MazeClass::printPath()
{
	String s = "";
	while (!realPath.isEmpty())
	{
		Coordinate check = realPath.peek();
		realPath.pop();
		s += check.x;
		s += " ";
		s += check.y;
		s += "\n";
	}
	return s;
}

String MazeClass::printFloodFill()
{
	String s = "";
	for (int j = SIZE - 1; j >= 0; j--)
	{
		for (int i = 0; i < SIZE; i++)
		{
			String hold(path[i][j]);
			s += hold;
			s += " ";
			if (path[i][j] < 10) s += " ";
		}
		s += "\n";
	}
	return s;
}

void MazeClass::floodFill(const Coordinate &end)
{


	QueueList<MazeCount> way;
	way.push(MazeCount(curLocation, 0));
	bool checked[SIZE][SIZE];
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			checked[i][j] = false;
			path[i][j] = 0;
		}
	}
	checked[curLocation.x][curLocation.y] = true;
	while (!way.isEmpty())
	{
		MazeCount temp = way.peek();
		way.pop();
		path[temp.current.x][temp.current.y] = temp.count;

		if (temp.current.y < SIZE - 1 && temp.current.y >= 0									//check North
			&& maze[temp.current.x][temp.current.y].wallNorth == false
			&& checked[temp.current.x][temp.current.y + 1] == false)
		{
			way.push(MazeCount(Coordinate(temp.current.x, temp.current.y + 1), temp.count + 1));
			checked[temp.current.x][temp.current.y + 1] = true;
		}

		if (temp.current.x < SIZE - 1 && temp.current.x >= 0					//check East
			&& maze[temp.current.x][temp.current.y].wallEast == false
			&& checked[temp.current.x + 1][temp.current.y] == false)
		{
			way.push(MazeCount(Coordinate(temp.current.x + 1, temp.current.y), temp.count + 1));
			checked[temp.current.x + 1][temp.current.y] = true;
		}

		if (temp.current.y > 0 && temp.current.y < SIZE											//check South
			&& maze[temp.current.x][temp.current.y].wallSouth == false
			&& checked[temp.current.x][temp.current.y - 1] == false)
		{
			way.push(MazeCount(Coordinate(temp.current.x, temp.current.y - 1), temp.count + 1));
			checked[temp.current.x][temp.current.y - 1] = true;
		}

		if (temp.current.x > 0 && temp.current.x < SIZE		//check West
			&& maze[temp.current.x][temp.current.y].wallWest == false
			&& checked[temp.current.x - 1][temp.current.y] == false)
		{
			way.push(MazeCount(Coordinate(temp.current.x - 1, temp.current.y), temp.count + 1));
			checked[temp.current.x - 1][temp.current.y] = true;
		}
	}
	if (curLocation.x >= SIZE || curLocation.x < 0 || curLocation.y < 0 || curLocation.y >= SIZE)
	{
		analogWrite(13, 1023);
		move->stop();
		return;
	}

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			checked[i][j] = false;
		}
	}

	while (!realPath.isEmpty())
	{
		realPath.pop();
	}
	
	Coordinate find = curLocation;
	Dir psuDir = curDirection;
	int step = 0;
	while (find != end)
	{
		StackList<Coordinate> shortest;
		++step;
		if (step > 260)
		{
			move->stop();
			continue;
		}
		if (find.x > 0 && find.x <SIZE)  // check WEST
		{
			Coordinate next = getCellDir(find, WEST);
			if (path[next.x][next.y] == step && checked[next.x][next.y] == false)
			{
				shortest.push(next);
				checked[next.x][next.y] = true;
			}
		}
		if (find.y < SIZE - 1 && find.y >=0)  // check NORTH
		{
			Coordinate next = getCellDir(find, NORTH);
			if (path[next.x][next.y] == step && checked[next.x][next.y] == false)
			{
				shortest.push(next);
				checked[next.x][next.y] = true;
			}
		}
		if (find.x < SIZE - 1 && find.x >= 0)  // check EAST
		{
			Coordinate next = getCellDir(find, EAST);
			if (path[next.x][next.y] == step && checked[next.x][next.y] == false)
			{
				shortest.push(next);
				checked[next.x][next.y] = true;
			}
		}
		if (find.y > 0 && find.y < SIZE)  // check SOUTH
		{
			Coordinate next = getCellDir(find, SOUTH);
			if (path[next.x][next.y] == step && checked[next.x][next.y] == false)
			{
				shortest.push(next);
				checked[next.x][next.y] = true;
			}
		}
		if (shortest.isEmpty())
		{
			digitalWrite(13, HIGH);
			break;
		}

		Coordinate final = shortest.peek();
		int bug = 0;
		do
		{
			bug++;
			if (bug > 500)
			{
				move->stop();
				continue;
			}
			Coordinate compare = shortest.peek();
			shortest.pop();
			int dis1 = shortestDistance(compare, end);
			int dis2 = shortestDistance(final, end);
			if (dis1 < dis2 || (dis1 == dis2 && getDirToGo(find, compare) == psuDir) || compare == end || final == end) // if less or equal but straight
			{
				final = compare;
			}
		}
		while (!shortest.isEmpty());
		
		realPath.push(final);
		psuDir = getDirToGo(find, final);
		find = final;
		
	}
}

Turn MazeClass::getTurnDir(const Dir &next)
{
	switch (next - curDirection)
	{
	case 0:
		return NO_TURN;
	case 2:
	case -2:
		return BACK;
	case 1:
	case -3:
		return LEFT;
	default:
		return RIGHT;
	}
}

void MazeClass::command()
{
	if (move->getDistanceTravel() < 18.0*counter - 1.25) 
	return;
	++counter;
	move->stopForward();
	mapFlag = false;
	delay(500);
	move->resetEncoder();
	return;
	if (realPath.isEmpty()) move->stopForward();

	Coordinate next = realPath.peek();
	realPath.pop();
	Dir temp = getDirToGo(curLocation, next);
	if (temp == curDirection) return;
	else
	{
		
		move->stopForward();
		move->turn_encoder(getTurnDir(temp));
		curDirection = temp;
		move->goForward();
	}
}

void MazeClass::simpleMap()
{
	if (move->getDistanceTravel() < 18.0*counter - 9 && mapFlag == false)
	{
		mapFlag = true;
		//front = (led->getLed(LEFT_REAR) + led->getLed(RIGHT_REAR) > led->frontThreshold *0.6);
		//left = led->left_diagonal > led->leftThreshold*0.6;
		//right = led->right_diagonal > led->rightThreshold*0.6;
	}
}

void MazeClass::simpleFill()
{
	
	if (move->getDistanceTravel() < 18.0*counter) return;
	++counter;
	mapFlag = false;

	Coordinate next = realPath.peek();
	realPath.pop();
	Dir temp = getDirToGo(curLocation, next);
	if (temp == curDirection) return;
	else
	{

		move->stopForward();
		move->turn_encoder(getTurnDir(temp));
		curDirection = temp;
		move->goForward();
	}
}