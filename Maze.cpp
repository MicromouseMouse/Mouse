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
	analogWrite(13, 1023);
	led->measure(10);
	bool front = (led->getLed(LEFT_REAR) + led->getLed(RIGHT_REAR) > 7700);
	bool left = led->left_diagonal > 19500;// led->leftThreshold*0.9;
	bool right = led->right_diagonal > 16200;// led->rightThreshold*0.8;

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
	
	floodFill(Coordinate(0, 0));

}

void MazeClass::mapping()
{
	if (move->getDistanceTravel() > 18.0*counter - 6 - 035*counter && mapFlag == false)
	{
		mapFlag = true;
		updateMap();
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
	//way.push(MazeCount(Coordinate(4, 4), 0));
	
	bool checked[SIZE][SIZE];
	memset(path, 0, sizeof(path[0][0]) * SIZE*SIZE);
	memset(checked, 0, sizeof(checked[0][0]) * SIZE*SIZE);

	checked[7][7] = true;
	checked[7][8] = true;
	checked[8][7] = true;
	checked[8][8] = true;

	//checked[4][4] = true;

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
	if (move->getDistanceTravel() < 18.0*counter - -0.5 - 0.15*counter && force == false)
		return 0;
	//++counter;
	int x = curLocation.x;
	int y = curLocation.y;
	Coordinate tempFront = getCellDir(curLocation, curDirection);
	Coordinate tempLeft = getCellDir(curLocation, leftDir(curDirection));
	Coordinate tempRight = getCellDir(curLocation, rightDir(curDirection));


	if (getWallDir(NO_TURN))
	{
		stopAlign();
	}

	
	//move->stopForward();
	if (!getWallDir(NO_TURN))
	{
		//Coordinate temp = getCellDir(curLocation,curDirection);
		if (path[x][y] > path[tempFront.x][tempFront.y])
		{
			//counter++;
			move->resetEncoder();
			mapFlag = false;
			return 5;
		}
	}
	
	if (!getWallDir(LEFT))
	{
		//Coordinate temp = getCellDir(curLocation, leftDir(curDirection));
		if (path[x][y] > path[tempLeft.x][tempLeft.y])
		{
			move->stopForward();
			move->resetEncoder();
			
			
			/*
			delay(300);
			bt.print(curLocation.x);
			bt.print(" ");
			bt.println(curLocation.y);

			bt.println(printFloodFill());
			bt.println(printMap());
			delay(300);
			*/
			
		
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
			move->stopForward();
			move->resetEncoder();

			
			/*
			delay(300);
			bt.print(curLocation.x);
			bt.print(" ");
			bt.println(curLocation.y);

			bt.println(printFloodFill());
			bt.println(printMap());
			delay(300);
			*/
			

			move->turn_encoder(RIGHT);
			curDirection = rightDir(curDirection);
			mapFlag = false;
			return 1;
		}
	}
	

	if ((getWallDir(NO_TURN) && getWallDir(LEFT) && getWallDir(RIGHT)))
		//|| (loopCounter > 2))
	{
		move->stopForward();
		move->turn_encoder(LEFT);
		stopAlign();
		move->turn_encoder(LEFT);
		//move->turn_encoder(BACK);
		move->resetEncoder();

		/*
		delay(300);
		bt.print(curLocation.x);
		bt.print(" ");
		bt.println(curLocation.y);

		bt.println(printFloodFill());
		bt.println(printMap());
		delay(300);
		*/

		curDirection = opposite(curDirection);
		floodFill(Coordinate(0, 0));
		mapFlag = false;
		loopCounter = 0;
		return 1;
	}
	else if (loopCounter > 2)
	{
		move->stopForward();
		if (getWallDir(LEFT))
		{
			move->turn_encoder(LEFT);
			stopAlign();
			move->turn_encoder(LEFT);
		}
		else if (getWallDir(RIGHT))
		{
			move->turn_encoder(RIGHT);
			stopAlign();
			move->turn_encoder(RIGHT);
		}
		else
		{
			move->turn_encoder(BACK);
		}
		//move->turn_encoder(BACK);
		move->resetEncoder();

		/*
		delay(300);
		bt.print(curLocation.x);
		bt.print(" ");
		bt.println(curLocation.y);

		bt.println(printFloodFill());
		bt.println(printMap());
		delay(300);
		*/

		curDirection = opposite(curDirection);
		floodFill(Coordinate(0, 0));
		mapFlag = false;
		loopCounter = 0;
		return 1;
	}
	
	//floodFill(Coordinate(0, 0));
	//bt.println(printFloodFill());
	//bt.println(loopCounter);
	++loopCounter;
	return -1;
}


void MazeClass::stopAlign()
{
	int tolerance = 700;
	elapsedMillis time = 0;
	while (time < 300)
	{
		led->measure(10);
		int leftError = led->left_rear - led->frontThresholdL; // placeholder value for left rear led threshold
		int rightError = led->right_rear - led->frontThresholdR; // placeholder for right rear led threshold

		if (leftError > tolerance) // too close from the wall
		{
			analogWrite(LEFT_BACKWARD, 150);
			analogWrite(LEFT_FORWARD, 0);
		}

		else if (leftError < -tolerance) // too far
		{
			analogWrite(LEFT_FORWARD, 120);
			analogWrite(LEFT_BACKWARD, 0);
		}

		else
		{
			analogWrite(LEFT_FORWARD, 0);
			analogWrite(LEFT_BACKWARD, 0);
		}

		if (rightError > tolerance) // too close from the wall
		{
			analogWrite(RIGHT_BACKWARD, 150);
			analogWrite(RIGHT_FORWARD, 0);
		}

		else if (rightError < -tolerance) // too far
		{
			analogWrite(RIGHT_FORWARD, 120);
			analogWrite(RIGHT_BACKWARD, 0);
		}

		else
		{
			analogWrite(RIGHT_FORWARD, 0);
			analogWrite(RIGHT_BACKWARD, 0);
		}
		delay(1);
	}
	move->goForward(0, 0);
	/*
	bt.println(printMap());
	bt.println(printFloodFill());
	bt.print(curLocation.x);
	bt.print(" ");
	bt.println(curLocation.y);
	*/
	delay(100);
	//move->resetEncoder();
}