#include <iostream>
#include <fstream>
#include <cstdlib>
#include <conio.h> 
#include <windows.h> 

//define constants
const int width = 20;
const int height = 17;
const int numObstacles = 5;
const int powerUpDuration = 100;
const int specialFruitDuration = 50;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum PowerUp { NONE = 0, MOVE_THROUGH_WALLS, SLOW_TIME, DOUBLE_SCORE };

//global variables
int highScore = 0;
bool gameOver;
bool isPaused = false;
int gameSpeed = 100; // default game speed

//variables for solo play
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

//variables for AI play
int aiX, aiY, aiScore, aiNTail;
int aiTailX[100], aiTailY[100];
Direction aiDir;
int aiSpeed;

//power-up variables
bool powerUpActive = false;
int powerUpX, powerUpY;
PowerUp currentPowerUp = NONE;
int powerUpTimer = 0;
PowerUp powerUpType;
bool specialFruitActive = false;
int specialFruitX, specialFruitY;
int specialFruitTimer = 0;
// int specialFruitDuration = 100;

//mode selection
bool playAgainstAI = false;

//function prototypes
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
    gameOver = false;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;

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

    // move the AI's head
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

    // wrap around the screen
    if (aiX >= width) aiX = 0; else if (aiX < 0) aiX = width - 1;
    if (aiY >= height) aiY = 0; else if (aiY < 0) aiY = height - 1;

    // check for collision with walls
    for (int i = 0; i < numObstacles; i++) {
        if (aiX == obstacleX[i] && aiY == obstacleY[i]) {
            gameOver = true;
            return;
        }
    }

    // check if AI eats the fruit
    if (aiX == fruitX && aiY == fruitY) {
        aiScore += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        aiNTail++;

        // ensure fruit does not spawn on the AI's tail or obstacles
        while (std::find(aiTailX, aiTailX + aiNTail, fruitX) != aiTailX + aiNTail ||
            std::find(aiTailY, aiTailY + aiNTail, fruitY) != aiTailY + aiNTail ||
            std::find(obstacleX, obstacleX + numObstacles, fruitX) != obstacleX + numObstacles ||
            std::find(obstacleY, obstacleY + numObstacles, fruitY) != obstacleY + numObstacles) {
            fruitX = rand() % width;
            fruitY = rand() % height;
        }
    }

    // check for collision with AI's own tail
    for (int i = 0; i < aiNTail; i++) {
        if (aiTailX[i] == aiX && aiTailY[i] == aiY) {
            gameOver = true;
        }
    }
}

void Draw() {
    system("cls");
    for (int i = 0; i < width + 2; i++)
        std::cout << "#";
    std::cout << std::endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                std::cout << "#"; // border

            bool print = false;

            // draw AI's snake
            if (playAgainstAI) {
                if (i == aiY && j == aiX) {
                    std::cout << "A"; // AI's snake head
                    print = true;
                }
                else {
                    for (int k = 0; k < aiNTail; k++) {
                        if (aiTailX[k] == j && aiTailY[k] == i) {
                            std::cout << "a"; // AI's snake tail
                            print = true;
                        }
                    }
                }
            }

            // draw player's snake
            if (!print) {
                if (i == y && j == x) {
                    std::cout << "O"; // player's snake head
                    print = true;
                }
                else {
                    for (int k = 0; k < nTail; k++) {
                        if (tailX[k] == j && tailY[k] == i) {
                            std::cout << "o"; // player's snake tail
                            print = true;
                        }
                    }
                }
            }

            // draw obstacles
            if (!print) {
                for (int k = 0; k < numObstacles; k++) {
                    if (i == obstacleY[k] && j == obstacleX[k]) {
                        std::cout << "#"; // obstacle
                        print = true;
                    }
                }
            }

            // draw fruit
            if (!print) {
                if (i == fruitY && j == fruitX) {
                    std::cout << "F"; // fruit
                }
                else {
                    std::cout << " "; // empty space
                }
            }

            if (j == width - 1)
                std::cout << "#";
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < width + 2; i++)
        std::cout << "#";
    std::cout << std::endl;

    std::cout << "Score: " << score << " High Score: " << highScore << std::endl;
    std::cout << "Power Up: " << (powerUpActive ? (currentPowerUp == MOVE_THROUGH_WALLS ? "Move Through Walls" : (currentPowerUp == SLOW_TIME ? "Slow Time" : "Double Score")) : "None") << std::endl;
    std::cout << "Special Fruit: " << (specialFruitActive ? "Active" : "Inactive") << std::endl;

    if (playAgainstAI) {
        std::cout << "AI Score: " << aiScore << std::endl;
    }
}


void Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a':
            if (x > 0)
                x--;
            break;
        case 'd':
            if (x < width - 1)
                x++;
            break;
        case 'w':
            if (y > 0)
                y--;
            break;
        case 's':
            if (y < height - 1)
                y++;
            break;
        case 'p':
            isPaused = !isPaused;
            break;
        case 'q':
            gameOver = true;
            break;
        case 't':
            if (!powerUpActive) {
                powerUpX = rand() % width;
                powerUpY = rand() % height;
                currentPowerUp = static_cast<PowerUp>(rand() % 3 + 1); // random power-up
                powerUpActive = true;
            }
            break;
        case 'f':
            if (!specialFruitActive) {
                specialFruitX = rand() % width;
                specialFruitY = rand() % height;
                specialFruitActive = true;
                specialFruitTimer = specialFruitDuration;
            }
            break;
        default:
            break;
        }
    }
}

void Logic() {
    if (isPaused)
        return;

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

    // check for collision with fruit
    if (x == fruitX && y == fruitY) {
        score += 10;
        nTail++;
        fruitX = rand() % width;
        fruitY = rand() % height;

        // ensure fruit does not spawn on the snake's tail or obstacles
        while (std::find(tailX, tailX + nTail, fruitX) != tailX + nTail ||
            std::find(tailY, tailY + nTail, fruitY) != tailY + nTail ||
            std::find(obstacleX, obstacleX + numObstacles, fruitX) != obstacleX + numObstacles ||
            std::find(obstacleY, obstacleY + numObstacles, fruitY) != obstacleY + numObstacles) {
            fruitX = rand() % width;
            fruitY = rand() % height;
        }

        if (score > highScore) {
            highScore = score;
        }

        fruitsCollected++;
        consecutiveFruits++;
        survivalTicks++;
        totalTilesCovered++;
        tilesCovered[x][y] = true;

        if (consecutiveFruits % 10 == 0) {
            if (consecutiveFruits >= 10) {
                scoreHunterUnlocked = true;
                std::cout << "Achievement Unlocked: Score Hunter!" << std::endl;
            }
        }

        if (survivalTicks >= 100) {
            survivorUnlocked = true;
            std::cout << "Achievement Unlocked: Survivor!" << std::endl;
        }

        if (fruitsCollected >= 20) {
            fruitCollectorUnlocked = true;
            std::cout << "Achievement Unlocked: Fruit Collector!" << std::endl;
        }

        if (score >= 500) {
            speedDemonUnlocked = true;
            std::cout << "Achievement Unlocked: Speed Demon!" << std::endl;
        }

        if (totalTilesCovered >= 100) {
            explorerUnlocked = true;
            std::cout << "Achievement Unlocked: Explorer!" << std::endl;
        }

        if (survivalTicks >= 200) {
            longevityUnlocked = true;
            std::cout << "Achievement Unlocked: Longevity!" << std::endl;
        }

        if (score >= 1000) {
            highRollerUnlocked = true;
            std::cout << "Achievement Unlocked: High Roller!" << std::endl;
        }

        if (specialFruitActive) {
            specialFruitTimer--;
            if (specialFruitTimer <= 0) {
                specialFruitActive = false;
            }
        }
    }

    // check for collision with tail or obstacles
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y) {
            std::cout << "Game Over: Collision with tail at (" << tailX[i] << ", " << tailY[i] << ")" << std::endl;
            gameOver = true;
        }
    }

    for (int i = 0; i < numObstacles; i++) {
        if (obstacleX[i] == x && obstacleY[i] == y) {
            std::cout << "Game Over: Collision with obstacle at (" << obstacleX[i] << ", " << obstacleY[i] << ")" << std::endl;
            gameOver = true;
        }
    }

    if (playAgainstAI) {
        AILogic();
        if (aiX == x && aiY == y) {
            std::cout << "Game Over: Collision with AI at (" << aiX << ", " << aiY << ")" << std::endl;
            gameOver = true;
        }
    }
}

int main() {
    ShowMenu();
    Setup();

    while (!gameOver) {
        Draw();
        Input();
        Logic();
        if (playAgainstAI) {
            Sleep(aiSpeed);
        }
        else {
            Sleep(gameSpeed);
        }
    }

    SaveHighScore();
    std::cout << "Game Over!" << std::endl;
    std::cout << "Final Score: " << score << std::endl;
    std::cout << "Press any key to exit...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}