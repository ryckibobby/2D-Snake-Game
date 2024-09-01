// SnakeGame2D.cpp 

#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream>



//define game area size
const int width = 20;
const int height = 17;

//variable for snake and food position
int x, y, fruitX, fruitY, score;

//special fruit variables
int specialFruitX, specialFruitY;
int specialFruitTimer = 0;
bool specialFruitActive = false;
const int specialFruitDuration = 50;

//track snake's tail
int tailX[100], tailY[100];
int nTail;


//direction control
enum eDirection {STOP = 0, LEFT, RIGHT, UP, DOWN};
eDirection dir;

//to store high score
int highScore = 0;

//game over 
bool gameOver;

void LoadHighScore() {
	std::ifstream file("highscore.txt");
	if (file.is_open()) {
		file >> highScore;
	}
	file.close();
}

void SaveHighScore() {
	std::ofstream file("highscore.txt");
	if (file.is_open()) {
		file << highScore;
	}
	file.close();
}

void Setup() {
	gameOver = false; 
	dir = STOP;
	x = width / 2;
	y = height / 2;
	fruitX = rand() % width;
	fruitY = rand() % height;
	score = 0;

	specialFruitTimer = 0;
	specialFruitActive = false;

	LoadHighScore();
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
			else if (specialFruitActive && i == specialFruitY && j == specialFruitX)
				std::cout << "S";  // special fruit 
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
	std::cout << "High Score: " << highScore << std::endl;
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

		if (score > highScore) {
			highScore = score;
			SaveHighScore();
		}
	}
	if (!specialFruitActive && rand() % 100 < 5) {  // 5% chance to spawn special fruit each cycle
		specialFruitX = rand() % width;
		specialFruitY = rand() % height;
		specialFruitActive = true;
		specialFruitTimer = specialFruitDuration;
	}

	if (specialFruitActive) {
		specialFruitTimer--;  // decrease the timer
		if (specialFruitTimer <= 0) {
			specialFruitActive = false;  // despawn the special fruit
		}

		if (x == specialFruitX && y == specialFruitY) {
			score += 50;  // increase score by 50 for special fruit
			specialFruitActive = false;  // despawn after eating
			if (score > highScore) {
				highScore = score;
				SaveHighScore();
			}
		}
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

