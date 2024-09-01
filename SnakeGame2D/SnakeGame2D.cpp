// SnakeGame2D.cpp 

#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream>



//define game area size
const int width = 20;
const int height = 17;

//achievements variables
bool scoreHunterUnlocked = false;
bool survivorUnlocked = false;
bool fruitCollectorUnlocked = false;

bool speedDemonUnlocked = false;
bool perfectionistUnlocked = false;
bool explorerUnlocked = false;
bool longevityUnlocked = false;
bool highRollerUnlocked = false;

int consecutiveFruits = 0;  //tracks the number of consecutive fruits collected
int totalTilesCovered = 0;  //tracks the number of unique tiles covered
bool tilesCovered[20][20];  //array to track which tiles have been covered

int survivalTicks = 0;
int fruitsCollected = 0; 

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

	//reset achievement tracking variables
	survivalTicks = 0;
	fruitsCollected = 0;
	consecutiveFruits = 0;
	totalTilesCovered = 0;
	memset(tilesCovered, false, sizeof(tilesCovered));
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

	if (scoreHunterUnlocked)
		std::cout << "Achievement: Score Hunter" << std::endl;
	if (survivorUnlocked)
		std::cout << "Achievement: Survivor" << std::endl;
	if (fruitCollectorUnlocked)
		std::cout << "Achievement: Fruit Collector" << std::endl;
	if (speedDemonUnlocked)
		std::cout << "Achievement: Speed Demon" << std::endl;
	if (perfectionistUnlocked)
		std::cout << "Achievement: Perfectionist" << std::endl;
	if (explorerUnlocked)
		std::cout << "Achievement: Explorer" << std::endl;
	if (longevityUnlocked)
		std::cout << "Achievement: Longevity" << std::endl;
	if (highRollerUnlocked)
		std::cout << "Achievement: High Roller" << std::endl;
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
			score += 30;  // increase score by 30 for special fruit
			specialFruitActive = false;  // despawn after eating
			if (score > highScore) {
				highScore = score;
				SaveHighScore();
			}
		}
	}
	survivalTicks++;  // increment survival time

	//track tile coverage for explorer achievement
	if (!tilesCovered[x][y]) {
		tilesCovered[x][y] = true;
		totalTilesCovered++;
	}

	//achievement: Score Hunter
	if (!scoreHunterUnlocked && score >= 100) {
		scoreHunterUnlocked = true;
		std::cout << "Achievement Unlocked: Score Hunter (Reach 100 points)" << std::endl;
	}

	//achievement: Survivor
	if (!survivorUnlocked && survivalTicks >= 200) {
		survivorUnlocked = true;
		std::cout << "Achievement Unlocked: Survivor (Survive for 200 ticks)" << std::endl;
	}

	//achievement: High Roller
	if (!highRollerUnlocked && score >= 200) {
		highRollerUnlocked = true;
		std::cout << "Achievement Unlocked: High Roller (Reach 200 points)" << std::endl;
	}

	//achievement: Longevity
	if (!longevityUnlocked && survivalTicks >= 500) {
		longevityUnlocked = true;
		std::cout << "Achievement Unlocked: Longevity (Survive for 500 ticks)" << std::endl;
	}

	//achievement: Explorer
	if (!explorerUnlocked && totalTilesCovered >= width * height) {
		explorerUnlocked = true;
		std::cout << "Achievement Unlocked: Explorer (Cover every tile on the board)" << std::endl;
	}

	if (x == fruitX && y == fruitY) {
		score += 10;
		fruitX = rand() % width;
		fruitY = rand() % height;
		nTail++;
		fruitsCollected++;  // increment the number of fruits collected

		//achievement: Fruit Collector
		if (!fruitCollectorUnlocked && fruitsCollected >= 10) {
			fruitCollectorUnlocked = true;
			std::cout << "Achievement Unlocked: Fruit Collector (Collect 10 fruits without dying)" << std::endl;
		}

		//achievement: Perfectionist
		if (!perfectionistUnlocked && consecutiveFruits >= 5) {
			perfectionistUnlocked = true;
			std::cout << "Achievement Unlocked: Perfectionist (Collect 5 consecutive fruits without missing)" << std::endl;
		}

		//achievement: Speed Demon
		if (!speedDemonUnlocked && nTail >= 15 && survivalTicks >= 100) {
			speedDemonUnlocked = true;
			std::cout << "Achievement Unlocked: Speed Demon (Survive for 100 ticks with a length of 15 or more)" << std::endl;
		}

		if (score > highScore) {
			highScore = score;
			SaveHighScore();
		}
	}
	else {
		consecutiveFruits = 0;  //reset the consecutive fruit counter if a fruit is missed
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

