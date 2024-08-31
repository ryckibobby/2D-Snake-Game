// SnakeGame2D.cpp 

#include <iostream>
#include <conio.h>
#include <windows.h>



//define game area size
const int width = 20;
const int height = 17;

//variable for snake and food position
int x, y, fruitX, fruitY, score;

//track snake's tail
int tailX[100], tailY[100];
int nTail;


//direction control
enum eDirection {STOP = 0, LEFT, RIGHT, UP, DOWN};
eDirection dir;

//game over 
bool gameOver;

void Setup() {
	gameOver = false; 
	dir = STOP;
	x = width / 2;
	y = height / 2;
	fruitX = rand() % width;
	fruitY = rand() % height;
	score = 0;
}

void Draw() {
	system("cls");

	for (int i = 0; i < width + 2; i++)
		std::cout << "#";
	std::cout << std::endl;

	//draw game area
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (j == 0)
				std::cout << "#"; //left wall
			if (i == y && j == x)
				std::cout << "0"; //snake head
			else if (i == fruitY && j == fruitX)
				std::cout << "F";
			else {
				bool print = false;
				for (int k = 0; k < nTail; k++) {
					if (tailX[k] == j && tailY[k] == i) {
						std::cout << "o"; //snake tail
						print = true;
					}
				}
				if (!print)
					std::cout << " ";
			}
			if (j == width - 1)
				std::cout << "#";
		}
		std::cout << std::endl;
	}

	//draw bottom wall
	for (int i = 0; i < width + 2; i++)
		std::cout << "#";
	std::cout << std::endl;

	//display score
	std::cout << "Score: " << score << std::endl;
}

//user input
void Input() {
	if (_kbhit()) {
		switch (_getch()) {
		case 'a' :
			dir = LEFT;
			break;
		case 'd' :
			dir = RIGHT;
			break;
		case 'w' :
			dir = UP;
			break;
		case 's' :
			dir = DOWN;
			break;
		case 'x' :
			gameOver = true;
			break;
		}
	}
}

void Logic() {
	//track previous positions
	int prevX = tailX[0];
	int prevY = tailY[0];
	int prev2X, prev2Y;
	tailX[0] = x;
	tailY[0] = y;
	for (int i = 1; i < nTail; i++) {
		prev2X = tailX[i];
		prev2Y = tailY[i];
		tailX[i] = prevX;
		tailY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}

	//update snake's head position based on the direction
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
	
	//check if snake hits the wall
	if (x >= width) x = 0; else if (x < 0) x = width - 1;
	if (y >= height) y = 0; else if (y < 0) y = height - 1;

	//check if snake hits iself
	for (int i = 0; i < nTail; i++) {
		if (tailX[i] == x && tailY[i] == y) {
			gameOver = true;
		}
	}

	//check if snake eats the fruit
	if (x == fruitX && y == fruitY) {
		score += 10;
		fruitX = rand() % width;
		fruitY = rand() % height;
		nTail++;
	}
}


int main()
{
	Setup();
	while (!gameOver) {
		Draw();
		Input();
		Logic();
		Sleep(100);
	}
	return 0;
}

