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
int x, y, aiX, aiY;
int fruitX, fruitY, score, aiScore;

//special fruit variables
int specialFruitX, specialFruitY;
int specialFruitTimer = 0;
bool specialFruitActive = false;
const int specialFruitDuration = 50;

//track snake and ai snake's tail
int tailX[100], tailY[100], aiTailX[100], aiTailY[100];
int nTail, aiNTail;
int aiSpeed = 150;


//direction control
enum eDirection {STOP = 0, LEFT, RIGHT, UP, DOWN};
eDirection dir, aiDir;

//to store high score
int highScore = 0;

//game over 
bool gameOver;

bool isPaused = false;  // tracks if the game is currently paused


int gameSpeed = 100; // default game speed

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

void SelectAIDifficulty() {
	int difficulty;
	std::cout << "Select AI Difficulty: " << std::endl;
	std::cout << "1. Easy" << std::endl;
	std::cout << "2. Medium" << std::endl;
	std::cout << "3. Hard" << std::endl;
	std::cout << "Enter your choice: ";
	std::cin >> difficulty;

	switch (difficulty) {
	case 1:
		aiSpeed = 150;  //slower AI speed
		break;
	case 2:
		aiSpeed = 100;  //default AI speed
		break;
	case 3:
		aiSpeed = 50;   //faster AI speed
		break;
	default:
		std::cout << "Invalid choice! Setting to Medium by default." << std::endl;
		aiSpeed = 100;
		break;
	}
	std::cout << "AI Difficulty set!" << std::endl;
	std::cout << "\nPress any key to return to the menu...";
	std::cin.ignore();
	std::cin.get();
}

void ChangeDifficulty() {
	int difficulty;
	std::cout << "Select Difficulty: " << std::endl;
	std::cout << "1. Easy" << std::endl;
	std::cout << "2. Medium" << std::endl;
	std::cout << "3. Hard" << std::endl;
	std::cout << "Enter your choice: ";
	std::cin >> difficulty;

	switch (difficulty) {
	case 1:
		gameSpeed = 150;  // Slower speed
		break;
	case 2:
		gameSpeed = 100;  // Default speed
		break;
	case 3:
		gameSpeed = 50;   // Faster speed
		break;
	default:
		std::cout << "Invalid choice! Setting to Medium by default." << std::endl;
		gameSpeed = 100;
		break;
	}
	std::cout << "Difficulty set!" << std::endl;
	std::cout << "\nPress any key to return to the menu...";
	std::cin.ignore();
	std::cin.get();
}

void ShowHighScores() {
	system("cls");
	std::cout << "=== High Scores ===" << std::endl;
	std::cout << "1. " << highScore << std::endl;
	// add more high scores if needed
	std::cout << "\nPress any key to return to the menu...";
	std::cin.ignore();
	std::cin.get();
}

void ShowSettings() {
	int settingChoice;
	do {
		system("cls");
		std::cout << "=== Settings ===" << std::endl;
		std::cout << "1. Change Difficulty" << std::endl;
		std::cout << "2. Back to Main Menu" << std::endl;
		std::cout << "Enter your choice: ";
		std::cin >> settingChoice;

		switch (settingChoice) {
		case 1:
			// change difficulty (e.g., adjust speed)
			ChangeDifficulty();
			break;
		case 2:
			// return to main menu
			return;
		default:
			std::cout << "Invalid choice! Please select again." << std::endl;
			break;
		}
	} while (settingChoice != 2);
}

void ShowMenu() {
	int choice;
	do {
		system("cls");
		std::cout << "=== Snake Game ===" << std::endl;
		std::cout << "1. Start Game" << std::endl;
		std::cout << "2. View High Scores" << std::endl;
		std::cout << "3. Settings" << std::endl;
		std::cout << "4. Exit" << std::endl;
		std::cout << "Enter your choice: ";
		std::cin >> choice;

		switch (choice) {
		case 1:
			//start the game
			return;
		case 2:
			//view high scores
			ShowHighScores();
			break;
		case 3:
			//adjust settings
			ShowSettings();
			break;
		case 4:
			//exit the game
			exit(0);
		default:
			std::cout << "Invalid choice! Please select again." << std::endl;
			break;
		}
	} while (choice != 1);
}



void Setup() {
	gameOver = false; 
	dir = STOP;
	aiDir = STOP;
	x = width / 2;
	y = height / 2;
	aiX = width / 3;
	aiY = height / 3; 
	fruitX = rand() % width;
	fruitY = rand() % height;
	score = 0;
	aiScore = 0;

	nTail = 0;
	aiNTail = 0;

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

// AI movement logic
void AILogic() {
	if (aiX < fruitX) {
		aiDir = RIGHT;
	}
	else if (aiX > fruitX) {
		aiDir = LEFT;
	}
	else if (aiY < fruitY) {
		aiDir = DOWN;
	}
	else if (aiY > fruitY) {
		aiDir = UP;
	}

	//move AI snake
	int prevX = aiTailX[0];
	int prevY = aiTailY[0];
	int prev2X, prev2Y;
	aiTailX[0] = aiX;
	aiTailY[0] = aiY;
	for (int i = 1; i < aiNTail; i++) {
		prev2X = aiTailX[i];
		prev2Y = aiTailY[i];
		aiTailX[i] = prevX;
		aiTailY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}

	switch (aiDir) {
	case LEFT:
		aiX--;
		break;
	case RIGHT:
		aiX++;
		break;
	case UP:
		aiY--;
		break;
	case DOWN:
		aiY++;
		break;
	default:
		break;
	}

	//check if AI hits the wall
	if (aiX >= width) aiX = 0; else if (aiX < 0) aiX = width - 1;
	if (aiY >= height) aiY = 0; else if (aiY < 0) aiY = height - 1;

	//check if AI hits itself
	for (int i = 0; i < aiNTail; i++) {
		if (aiTailX[i] == aiX && aiTailY[i] == aiY) {
			gameOver = true;
		}
	}

	//check if AI eats the fruit
	if (aiX == fruitX && aiY == fruitY) {
		aiScore += 10;
		fruitX = rand() % width;
		fruitY = rand() % height;
		aiNTail++;
	}
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
			else if (i == aiY && j == aiX)
				std::cout << "A"; //AI snake head
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
				for (int k = 0; k < aiNTail; k++) {
					if (aiTailX[k] == j && aiTailY[k] == i) {
						std::cout << "a"; //AI snake tail
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
	std::cout << "AI Score: " << aiScore << std::endl;
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
		case 'p':
			isPaused = !isPaused;  // toggle pause state
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

	AILogic();


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
	SelectAIDifficulty();
	while (true) {
		ShowMenu();  // display the menu and handle selection

		Setup();
		while (!gameOver) {
			if (!isPaused) {
				Draw();
				Input();
				Logic();
				AILogic();
				Sleep(gameSpeed);  // Adjust speed based on difficulty
			}
			else {
				std::cout << "Game Paused. Press 'P' to resume..." << std::endl;
				Input();  // still listen for the 'P' key to unpause
				Sleep(100);  // reduce CPU usage while paused
			}
			Sleep(gameSpeed);  // adjust speed based on difficulty
		}
		std::cout << "Game Over! Press any key to return to the main menu...";
		std::cin.ignore();
		std::cin.get();
	}
	return 0;
}

