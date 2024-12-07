#include <iostream>
#include <conio.h>
#include <windows.h>
#include <chrono>
#include <thread>
#include <string>

using namespace std;

bool gameOver;
const int width = 40; // ширина iгрового поля
const int height = 40; //высота iгрового поля
int x, y, fruitx, fruity, score; //координати змiйки та фрукта та очки гри
enum edirection { STOP = 0, LEFT, RIGHT, UP, DOWN }; // напрям руху
enum GameMode { CONTINUOUS, STEP_BY_STEP }; // iгровi режими
GameMode currentMode; // текущiй режим
edirection dir; // текущий напрямок

HANDLE console; //дескриптор консоллi
CHAR_INFO consoleBuffer[height][width]; //буфер консолi

GameMode show_menu() {
    int selected = 0; // обранний пункт менню
    while (true) {
        system("cls"); // очистка екрана
        cout << "=== Snake Game ===" << endl; // назва гри
        cout << "Select game mode:" << endl; // назва обрати режим гри
        cout << (selected == 0 ? "> " : "  ") << "Continuous Mode" << endl; // обрати перший режим
        cout << (selected == 1 ? "> " : "  ") << "Step-by-Step Mode" << endl; // обрати другий режим
        cout << "\nUse W/S to move, Enter to select" << endl; // назва як рухатися у меню

        char key = _getch(); 
        // обробка навігації
        if (key == 'w' && selected > 0) selected--; 
        if (key == 's' && selected < 1) selected++; 
        if (key == 13) return static_cast<GameMode>(selected);
    }
}

std::pair<int, int> generate_random_position(int width, int height, int x = -1, int y = -1) {
    int newX, newY;
    do { // генерацiя випадкових координат
        newX = rand() % (width - 2) + 1;
        newY = rand() % (height - 2) + 1;
    } while (newX == x && newY == y); // перевiрка на збіг з існуючими координатами
    return std::make_pair(newX, newY);
}

void setup()
{
    srand(time(0)); // генератор випадкових чисел 
    gameOver = false; 
    dir = STOP; // начальне направлення
    //генерація початкової позиції змійки та фрукта
    auto snakePos = generate_random_position(width, height);
    x = snakePos.first;
    y = snakePos.second;

    auto fruitPos = generate_random_position(width, height, x, y);
    fruitx = fruitPos.first;
    fruity = fruitPos.second;

    score = 0; // скидання ігрових очок
    //консльний буфер
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
{   // заповнення буфера консолi
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) //отрисовка карти, змiйки та фрукту
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

    string modeStr = (currentMode == CONTINUOUS) ? "Continuous" : "Step-by-Step"; // відображення ігрової панелі
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
    if (_kbhit()) { // рух гри
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
    // оновлення позиції змійки 
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
    // перевірка меж поля
    if (x >= width) x = 0;
    else if (x < 0) x = width - 1;
    if (y >= height) y = 0;
    else if (y < 0) y = height - 1;
    // перевірка зіткнення із фруктом, якщо так, то +10 score
    if (x == fruitx && y == fruity) {
        score += 10;
        auto newFruitPos = generate_random_position(width, height, x, y);
        fruitx = newFruitPos.first;
        fruity = newFruitPos.second;
    }
}

int main()
{
    while (true) { // основний ігровий цикл
        currentMode = show_menu(); // показ меню
        setup(); // наллаштунок гри
        while (!gameOver) { // цикл гри
            draw(); // відмальовка гри
            input(); // обробка введення
            if (currentMode == CONTINUOUS || (currentMode == STEP_BY_STEP && dir != STOP)) { // обробка логіки залежно від режиму
                logic();
                if (currentMode == STEP_BY_STEP) {
                    dir = STOP;
                }
            }
            this_thread::sleep_for(chrono::milliseconds(50)); // задержка
        }
        cout << "Game Over! Your score: " << score << endl; // ще не робе треба доробити
        cout << "Press any key to return to menu..." << endl;
        _getch();
    }
    return 0;
}