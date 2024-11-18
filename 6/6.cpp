#include <iostream>
#include <conio.h>
#include <windows.h>
#include <chrono>
#include <thread>
#include <string>

using namespace std;

bool gameOver;
const int width = 40;
const int height = 40;
int x, y, fruitx, fruity, score;
enum edirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum GameMode { CONTINUOUS, STEP_BY_STEP };
GameMode currentMode;
edirection dir;

HANDLE console;
CHAR_INFO consoleBuffer[height][width];

GameMode show_menu() {
    int selected = 0;
    while (true) {
        system("cls");
        cout << "=== Snake Game ===" << endl;
        cout << "Select game mode:" << endl;
        cout << (selected == 0 ? "> " : "  ") << "Continuous Mode" << endl;
        cout << (selected == 1 ? "> " : "  ") << "Step-by-Step Mode" << endl;
        cout << "\nUse W/S to move, Enter to select" << endl;

        char key = _getch();
        if (key == 'w' && selected > 0) selected--;
        if (key == 's' && selected < 1) selected++;
        if (key == 13) return static_cast<GameMode>(selected);
    }
}

std::pair<int, int> generate_random_position(int width, int height, int x = -1, int y = -1) {
    int newX, newY;
    do {
        newX = rand() % (width - 2) + 1;
        newY = rand() % (height - 2) + 1;
    } while (newX == x && newY == y);
    return std::make_pair(newX, newY);
}

void setup()
{
    srand(time(0));
    gameOver = false;
    dir = STOP;

    auto snakePos = generate_random_position(width, height);
    x = snakePos.first;
    y = snakePos.second;

    auto fruitPos = generate_random_position(width, height, x, y);
    fruitx = fruitPos.first;
    fruity = fruitPos.second;

    score = 0;

    console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if (console == INVALID_HANDLE_VALUE) {
        cerr << "CreateConsoleScreenBuffer failed - (" << GetLastError() << ")" << endl;
        exit(1);
    }
    if (!SetConsoleActiveScreenBuffer(console)) {
        cerr << "SetConsoleActiveScreenBuffer failed - (" << GetLastError() << ")" << endl;
        exit(1);
    }
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

    string modeStr = (currentMode == CONTINUOUS) ? "Continuous" : "Step-by-Step";
    string infoStr = "Mode: " + modeStr + " | Score: " + to_string(score);
    for (int i = 0; i < infoStr.length(); i++) {
        consoleBuffer[0][i + 1].Char.AsciiChar = infoStr[i];
        consoleBuffer[0][i + 1].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
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
        auto newFruitPos = generate_random_position(width, height, x, y);
        fruitx = newFruitPos.first;
        fruity = newFruitPos.second;
    }
}

int main()
{
    while (true) {
        currentMode = show_menu();
        setup();
        while (!gameOver) {
            draw();
            input();
            if (currentMode == CONTINUOUS || (currentMode == STEP_BY_STEP && dir != STOP)) {
                logic();
                if (currentMode == STEP_BY_STEP) {
                    dir = STOP;
                }
            }
            this_thread::sleep_for(chrono::milliseconds(50));
        }
        cout << "Game Over! Your score: " << score << endl;
        cout << "Press any key to return to menu..." << endl;
        _getch();
    }
    return 0;
}