// 
// 
// 
#include "Maze.h"
#include "Led.h"
//#include "Movement.h"
#include <SoftwareSerial.h>
#include <cstring>
#include <cstdlib>

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


const int centerx = 7;
const int centery = 7;
const int centerx2 = 8;
const int centery2 = 7;
const int centerx3 = 8;
const int centery3 = 7;
const int centerx4 = 8;
const int centery4 = 8;

void MazeClass::simpleFill()
{
	mazeCoordinate center1(centerx, centery);
	mazeCoordinate center2(centerx2, centery2);
	mazeCoordinate center3(centerx3, centery2);
	mazeCoordinate center4(centerx4, centery4);

	int temp1[SIZE][SIZE];
	int temp2[SIZE][SIZE];

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			temp1[j][i] = 0;
			temp2[j][i] = 0;
		}
	}


	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < center1.x; j++)
			temp1[j][i] = absolute(center1.x - j);
	for (int i = 0; i < SIZE; i++)
		for (int j = center2.x; j < SIZE; j++)
			temp1[j][i] = absolute(center2.x - j);


	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < center1.y; j++)
			temp2[i][j] = absolute(center1.y - j);
	for (int i = 0; i < SIZE; i++)
		for (int j = center4.y; j < SIZE; j++)
			temp2[i][j] = absolute(center4.y - j);

	
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			path[j][i] = temp1[j][i] + temp2[j][i];

}

void MazeClass::simpleTravel()
{
	if (move->getDistanceTravel() < 18.0*counter - 1.15)
		return;
	mapFlag = false;
	int x = curLocation.x;
	int y = curLocation.y;
	int value = path[x][y];
	QueueList<Coordinate> holder;
	for (int i = 0; i < 3; i++)
	{
		Coordinate next = getCellDir(curLocation, (Dir)i);
		if (next == NULL_COORD) continue;
		else holder.push(next);
	}
	Coordinate final = holder.pop();
	while (!holder.isEmpty())
	{
		Coordinate temp = holder.pop();
		if ((path[temp.x][temp.y] < path[final.x][final.y]) 
			|| (( path[temp.x][temp.y] == path[final.x][final.y]) && getDir(curLocation,temp) == curDirection))
		{
			final = temp;
		}
	}

	Dir tempDir = getDirToGo(curLocation, final);
	if (tempDir == curDirection) {
		counter++;
		return;
	}
	else
	{
		move->stopForward();
		delay(100);
		move->turn_encoder(getTurnDir(tempDir));
		curDirection = tempDir;
		move->goForward();
	}
	counter = 1;
	move->resetEncoder();
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
	nextLocation = Coordinate(0, 1);
	curDirection = NORTH;
	nextDirection = NORTH;
	loopCounter = 0;
	
}

bool MazeClass::getWall(const Dir &a)
{
	int x = curLocation.x;
	int y = curLocation.y;
	switch (a)
	{
	case NORTH:
		return maze[x][y].wallNorth;
	case EAST:
		return maze[x][y].wallEast;
	case SOUTH:
		return maze[x][y].wallSouth;
	case WEST:
		return maze[x][y].wallWest;
	default:
		exit(1);
	}
}

bool MazeClass::getWallDir(const Turn &a)
{
	switch (a)
	{
	case LEFT:
		return getWall(leftDir(curDirection));
	case RIGHT:
		return getWall(rightDir(curDirection));
	case NO_TURN:
		return getWall(curDirection);
	case BACK:
		return getWall(opposite(curDirection));
	default:
		exit(1);
	}
}

void MazeClass::updateMap()
{
	
	bool front = (led->getLed(LEFT_REAR) + led->getLed(RIGHT_REAR) > 8100);
	bool left =  led->left_diagonal > led->leftThreshold*0.48;
	bool right = led->right_diagonal > led->rightThreshold*0.48;

	switch (curDirection)
	{
	case NORTH:
		curLocation = Coordinate(curLocation.x, curLocation.y + 1);
		if (maze[curLocation.x][curLocation.y].visited == true) return;
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
		if (maze[curLocation.x][curLocation.y].visited == true) return;
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
		if (maze[curLocation.x][curLocation.y].visited == true) return;
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
		if (maze[curLocation.x][curLocation.y].visited == true) return;
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
	if (move->getDistanceTravel() > 18.0*counter - 4.4 && mapFlag == false)
	{
		mapFlag = true;
		updateMap();
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
	StackList<Coordinate> temp;
	while (!pastPath.isEmpty())
	{
		Coordinate check = pastPath.peek();
		pastPath.pop();
		s += check.x;
		s += " ";
		s += check.y;
		s += "\n";
		temp.push(check);
	}
	pastPath = temp;
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
	way.push(MazeCount(Coordinate(7,7), 0));
	way.push(MazeCount(Coordinate(7,8), 0));
	way.push(MazeCount(Coordinate(8,7), 0));
	way.push(MazeCount(Coordinate(8,8), 0));
	
	bool checked[SIZE][SIZE];
	memset(path, 0, sizeof(path[0][0]) * SIZE*SIZE);
	memset(checked, 0, sizeof(checked[0][0]) * SIZE*SIZE);

	checked[7][7] = true;
	checked[7][8] = true;
	checked[8][7] = true;
	checked[8][8] = true;

	while (!way.isEmpty())
	{
		MazeCount temp = way.peek();
		way.pop();
		int x = temp.current.x;
		int y = temp.current.y;
		int cost = temp.count;
		path[x][y] = temp.count;

		if (y < SIZE - 1 && y >= 0									//check North
			&& maze[x][y].wallNorth == false
			&& checked[x][y + 1] == false)
		{
			way.push(MazeCount(Coordinate(x, y + 1), cost + 1));
			checked[x][y + 1] = true;
		}

		if (x < SIZE - 1 && x >= 0					//check East
			&& maze[x][y].wallEast == false
			&& checked[x + 1][y] == false)
		{
			way.push(MazeCount(Coordinate(x + 1, y), cost + 1));
			checked[x + 1][y] = true;
		}

		if (y > 0 && y < SIZE											//check South
			&& maze[x][y].wallSouth == false
			&& checked[x][y - 1] == false)
		{
			way.push(MazeCount(Coordinate(x, y - 1), cost + 1));
			checked[x][y - 1] = true;
		}

		if (x > 0 && x < SIZE		//check West
			&& maze[x][y].wallWest == false
			&& checked[x - 1][y] == false)
		{
			way.push(MazeCount(Coordinate(x - 1, y), cost + 1));
			checked[x - 1][y] = true;
		}
	}

	if (curLocation.x >= SIZE || curLocation.x < 0 || curLocation.y < 0 || curLocation.y >= SIZE)
	{
		analogWrite(13, 1023);
		move->stop();
		return;
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

int MazeClass::command(bool force)
{
	if (move->getDistanceTravel() < 18.0*counter - 1.1 && force == false)
		return 0;
	//++counter;
	int x = curLocation.x;
	int y = curLocation.y;
	Coordinate tempFront = getCellDir(curLocation, curDirection);
	Coordinate tempLeft = getCellDir(curLocation, leftDir(curDirection));
	Coordinate tempRight = getCellDir(curLocation, rightDir(curDirection));

	
	if (getWallDir(NO_TURN))
	{
		while (true)
		{
			if (led->getLed(LEFT_REAR) + led->getLed(RIGHT_REAR) > led->frontThreshold*0.55)
			{
				move->stopForward();
				break;
			}
			move->goForward();
			led->measure(10);
		}
	}
	else if(force == false) move->stopForward();
	
	if ((getWallDir(NO_TURN) && getWallDir(LEFT) && getWallDir(RIGHT)) 
		|| (loopCounter > 4 ))
	{
		move->turn_encoder(BACK);
		move->resetEncoder();
		curDirection = opposite(curDirection);
		floodFill(Coordinate(0, 0));
		mapFlag = false;
		loopCounter = 0;
		return 1;
	}
	if (!getWallDir(NO_TURN))
	{
		//Coordinate temp = getCellDir(curLocation,curDirection);
		if (path[x][y] > path[tempFront.x][tempFront.y])
		{
			move->resetEncoder();
			mapFlag = false;
			return 1;
		}
	}
	if (!getWallDir(LEFT))
	{
		//Coordinate temp = getCellDir(curLocation, leftDir(curDirection));
		if (path[x][y] > path[tempLeft.x][tempLeft.y])
		{
			move->resetEncoder();
			move->turn_encoder(LEFT);
			curDirection = leftDir(curDirection);
			mapFlag = false;
			return 1;
		}
	}
	if (!getWallDir(RIGHT))
	{
		//Coordinate temp = getCellDir(curLocation, rightDir(curDirection));
		if (path[x][y] > path[tempRight.x][tempRight.y])
		{
			move->resetEncoder();
			move->turn_encoder(RIGHT);
			curDirection = rightDir(curDirection);
			mapFlag = false;
			return 1;
		}
	}
	
	floodFill(Coordinate(0, 0));
	//bt.println(printFloodFill());
	//bt.println(loopCounter);
	++loopCounter;
	return -1;
}


void MazeClass::randomMapping()
{
	if (move->getDistanceTravel() < 18.0*counter - 3)
		return;
	//move->stopForward();
	Turn randomDirection = determineRandomMotion();
	switch (randomDirection)
	{
	case NO_TURN:
		++counter;
		return;
	case LEFT:
		move->stopForward();
		move->turn_encoder(LEFT);
		break;
	case RIGHT:
		move->stopForward();
		move->turn_encoder(RIGHT);
		break;
	case BACK:
		move->stopForward();
		move->turn_encoder(BACK);
		break;
	}
	counter = 1;
	move->resetEncoder();

}

Turn MazeClass::determineRandomMotion()
{
	led->measure(10);
	bool leftOpening = led->getLed(LEFT_DIAGONAL) < led->leftThreshold*0.7;
	bool rightOpening = led->getLed(RIGHT_DIAGONAL) < led->rightThreshold*0.7;
	bool frontOpening = led->getLed(RIGHT_REAR) + led->getLed(LEFT_REAR) < led->frontThreshold*0.6;
	if (leftOpening && rightOpening && frontOpening)
	{
		switch (led->right_rear % 3)
		{
		case 0:
			return NO_TURN;
		case 1:
			// turn left;
			return LEFT;
		case 2:
			// turn left;
			return RIGHT;
		}
	}

	else if (leftOpening && frontOpening)
	{
		switch (led->left_rear % 2)
		{
		case 0:
			return NO_TURN;
		case 1:
			// turn left;
			return LEFT;
		}
	}

	else if (leftOpening && rightOpening)
	{
		switch (led->left_diagonal % 2)
		{
		case 0:
			return LEFT;
		case 1:
			// turn left;
			return RIGHT;
		}
	}

	else if (frontOpening && rightOpening)
	{	
		switch (led->right_diagonal % 2)
		{

		case 0:
			return NO_TURN;
		case 1:
			// turn right;
			return RIGHT;
		}
	}
	else if (!frontOpening && !leftOpening && !rightOpening)
		return BACK;

	else if (leftOpening)
		return LEFT;
	else if (rightOpening)
		return RIGHT;
	else 
		return NO_TURN;

}