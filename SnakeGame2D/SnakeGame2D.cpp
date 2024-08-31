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

int main()
{

}

