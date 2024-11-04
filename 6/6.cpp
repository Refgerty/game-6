#include <iostream>
#include <conio.h>

using namespace std;

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruitx, fruity, score;
enum edirection {STOP =0, LEFT,RIGHT,UP,DOWN};
edirection dir;


void setup()
{
	gameOver = false;
	dir = STOP;
	x = width / 2 - 1;
	y = height / 2 - 1;
	fruitx = rand() % width;
	fruitx = rand() % height;
	score = 0;
}
void draw()
{
	system("cls");
	for (int i = 0; i < width + 1; i++)
		cout << "#";
	cout << endl;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (j == 0 || j == width - 1)
				cout << "#";
			if (i == y && j == x)
				cout << "0";
			else if (i == fruity && j == fruitx)
				cout << "F";
			else
			cout << " ";
		}
		cout << endl;
	}
		

	for (int i = 0; i < width + 1; i++)
		cout << "#";
	cout << endl;
	
}
void input()
{
	if (_kbhit()) {
		switch (_getch())
		{
		case 'a':
			dir = LEFT;
			break;
		case 'd':
			dir = RIGHT;
			break;
		case 'w':
			dir = UP;
			break;
		case 's':
			dir = DOWN;
			break;
		case 'x':
			gameOver = true;
			break;
		}
	}
}
void logic(){
	switch(dir){
	case LEFT:
		x--;
		break;
	case RIGHT:
		x++;
		break;
	case UP:
		y--;
		break;
	case DOWN:
		y++;
		break;
}

}
int main()
{
	setup();
	while (!gameOver) {
		draw();
		input();
		logic();
	}

	return 0;
}
