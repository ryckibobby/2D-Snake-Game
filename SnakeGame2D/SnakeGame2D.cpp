#include <iostream>
#include <fstream>
#include <cstdlib>
#include <conio.h>
#include <windows.h>

// define constants
const int width = 20;
const int height = 17;
const int numObstacles = 5;
const int powerUpDuration = 100;
const int specialFruitDuration = 50;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum PowerUp { NONE = 0, MOVE_THROUGH_WALLS, SLOW_TIME, DOUBLE_SCORE };

// global variables
int highScore = 0;
bool gameOver;
bool isPaused = false;
int gameSpeed = 100; // default game speed

// variables for solo play
int x, y, fruitX, fruitY, score;
int nTail;
int tailX[100], tailY[100];
int obstacleX[numObstacles], obstacleY[numObstacles];
bool tilesCovered[width][height] = { false };
int totalTilesCovered;
int survivalTicks;
int fruitsCollected;
int consecutiveFruits;
bool scoreHunterUnlocked = false;
bool survivorUnlocked = false;
bool fruitCollectorUnlocked = false;
bool speedDemonUnlocked = false;
bool perfectionistUnlocked = false;
bool explorerUnlocked = false;
bool longevityUnlocked = false;
bool highRollerUnlocked = false;

// variables for AI play
int aiX, aiY, aiScore, aiNTail;
int aiTailX[100], aiTailY[100];
Direction aiDir;
int aiSpeed;

// power-up variables
bool powerUpActive = false;
int powerUpX, powerUpY;
PowerUp currentPowerUp = NONE;
int powerUpTimer = 0;
PowerUp powerUpType;
bool specialFruitActive = false;
int specialFruitX, specialFruitY;
int specialFruitTimer = 0;

// mode selection
bool playAgainstAI = false;

// direction of the snake
Direction dir;

// function prototypes
void LoadHighScore();
void SaveHighScore();
void SelectAIDifficulty();
void ChangeDifficulty();
void ShowHighScores();
void ShowSettings();
void ShowMenu();
void Setup();
void AILogic();
void Draw();
void Input();
void Logic();

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
        aiSpeed = 150;  // slower AI speed
        break;
    case 2:
        aiSpeed = 100;  // default AI speed
        break;
    case 3:
        aiSpeed = 50;   // faster AI speed
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
        gameSpeed = 150;  // slower speed
        break;
    case 2:
        gameSpeed = 100;  // default speed
        break;
    case 3:
        gameSpeed = 50;   // faster speed
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
            ChangeDifficulty();
            break;
        case 2:
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
        std::cout << "1. Start Solo Game" << std::endl;
        std::cout << "2. Play Against AI" << std::endl;
        std::cout << "3. View High Scores" << std::endl;
        std::cout << "4. Settings" << std::endl;
        std::cout << "5. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            playAgainstAI = false;
            return;
        case 2:
            playAgainstAI = true;
            SelectAIDifficulty();
            return;
        case 3:
            ShowHighScores();
            break;
        case 4:
            ShowSettings();
            break;
        case 5:
            exit(0);
        default:
            std::cout << "Invalid choice! Please select again." << std::endl;
            break;
        }
    } while (choice != 1 && choice != 2);
}

void Setup() {
    std::cout << "Setting up game..." << std::endl;
    gameOver = false;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
    dir = STOP; // initialize direction

    specialFruitTimer = 0;
    specialFruitActive = false;

    LoadHighScore();

    for (int i = 0; i < numObstacles; i++) {
        obstacleX[i] = rand() % width;
        obstacleY[i] = rand() % height;
    }

    survivalTicks = 0;
    fruitsCollected = 0;
    consecutiveFruits = 0;
    totalTilesCovered = 0;
    memset(tilesCovered, false, sizeof(tilesCovered));

    if (playAgainstAI) {
        aiX = width / 3;
        aiY = height / 3;
        aiScore = 0;
        aiNTail = 0;
        aiDir = STOP;
    }
}

void AILogic() {
    // basic AI movement logic towards the fruit
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

    // move the tail
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

    // move the head
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

    // wrap around
    if (aiX >= width) aiX = 0; else if (aiX < 0) aiX = width - 1;
    if (aiY >= height) aiY = 0; else if (aiY < 0) aiY = height - 1;

    // check for collision with AI's own tail
    for (int i = 0; i < aiNTail; i++) {
        if (aiTailX[i] == aiX && aiTailY[i] == aiY) {
            gameOver = true;
            return;
        }
    }

    // check for collision with obstacles
    for (int i = 0; i < numObstacles; i++) {
        if (obstacleX[i] == aiX && obstacleY[i] == aiY) {
            gameOver = true;
            return;
        }
    }

    // check for collision with fruit
    if (aiX == fruitX && aiY == fruitY) {
        aiScore++;
        fruitX = rand() % width;
        fruitY = rand() % height;
        aiNTail++;
    }
}

void Draw() {
    system("cls");
    std::cout << "Score: " << score << " | High Score: " << highScore << std::endl;
    std::cout << "Press 'p' to pause or resume" << std::endl;
    std::cout << "Press 'x' to quit" << std::endl;

    for (int i = 0; i < width + 2; i++) std::cout << "#";
    std::cout << std::endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) std::cout << "#";
            if (i == y && j == x) std::cout << "O";
            else if (i == fruitY && j == fruitX) std::cout << "F";
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        std::cout << "o";
                        print = true;
                        break;
                    }
                }
                if (!print) {
                    for (int k = 0; k < numObstacles; k++) {
                        if (obstacleX[k] == j && obstacleY[k] == i) {
                            std::cout << "#";
                            print = true;
                            break;
                        }
                    }
                }
                if (!print) std::cout << " ";
            }
            if (j == width - 1) std::cout << "#";
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < width + 2; i++) std::cout << "#";
    std::cout << std::endl;
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a':
            if (dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        case 'p':
            isPaused = !isPaused;
            break;
        }
    }
}

void Logic() {
    if (gameOver) {
        std::cout << "Game Over state detected in Logic function." << std::endl;
        return;
    }

    // move the tail
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

    // move the head
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

    // wrap around
    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    // check for collision with tail
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y) {
            std::cout << "Collision with tail!" << std::endl;
            gameOver = true;
            return;
        }
    }

    // check for collision with obstacles
    for (int i = 0; i < numObstacles; i++) {
        if (obstacleX[i] == x && obstacleY[i] == y) {
            std::cout << "Collision with obstacle!" << std::endl;
            gameOver = true;
            return;
        }
    }

    // check for collision with fruit
    if (x == fruitX && y == fruitY) {
        score++;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }

    // handle special fruit
    if (specialFruitActive) {
        specialFruitTimer--;
        if (specialFruitTimer <= 0) {
            specialFruitActive = false;
            std::cout << "Special fruit effect ended!" << std::endl;
        }
    }

    // check if high score is broken
    if (score > highScore) {
        highScore = score;
        SaveHighScore();
        std::cout << "New High Score: " << highScore << std::endl;
    }

    // AI logic
    if (playAgainstAI) {
        AILogic();
    }
}

int main() {
    ShowMenu();
    while (!gameOver) {
        if (!isPaused) {
            Setup();
            while (!gameOver) {
                Draw();
                Input();
                Logic();
                Sleep(gameSpeed);
            }
        }
        else {
            std::cout << "Game paused. Press 'p' to resume." << std::endl;
        }
    }
    return 0;
}