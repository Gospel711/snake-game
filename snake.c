#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <conio.h> // For _kbhit() and _getch() on Windows
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

// Function to enable ANSI escape codes on Windows
void EnableVirtualTerminalProcessing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h> // for usleep

// Cross-platform implementation of _kbhit()
int _kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

// Cross-platform implementation of _getch()
int _getch() {
    struct termios oldt, newt;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif
#include <time.h>
#include <time.h>

// Game constants
#define WIDTH 30
#define HEIGHT 20
#define MAX_TAIL 100
#define MAX_OBSTACLES 5

// Difficulty levels
#define EASY_SPEED 150
#define NORMAL_SPEED 100
#define HARD_SPEED 60

// Colors
#define COLOR_RESET "\033[0m"
#define COLOR_SNAKE "\033[1;32m" // Green for the snake
#define COLOR_APPLE "\033[1;31m" // Red for the apple
#define COLOR_OBSTACLE "\033[1;33m" // Yellow for obstacles
#define COLOR_BOUNDARY "\033[1;34m" // Blue for boundaries

// Global variables
int gameOver;
int x, y, fruitX, fruitY, score;
int tailX[MAX_TAIL], tailY[MAX_TAIL];
int nTail;
int obstacleX[MAX_OBSTACLES], obstacleY[MAX_OBSTACLES];
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum eDirection dir;

// New: Customization and leaderboard
int gameSpeed = 100; // Default speed (ms)
int highScore = 0;   // Session high score

// Snake skin and background customization
char snakeHeadChar = 'O';
char snakeTailChar = 'o';
char backgroundChar = ' ';

// Function prototypes
void Setup();
void Draw();
void Input();
void Logic();
void GenerateObstacles();
void DisplayMenu();
void DisplayInstructions();
int GameOverScreen();
void MoveCursorToTopLeft();
void ChangeDifficulty();
void ChangeSnakeSkin();
void ShowLeaderboard();

// Generate random obstacles (no overlap with snake or fruit)
void GenerateObstacles() {
    int obs;
    for (obs = 0; obs < MAX_OBSTACLES; obs++) {
        int validPosition = 0;
        while (!validPosition) {
            obstacleX[obs] = rand() % WIDTH;
            obstacleY[obs] = rand() % HEIGHT;
            validPosition = 1;
            // Check overlap with snake head or fruit
            if ((obstacleX[obs] == x && obstacleY[obs] == y) || (obstacleX[obs] == fruitX && obstacleY[obs] == fruitY)) {
                validPosition = 0;
                continue;
            }
            // Check overlap with snake tail
            int t;
            for (t = 0; t < nTail; t++) {
                if (obstacleX[obs] == tailX[t] && obstacleY[obs] == tailY[t]) {
                    validPosition = 0;
                    break;
                }
            }
            // Check overlap with other obstacles
            int k;
            for (k = 0; k < obs; k++) {
                if (obstacleX[obs] == obstacleX[k] && obstacleY[obs] == obstacleY[k]) {
                    validPosition = 0;
                    break;
                }
            }
        }
    }
}

void MoveCursorToTopLeft() {
    printf("\033[H"); // ANSI escape code to move the cursor to the top-left corner
}

// Draw the game
void Draw() {
    int i, row, col, j, k;
    MoveCursorToTopLeft(); // Move the cursor to the top-left corner

    // Draw the top boundary
    printf(COLOR_BOUNDARY);
    for (i = 0; i < WIDTH + 2; i++) {
        printf("#");
    }
    printf(COLOR_RESET "\n");

    // Draw the game area
    for (row = 0; row < HEIGHT; row++) {
        printf(COLOR_BOUNDARY "#" COLOR_RESET); // Left boundary
        for (col = 0; col < WIDTH; col++) {
            if (col == x && row == y) {
                printf(COLOR_SNAKE "%c" COLOR_RESET, snakeHeadChar); // Snake head
            } else if (col == fruitX && row == fruitY) {
                printf(COLOR_APPLE "F" COLOR_RESET); // Apple
            } else {
                int printed = 0;

                // Draw the snake's tail
                for (j = 0; j < nTail; j++) {
                    if (tailX[j] == col && tailY[j] == row) {
                        printf(COLOR_SNAKE "%c" COLOR_RESET, snakeTailChar);
                        printed = 1;
                        break;
                    }
                }

                // Draw obstacles
                if (!printed) {
                    for (k = 0; k < MAX_OBSTACLES; k++) {
                        if (obstacleX[k] == col && obstacleY[k] == row) {
                            printf(COLOR_OBSTACLE "X" COLOR_RESET);
                            printed = 1;
                            break;
                        }
                    }
                }

                if (!printed) {
                    printf("%c", backgroundChar); // Customizable background
                }
            }
        }
        printf(COLOR_BOUNDARY "#" COLOR_RESET "\n"); // Right boundary
    }

    // Draw the bottom boundary
    printf(COLOR_BOUNDARY);
    for (i = 0; i < WIDTH + 2; i++) {
        printf("#");
    }
    printf(COLOR_RESET "\n");

    // Display the score and high score
    printf("Score: %d\tHigh Score: %d\n", score, highScore);
    printf("Controls: W/A/S/D to move, X to exit\n");
}
// Handle user input
void Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a':
        case 'A':
            if (dir != RIGHT) dir = LEFT;
            break;
        case 'd':
        case 'D':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
        case 'W':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
        case 'S':
            if (dir != UP) dir = DOWN;
            break;
        case 'x':
        case 'X':
            gameOver = 1;
            break;
        }
    }
}

// Game logic
void Logic() {
    int t, obs;
    // Move tail
    for (t = nTail - 1; t > 0; t--) {
        tailX[t] = tailX[t - 1];
        tailY[t] = tailY[t - 1];
    }
    if (nTail > 0) {
        tailX[0] = x;
        tailY[0] = y;
    }

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

    // Wrap around the screen
    if (x >= WIDTH) x = 0; else if (x < 0) x = WIDTH - 1;
    if (y >= HEIGHT) y = 0; else if (y < 0) y = HEIGHT - 1;

    // Check collision with tail
    for (t = 0; t < nTail; t++) {
        if (tailX[t] == x && tailY[t] == y) {
            gameOver = 1;
            return;
        }
    }
    // Check collision with obstacles
    for (obs = 0; obs < MAX_OBSTACLES; obs++) {
        if (obstacleX[obs] == x && obstacleY[obs] == y) {
            gameOver = 1;
            return;
        }
    }

    // Check if the snake eats the fruit
    if (x == fruitX && y == fruitY) {
        score += 10;
        nTail++;
        int validFruitPosition = 0;
        while (!validFruitPosition) {
            fruitX = rand() % WIDTH;
            fruitY = rand() % HEIGHT;
            validFruitPosition = 1;
            // Check if fruit overlaps with the snake's body
            for (t = 0; t < nTail; t++) {
                if (tailX[t] == fruitX && tailY[t] == fruitY) {
                    validFruitPosition = 0;
                    break;
                }
            }
            // Check if fruit overlaps with obstacles
            if (validFruitPosition) {
                for (obs = 0; obs < MAX_OBSTACLES; obs++) {
                    if (obstacleX[obs] == fruitX && obstacleY[obs] == fruitY) {
                        validFruitPosition = 0;
                        break;
                    }
                }
            }
        }
    }
}
// Display the main menu
void DisplayMenu() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("########################################\n");
    printf("#              SNAKE GAME              #\n");
    // Note: The leaderboard only tracks the high score for the current session.
    printf("########################################\n");
    printf("# 1. Start Game                        #\n");
    printf("# 2. Instructions                      #\n");
    printf("# 3. Change Difficulty                 #\n");
    printf("# 4. Change Snake Skin                 #\n");
    printf("# 5. Leaderboard                       #\n");
    printf("# 6. Exit                              #\n");
    printf("########################################\n");
    printf("Enter your choice: ");
}

// Display instructions
void DisplayInstructions() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("########################################\n");
    printf("#              INSTRUCTIONS            #\n");
    printf("########################################\n");
    printf("# - Use 'W' to move up                 #\n");
    printf("# - Use 'S' to move down               #\n");
    printf("# - Use 'A' to move left               #\n");
    printf("# - Use 'D' to move right              #\n");
    printf("# - Avoid obstacles (X)                #\n");
    printf("# - Eat the apple (F) to grow          #\n");
    printf("# - Press 'X' to exit during game      #\n");
    printf("# - Change difficulty or skin in menu  #\n");
    printf("########################################\n");
    printf("Press any key to return to the menu...\n");
    _getch();
}

// Game over screen
int GameOverScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    if (score > highScore) highScore = score;
    printf("########################################\n");
    printf("#              GAME OVER               #\n");
    printf("########################################\n");
    printf("# Your Score: %d                        #\n", score);
    printf("# High Score: %d                        #\n", highScore);
    printf("########################################\n");
    printf("# 1. Restart                           #\n");
    printf("# 2. Exit                              #\n");
    printf("########################################\n");
    printf("Enter your choice: ");
    int ch = _getch();
    return ch;
}

// Change difficulty
void ChangeDifficulty() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("########################################\n");
    printf("#           CHANGE DIFFICULTY          #\n");
    printf("########################################\n");
    printf("# 1. Easy                              #\n");
    printf("# 2. Normal                            #\n");
    printf("# 3. Hard                              #\n");
    printf("########################################\n");
    printf("Enter your choice: ");
    int diff = _getch();
    switch (diff) {
        case '1':
            gameSpeed = EASY_SPEED;
            break;
        case '2':
            gameSpeed = NORMAL_SPEED;
            break;
        case '3':
            gameSpeed = HARD_SPEED;
            break;
        default:
            printf("\nInvalid choice. Keeping previous difficulty.\n");
#ifdef _WIN32
            Sleep(1000);
#else
            usleep(1000000);
#endif
            break;
    }
}

// Change snake skin and background
void ChangeSnakeSkin() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("########################################\n");
    printf("#           CUSTOMIZE SNAKE            #\n");
    printf("########################################\n");
    printf("Enter Snake Head Character (default O): ");
    snakeHeadChar = (char)_getch();
    printf("%c\n", snakeHeadChar);
    printf("Enter Snake Tail Character (default o): ");
    snakeTailChar = (char)_getch();
    printf("%c\n", snakeTailChar);
    printf("Enter Background Character (default space): ");
    backgroundChar = (char)_getch();
    if (backgroundChar < 32 || backgroundChar > 126) backgroundChar = ' ';
    printf("%c\n", backgroundChar);
    printf("Snake skin and background updated!\n");
#ifdef _WIN32
    Sleep(1000);
#else
    usleep(1000000);
#endif
}

// Show leaderboard
void ShowLeaderboard() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("########################################\n");
    printf("#             LEADERBOARD              #\n");
    printf("########################################\n");
    printf("# High Score This Session: %d           #\n", highScore);
    printf("########################################\n");
    printf("Press any key to return to the menu...\n");
    _getch();
}

// Setup function
void Setup() {
    gameOver = 0;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    score = 0;
    nTail = 0;
    GenerateObstacles();
}

// Main function
int main() {
    srand((unsigned int)time(NULL)); // Seed the random number generator

#ifdef _WIN32
    EnableVirtualTerminalProcessing(); // Enable ANSI escape codes on Windows
#endif

    while (1) {
        DisplayMenu();
        int choice = _getch();
        switch (choice) {
        case '1': // Start game
            Setup();
            while (!gameOver) {
                Draw();
                Input();
                Logic();
#ifdef _WIN32
                Sleep((DWORD)gameSpeed); // Use selected speed
#else
                usleep(gameSpeed * 1000);
#endif
            }
            {
                int endChoice = GameOverScreen();
                if (endChoice == '2') {
                    printf("Exiting the game. Goodbye!\n");
                    return 0;
                }
            }
            break;
        case '2': // Instructions
            DisplayInstructions();
            break;
        case '3': // Change Difficulty
            ChangeDifficulty();
            break;
        case '4': // Change Snake Skin
            ChangeSnakeSkin();
            break;
        case '5': // Leaderboard
            ShowLeaderboard();
            break;
        case '6': // Exit
            printf("Exiting the game. Goodbye!\n");
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
#ifdef _WIN32
            Sleep(1000);
#else
            usleep(1000000);
#endif
            break;
        }
    }
    return 0;
}
