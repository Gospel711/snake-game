# Snake Game

##  Overview
This is a **classic Snake game**, implemented in **C**, with additional features like obstacles, a customizable snake skin, and a leaderboard.

##  Features
- **Move the snake** using `W`, `A`, `S`, `D`
- **Eat apples** (`F`) to grow the snake
- **Avoid obstacles** (`X`) and the snake's tail
- **Choose difficulty levels** (`Easy`, `Normal`, `Hard`)
- **Customize the snake’s skin** (change the head and tail appearance)
- **Leaderboard tracking** for high scores

##  Technologies Used
- **C programming language**
- **ANSI escape codes for colors**
- **CMake for build automation**
- **MinGW for compilation (Windows)**
- **Cross-platform support for input handling**

##  Installation & Setup
### **Prerequisites**
Ensure you have the following installed:
- **Git** → `git --version`
- **CMake** → `cmake --version`
- **MinGW** → `mingw32-make --version`

### **Setup Steps**
1. Clone the repository:
   ```sh
   git clone https://github.com/YOUR_USERNAME/YOUR_REPOSITORY.git
2. Navigate to the project directory:
   cd YOUR_REPOSITORY
3. Build the project:
   cmake -G "MinGW Makefiles" .
mingw32-make
4. - Run the game:
   ./SnakeGame.exe

 How to Play
- Navigate the snake using W, A, S, and D.
- Eat apples (F) to increase the score.
- Avoid crashing into obstacles (X) or the snake's own tail.
- Press 'X' anytime to exit the game.
- Change difficulty levels or customize the snake skin in the main menu.
  License
This project is licensed under the MIT License. Feel free to use and modify it!
  Contributing
Contributions you are welcome! If you'd like to improve the game, create a pull request or open an issue.
