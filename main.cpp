#include <iostream>
#include <cmath>
using namespace std;

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

int main()
{
	mazeCoordinate center1(centerx, centery);
	mazeCoordinate center2(centerx2, centery2);
	mazeCoordinate center3(centerx3, centery2);
	mazeCoordinate center4(centerx4, centery4);
	cout << "Hello\n";
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
			temp1[j][i] = abs(center1.x - j);
	for (int i = 0; i < SIZEY; i++)
		for (int j = center2.x; j < SIZEX; j++)
			temp1[j][i] = abs(center2.x - j);

	
	for (int i = 0; i < SIZEX; i++)
		for (int j = 0; j < center1.y; j++)
			temp2[i][j] = abs(center1.y - j);
	for (int i = 0; i < SIZEX; i++)
		for (int j = center4.y; j < SIZEY; j++)
			temp2[i][j] = abs(center4.y - j);

	int finalArray[SIZEX][SIZEY];
	for (int i = 0; i < SIZEY; i++)
		for (int j = 0; j < SIZEX; j++)
			finalArray[j][i] = temp1[j][i] + temp2[j][i];

	for (int i = 0; i < SIZEY; i++)
	{
		for (int j = 0; j < SIZEX; j++)
		{
		cout << finalArray[j][i] << "    ";
		if (finalArray[j][i] < 10)
			cout << " ";
		
		}
		cout << endl;
	}
	
	return 0;
}