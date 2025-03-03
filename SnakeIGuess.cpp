// SnakeIGuess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <vector>
#include <ctime>

int fieldWidth = 30;
int fieldHeight = 15;
unsigned char* field = nullptr;

int screenWidth = 50;
int screenHeight = 25;

int tickCount = 0;
int tickLimit = 3;

char buffer[10];

struct Point
{
	int x, y;

	Point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	Point(const Point& p)
	{
		x = p.x;
		y = p.y;
	}

	void operator=(const Point& p)
	{
		x = p.x;
		y = p.y;
	}

	bool operator==(const Point& p)
	{
		return x == p.x && y == p.y;
	}

	bool operator!=(const Point& p)
	{
		return x != p.x || y != p.y;
	}
};

std::vector<Point> snake;

Point apple(0, 0);
std::vector<Point> eatenApples;

std::vector<unsigned char> inputBuffer(3);

bool isInSnake(Point& p)
{
	for (auto& section : snake)
		if (p == section) return true;
	return false;
}

bool snakeEatsSelf()
{
	for (auto it = snake.begin() + 1; it != snake.end(); it++)
		if (snake.front() == *it) return true;
	return false;
}

void updateSnakePositions(unsigned char input)
{
	if (snake.front() == apple)
	{
		eatenApples.insert(eatenApples.begin(), Point(apple));

		while (isInSnake(apple))
			apple = Point(rand() % (fieldWidth - 2) + 1, rand() % (fieldHeight - 2) + 1);
	}

	if (!eatenApples.empty() && snake.back() == eatenApples.back())
	{
		snake.push_back(eatenApples.back());
		eatenApples.pop_back();

		for (int i = snake.size() - 2; i > 0; i--)
			snake[i] = snake[i - 1];
	}
	else
	{
		for (int i = snake.size() - 1; i > 0; i--)
			snake[i] = snake[i - 1];
	}

	switch (input)
	{
	case VK_UP:
		snake.front().y--;
		break;
	case VK_DOWN:
		snake.front().y++;
		break;
	case VK_LEFT:
		snake.front().x--;
		break;
	case VK_RIGHT:
		snake.front().x++;
		break;
	default:
		break;
	}

	if (inputBuffer.size() > 1)
		inputBuffer.erase(inputBuffer.begin());
}

void addToInputBuffer(unsigned char input)
{
	if (inputBuffer.size() < 3)
	{
		switch (input)
		{
		case VK_UP:
			if (inputBuffer.back() != VK_DOWN)
				inputBuffer.push_back(input);
			break;
		case VK_DOWN:
			if (inputBuffer.back() != VK_UP)
				inputBuffer.push_back(input);
			break;
		case VK_LEFT:
			if (inputBuffer.back() != VK_RIGHT)
				inputBuffer.push_back(input);
			break;
		case VK_RIGHT:
			if (inputBuffer.back() != VK_LEFT)
				inputBuffer.push_back(input);
			break;
		default:
			break;
		}
	}
}

int main()
{
	// Allocate game field
	field = new unsigned char[fieldWidth * fieldHeight];
	for (int x = 0; x < fieldWidth; x++)
		for (int y = 0; y < fieldHeight; y++)
			field[y * fieldWidth + x] = (x == 0 || x == fieldWidth - 1 || y == 0 || y == fieldHeight - 1) ? 3 : 0;

	// Create snake
	snake.push_back(Point(fieldWidth / 2, fieldHeight / 2));

	// Allocate screen
	char* screen = new char[screenWidth * screenHeight];
	for (int i = 0; i < screenWidth * screenHeight; i++) screen[i] = ' ';

	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SMALL_RECT windowSize = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(console, TRUE, &windowSize);
	SetConsoleScreenBufferSize(console, { (short)screenWidth, (short)screenHeight });
	windowSize = { 0, 0, (short)(screenWidth - 1), (short)(screenHeight - 1) };
	SetConsoleWindowInfo(console, TRUE, &windowSize);
	CONSOLE_CURSOR_INFO cursorInfo = { 1, false };
	SetConsoleCursorInfo(console, &cursorInfo);
	SetConsoleActiveScreenBuffer(console);
	DWORD bytesWritten = 0;

	for (int i = 0; i < 9; i++) screen[2 * screenWidth + (fieldWidth + 5 + i)] = "CONTROLS:"[i];
	for (int i = 0; i < 10; i++) screen[3 * screenWidth + (fieldWidth + 5 + i)] = "ARROW KEYS"[i];

	// Seed rng
	srand((int)time(nullptr));

	// Draw initial field
	for (int x = 0; x < fieldWidth; x++)
		for (int y = 0; y < fieldHeight; y++)
			screen[(y + 2) * screenWidth + (x + 4)] = ".0*#@"[field[y * fieldWidth + x]];

	WriteConsoleOutputCharacterA(console, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);

	//Initial input
	while (true)
	{
		if (GetAsyncKeyState(VK_UP) != 0 || true)
		{
			addToInputBuffer(VK_UP);
			break;
		}
		if (GetAsyncKeyState(VK_DOWN) != 0)
		{
			addToInputBuffer(VK_DOWN);
			break;
		}
		if (GetAsyncKeyState(VK_LEFT) != 0)
		{
			addToInputBuffer(VK_LEFT);
			break;
		}
		if (GetAsyncKeyState(VK_RIGHT) != 0)
		{
			addToInputBuffer(VK_RIGHT);
			break;
		}
	}

	//Initial apple position
	apple = Point(rand() % (fieldWidth - 2) + 1, rand() % (fieldHeight - 2) + 1);

	bool done = false;

	while (!done)
	{
		// Game timing
		Sleep(50);
		if (++tickCount == tickLimit)
		{
			tickCount %= tickLimit;

			updateSnakePositions(inputBuffer[0]);
		}

		// Input
		if (GetAsyncKeyState(VK_UP) != 0) addToInputBuffer(VK_UP);
		if (GetAsyncKeyState(VK_DOWN) != 0) addToInputBuffer(VK_DOWN);
		if (GetAsyncKeyState(VK_LEFT) != 0) addToInputBuffer(VK_LEFT);
		if (GetAsyncKeyState(VK_RIGHT) != 0) addToInputBuffer(VK_RIGHT);
		if (GetAsyncKeyState(VK_RETURN) != 0) done = true;


		// Game logic

		// Kill condition
		if (snake.front().x == 0 || snake.front().x == fieldWidth - 1 || snake.front().y == 0 || snake.front().y == fieldHeight - 1 ||
			snakeEatsSelf())
			done = true;


		// Render output
		field[apple.y * fieldWidth + apple.x] = 2;

		for (auto& section : snake)
			field[section.y * fieldWidth + section.x] = 1;

		for (auto& eatenApple : eatenApples)
			field[eatenApple.y * fieldWidth + eatenApple.x] = 4;


		// Draw field to screen
		for (int x = 0; x < fieldWidth; x++)
			for (int y = 0; y < fieldHeight; y++)
				screen[(y + 2) * screenWidth + (x + 4)] = ".0*#@"[field[y * fieldWidth + x]];

		field[apple.y * fieldWidth + apple.x] = 0;

		for (auto& section : snake)
			field[section.y * fieldWidth + section.x] = 0;

		WriteConsoleOutputCharacterA(console, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);
	}

	// Game over screen
	for (int i = 0; i < 9; i++) screen[6 * screenWidth + (fieldWidth + 5 + i)] = "GAME OVER"[i];
	for (int i = 0; i < 9; i++) screen[7 * screenWidth + (fieldWidth + 5 + i)] = ' ';
	for (int i = 0; i < 11; i++) screen[8 * screenWidth + (fieldWidth + 5 + i)] = "PRESS ENTER"[i];
	for (int i = 0; i < 7; i++) screen[9 * screenWidth + (fieldWidth + 5 + i)] = "TO EXIT"[i];
	WriteConsoleOutputCharacterA(console, screen, screenWidth * screenHeight, { 0, 0 }, &bytesWritten);

	while (GetAsyncKeyState(VK_RETURN) == 0) {};

	return 0;
}