#include <iostream>
#include <conio.h>
#include <windows.h>
#include <chrono>
#include <thread>

using namespace std;

bool gameOver;
const int width = 40;
const int height = 40;
int x, y, fruitx, fruity, score;
enum edirection {STOP =0, LEFT,RIGHT,UP,DOWN};
edirection dir;

HANDLE console;
CHAR_INFO consoleBuffer[height][width];


void setup()
{
	gameOver = false;
	dir = STOP;
	x = width / 2 - 1;
	y = height / 2 - 1;
	fruitx = rand() % width;
	fruitx = rand() % height;
	score = 0;

	console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
}
void draw()
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
				consoleBuffer[i][j].Char.AsciiChar = '#';
			else if (i == y && j == x)
				consoleBuffer[i][j].Char.AsciiChar = '0';
			else if (i == fruity && j == fruitx)
				consoleBuffer[i][j].Char.AsciiChar = 'F';
			else
				consoleBuffer[i][j].Char.AsciiChar = ' ';

			consoleBuffer[i][j].Attributes = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED;
		}
	}

	SMALL_RECT writeArea = { 0, 0, width - 1, height - 1 };
	WriteConsoleOutput(console, (CHAR_INFO*)consoleBuffer, { width, height }, { 0, 0 }, &writeArea);
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
void logic() {
	int prevX = x;
	int prevY = y;

	switch (dir) {
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
	default:
		break;
	}

	if (x >= width) x = 0;
	else if (x < 0) x = width - 1;
	if (y >= height) y = 0;
	else if (y < 0) y = height - 1;

	if (x == fruitx && y == fruity) {
		score += 10;
		fruitx = rand() % width;
		fruity = rand() % height;
	}
}
int main()
{
	setup();
	while (!gameOver) {
		draw();
		input();
		logic();
		this_thread::sleep_for(chrono::milliseconds(50));
	}

	return 0;
}
