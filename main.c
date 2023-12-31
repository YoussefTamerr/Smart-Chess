#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"
#include <string.h>

// =============================================================================
// Keypad
// =============================================================================

#define ROWS 4
#define COLS 4

const uint8_t rowPins[ROWS] = {2, 3, 4, 5}; 
const uint8_t colPins[COLS] = {6, 7, 8, 9}; 

// =============================================================================
// Chess board
// =============================================================================

// // Starting Board
// char board[9][9] = {
// //    0    1    2    3    4    5    6    7    8
//     {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 0
//     {' ', 'B', 'P', ' ', ' ', ' ', ' ', 'p', 'r'}, // A
//     {' ', 'N', 'P', ' ', ' ', ' ', ' ', 'p', 'n'}, // B
//     {' ', 'B', 'P', ' ', ' ', ' ', ' ', 'p', 'b'}, // C
//     {' ', 'Q', 'P', ' ', ' ', ' ', ' ', 'p', 'q'}, // D
//     {' ', 'K', 'P', ' ', ' ', ' ', ' ', 'p', 'k'}, // E
//     {' ', 'B', 'P', ' ', ' ', ' ', ' ', 'p', 'b'}, // F
//     {' ', 'N', 'P', ' ', ' ', ' ', ' ', 'p', 'n'}, // G
//     {' ', 'R', 'P', ' ', ' ', ' ', ' ', 'p', 'r'}, // H
// };

// =============================================================================
// Puzzles
// =============================================================================

// // Siegbert Tarrasch vs. Max Kurschner
// // Solution: 1. Qg6+ hxg6 2. Bxg6#
// char board[9][9] = {
// //    0    1    2    3    4    5    6    7    8
//     {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 0
//     {' ', 'R', ' ', ' ', 'p', ' ', ' ', 'p', 'r'}, // A
//     {' ', 'N', ' ', ' ', ' ', ' ', 'n', 'b', ' '}, // B
//     {' ', ' ', 'B', 'P', 'p', 'B', ' ', ' ', ' '}, // C
//     {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'q'}, // D
//     {' ', 'R', ' ', ' ', ' ', ' ', 'P', ' ', 'k'}, // E
//     {' ', ' ', 'P', ' ', ' ', 'Q', 'b', ' ', ' '}, // F
//     {' ', 'K', 'P', ' ', ' ', ' ', ' ', 'p', ' '}, // G
//     {' ', ' ', 'P', ' ', ' ', ' ', ' ', 'p', 'r'}, // H
// };

// // James Mason vs. Georg Marco
// // Solution: 1. Rg1+ 2. Kxg1 Rxf1#
// char board[9][9] = {
// //    0    1    2    3    4    5    6    7    8
//     {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 0
//     {' ', ' ', ' ', ' ', ' ', ' ', 'B', 'p', 'k'}, // A
//     {' ', 'K', 'P', ' ', ' ', ' ', 'p', 'R', ' '}, // B
//     {' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', 'n'}, // C
//     {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'r'}, // D
//     {' ', ' ', ' ', ' ', 'P', ' ', ' ', ' ', 'r'}, // E
//     {' ', ' ', ' ', 'P', 'B', ' ', ' ', ' ', 'b'}, // F
//     {' ', ' ', 'P', ' ', ' ', ' ', ' ', 'p', ' '}, // G
//     {' ', ' ', 'P', ' ', ' ', ' ', 'p', ' ', ' '}, // H
// };

// Milan Vidmar vs. Max Euwe
// Solution: 1. Qf8+ Kxf8 2. Rd8#
char board[9][9] = {
//    0    1    2    3    4    5    6    7    8
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 0
    {' ', ' ', 'P', 'Q', ' ', 'p', ' ', ' ', ' '}, // A
    {' ', ' ', 'P', ' ', ' ', ' ', 'p', ' ', ' '}, // B
    {' ', ' ', 'r', ' ', ' ', ' ', ' ', ' ', ' '}, // C
    {' ', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // D
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // E
    {' ', ' ', ' ', ' ', 'q', 'N', ' ', 'p', ' '}, // F
    {' ', ' ', ' ', ' ', ' ', 'p', ' ', ' ', 'k'}, // G
    {' ', 'K', ' ', 'P', ' ', ' ', 'p', ' ', ' '}, // H
};

void printChessboard(char board[9][9]) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// =============================================================================
// Stepper motor
// =============================================================================

const uint STEPPER_PIN_1 =  10;
const uint STEPPER_PIN_2 =  11;
const uint STEPPER_PIN_3 =  12;
const uint STEPPER_PIN_4 =  13;
int step_numberX = 0;
int stepsX = 0;

const uint STEPPER_PIN_5 =  18;
const uint STEPPER_PIN_6 =  19;
const uint STEPPER_PIN_7 =  20;
const uint STEPPER_PIN_8 =  21;

int step_numberY = 0;
int stepsY = 0;

int directionX = 0;
int directionY = 0;

void moveX(bool dir){
    if (stepsX == 0) {
        return;
    }
    if (dir) {
        switch(step_numberX % 4){
            case 0:
            gpio_put(STEPPER_PIN_1, true);
            gpio_put(STEPPER_PIN_2, false);
            gpio_put(STEPPER_PIN_3, false);
            gpio_put(STEPPER_PIN_4, false);
            break;
            case 1:
            gpio_put(STEPPER_PIN_1, false);
            gpio_put(STEPPER_PIN_2, true);
            gpio_put(STEPPER_PIN_3, false);
            gpio_put(STEPPER_PIN_4, false);
            break;
            case 2:
            gpio_put(STEPPER_PIN_1, false);
            gpio_put(STEPPER_PIN_2, false);
            gpio_put(STEPPER_PIN_3, true);
            gpio_put(STEPPER_PIN_4, false);
            break;
            case 3:
            gpio_put(STEPPER_PIN_1, false);
            gpio_put(STEPPER_PIN_2, false);
            gpio_put(STEPPER_PIN_3, false);
            gpio_put(STEPPER_PIN_4, true);
            break;
        } 
    } else {
        switch(step_numberX % 4){
            case 0:
            gpio_put(STEPPER_PIN_1, false);
            gpio_put(STEPPER_PIN_2, false);
            gpio_put(STEPPER_PIN_3, false);
            gpio_put(STEPPER_PIN_4, true);
            break;
            case 1:
            gpio_put(STEPPER_PIN_1, false);
            gpio_put(STEPPER_PIN_2, false);
            gpio_put(STEPPER_PIN_3, true);
            gpio_put(STEPPER_PIN_4, false);
            break;
            case 2:
            gpio_put(STEPPER_PIN_1, false);
            gpio_put(STEPPER_PIN_2, true);
            gpio_put(STEPPER_PIN_3, false);
            gpio_put(STEPPER_PIN_4, false);
            break;
            case 3:
            gpio_put(STEPPER_PIN_1, true);
            gpio_put(STEPPER_PIN_2, false);
            gpio_put(STEPPER_PIN_3, false);
            gpio_put(STEPPER_PIN_4, false);
            break;
        }
    }
    step_numberX++;
    stepsX--; 
}

void moveY(bool dir) {
    if (stepsY == 0) {
        return;
    }
    if (dir) {
        switch(step_numberY % 4) {
            case 0:
            gpio_put(STEPPER_PIN_5, true);
            gpio_put(STEPPER_PIN_6, false);
            gpio_put(STEPPER_PIN_7, false);
            gpio_put(STEPPER_PIN_8, false);
            break;
            case 1:
            gpio_put(STEPPER_PIN_5, false);
            gpio_put(STEPPER_PIN_6, true);
            gpio_put(STEPPER_PIN_7, false);
            gpio_put(STEPPER_PIN_8, false);
            break;
            case 2:
            gpio_put(STEPPER_PIN_5, false);
            gpio_put(STEPPER_PIN_6, false);
            gpio_put(STEPPER_PIN_7, true);
            gpio_put(STEPPER_PIN_8, false);
            break;
            case 3:
            gpio_put(STEPPER_PIN_5, false);
            gpio_put(STEPPER_PIN_6, false);
            gpio_put(STEPPER_PIN_7, false);
            gpio_put(STEPPER_PIN_8, true);
            break;
        } 
    } else {
        switch(step_numberY % 4) {
            case 0:
            gpio_put(STEPPER_PIN_5, false);
            gpio_put(STEPPER_PIN_6, false);
            gpio_put(STEPPER_PIN_7, false);
            gpio_put(STEPPER_PIN_8, true);
            break;
            case 1:
            gpio_put(STEPPER_PIN_5, false);
            gpio_put(STEPPER_PIN_6, false);
            gpio_put(STEPPER_PIN_7, true);
            gpio_put(STEPPER_PIN_8, false);
            break;
            case 2:
            gpio_put(STEPPER_PIN_5, false);
            gpio_put(STEPPER_PIN_6, true);
            gpio_put(STEPPER_PIN_7, false);
            gpio_put(STEPPER_PIN_8, false);
            break;
            case 3:
            gpio_put(STEPPER_PIN_5, true);
            gpio_put(STEPPER_PIN_6, false);
            gpio_put(STEPPER_PIN_7, false);
            gpio_put(STEPPER_PIN_8, false);
            break;
        
        }
    }
    step_numberY++;
    stepsY--;
} 

// =============================================================================
// Function for reading keypad
// =============================================================================

char readKeypad() {
    for (int i = 0; i < ROWS; i++) {
        gpio_put(rowPins[i], 0);
        for (int j = 0; j < COLS; j++) {
            if (!gpio_get(colPins[j])) {
                gpio_put(rowPins[i], 1);
                return (char)('1' + i * COLS + j); 
            }
        }
        gpio_put(rowPins[i], 1);
    }
    return '\0'; 
}

// =============================================================================
// Initialize LED and magnet pins and functions
// =============================================================================

const uint led_green = 1;
const uint led_blue = 14; 
const uint led_red = 15;
const uint led_white = 16;
const uint led_black = 17;
const uint magnet = 26;

void turnOnMagnet() {
    gpio_put(magnet, true);
    gpio_put(led_blue, true);
}

void turnOffMagnet() {
    gpio_put(magnet, false);
    gpio_put(led_blue, false);
}

void blinkRed(int number) {
    for (int i = 0; i < number; i++) {
        gpio_put(led_red, true);
        sleep_ms(100);
        gpio_put(led_red, false);
        sleep_ms(100);
    }
}

void blinkGreen() {
    for (int i = 0; i < 5; i++) {
        gpio_put(led_green, true);
        sleep_ms(100);
        gpio_put(led_green, false);
        sleep_ms(100);
    }
}

void lightPlayerLed(char currPlayer) {
    if (currPlayer == 'W') {
        gpio_put(led_white, true);
        gpio_put(led_black, false);
    } else {
        gpio_put(led_black, true);
        gpio_put(led_white, false);
    }
}

void blinkAll() {
        for(int i = 0; i < 50; i++) {
        gpio_put(led_red, true);
        sleep_ms(100);
        gpio_put(led_red, false);
        gpio_put(led_green, true);
        sleep_ms(100);
        gpio_put(led_green, false);
        gpio_put(led_blue, true);
        sleep_ms(100);
        gpio_put(led_blue, false);
        gpio_put(led_black, true);
        sleep_ms(100);
        gpio_put(led_black, false);
        gpio_put(led_white, true);
        sleep_ms(100);
        gpio_put(led_white, false);
    }
}

// =============================================================================
// Validations
// =============================================================================

int isEnemy(char currPlayer, int currX, int currY, int targetX, int targetY) {
    char targetPiece = board[targetX][targetY];

    if ((currPlayer == 'W' && targetPiece >= 'a' && targetPiece <= 'z') ||
        (currPlayer == 'B' && targetPiece >= 'A' && targetPiece <= 'Z')) {
        return 1;
    }

    return 0;
}

int isValidMove(char currPlayer, int currX, int currY, int targetX, int targetY) {

    int isKill = isEnemy(currPlayer, currX, currY, targetX, targetY);

    char currPiece = board[currX][currY];

    if (currX < 1 || currX > 8 || currY < 1 || currY > 8 || targetX < 1 || targetX > 8 || targetY < 1 || targetY > 8) {
        return 0;
    }

    if (currX == targetX && currY == targetY) {
        return 0;
    }

   if (currPiece != 'N' && currPiece != 'n') {
        int deltaX = (targetX > currX) ? 1 : ((targetX < currX) ? -1 : 0);
        int deltaY = (targetY > currY) ? 1 : ((targetY < currY) ? -1 : 0);

        int x = currX + deltaX;
        int y = currY + deltaY;

        while (x != targetX || y != targetY) {
            if (board[x][y] != ' ') {
                return 0;
            }
            x += deltaX;
            y += deltaY;
        }
   }

    if (currPlayer == 'W') {
        if (board[targetX][targetY] >= 'A' && board[targetX][targetY] <= 'Z') {
            return 0;
        }
        if (currPiece == 'P') { 
            if (isKill == 0 && (currX == targetX && currY + 1 == targetY)) {
                return 1;
            } else if (isKill == 0 && (currX == targetX && currY + 2 == targetY && currY == 2)) {
                return 1;
            } else if (isKill == 1 && ((currX + 1 == targetX || currX - 1 == targetX) && currY + 1 == targetY)) {
                return 1;
            }
        } else if (currPiece == 'R') { 
            if (currX == targetX && currY != targetY) {
                return 1;
            } else if (currX != targetX && currY == targetY) {
                return 1;
            }
        } else if (currPiece == 'N') { 
            if ((currX + 1 == targetX || currX - 1 == targetX) && (currY + 2 == targetY || currY - 2 == targetY)) {
                return 1;
            } else if ((currX + 2 == targetX || currX - 2 == targetX) && (currY + 1 == targetY || currY - 1 == targetY)) {
                return 1;
            }
        } else if (currPiece == 'B') { 
            if (currX != targetX && currY != targetY && abs(currX - targetX) == abs(currY - targetY)) {
                return 1;
            }
        } else if (currPiece == 'Q') { 
            if (currX == targetX && currY != targetY) {
                return 1;
            } else if (currX != targetX && currY == targetY) {
                return 1;
            } else if (currX != targetX && currY != targetY && abs(currX - targetX) == abs(currY - targetY)) {
                return 1;
            }
        } else if (currPiece == 'K') { 
            if (abs(currX - targetX) == 1 || abs(currY - targetY) == 1) {
                return 1;
            }
        }
    } else if (currPlayer == 'B') {
        if (board[targetX][targetY] >= 'a' && board[targetX][targetY] <= 'z') {
            return 0;
        }
        if (currPiece == 'p') {
            if (isKill == 0 && (currX == targetX && currY - 1 == targetY)) {
                return 1;
            } else if (isKill == 0 && (currX == targetX && currY - 2 == targetY && currY == 7)) {
                return 1;
            } else if (isKill == 1 && ((currX + 1 == targetX || currX - 1 == targetX) && currY - 1 == targetY)) {
                return 1;
            }
        } else if (currPiece == 'r') {
            if (currX == targetX && currY != targetY) {
                return 1;
            } else if (currX != targetX && currY == targetY) {
                return 1;
            }
        } else if (currPiece == 'n') {
            if ((currX + 1 == targetX || currX - 1 == targetX) && (currY + 2 == targetY || currY - 2 == targetY)) {
                return 1;
            } else if ((currX + 2 == targetX || currX - 2 == targetX) && (currY + 1 == targetY || currY - 1 == targetY)) {
                return 1;
            }
        } else if (currPiece == 'b') {
            if (currX != targetX && currY != targetY && abs(currX - targetX) == abs(currY - targetY)) {
                return 1;
            }
        } else if (currPiece == 'q') {
            if (currX == targetX && currY != targetY) {
                return 1;
            } else if (currX != targetX && currY == targetY) {
                return 1;
            } else if (currX != targetX && currY != targetY && abs(currX - targetX) == abs(currY - targetY)) {
                return 1;
            }
        } else if (currPiece == 'k') {
            if (abs(currX - targetX) == 1 || abs(currY - targetY) == 1) {
                return 1;
            }
        }
    }
    return 0;
}

int isCheck(char currPlayer, char currBoard[][9]) {
    int kingX = -1;
    int kingY = -1;

    for (int i = 1; i <= 8; i++) {
        for (int j = 1; j <= 8; j++) {
            if ((currPlayer == 'W' && currBoard[i][j] == 'K') ||
                (currPlayer == 'B' && currBoard[i][j] == 'k')) {
                kingX = i;
                kingY = j;
                break;
            }
        }
    }

    char otherPlayer = (currPlayer == 'W') ? 'B' : 'W';
    for (int i = 1; i <= 8; i++) {
        for (int j = 1; j <= 8; j++) {
            if ((currPlayer == 'W' && currBoard[i][j] >= 'a' && currBoard[i][j] <= 'z') ||
                (currPlayer == 'B' && currBoard[i][j] >= 'A' && currBoard[i][j] <= 'Z')) {
                if (isValidMove(otherPlayer, i, j, kingX, kingY)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int isCheckMate(char currPlayer) {
    if (!isCheck(currPlayer, board)) {
        return 0;
    }

    for (int i = 1; i <= 8; i++) {
        for (int j = 1; j <= 8; j++) {
            if ((currPlayer == 'W' && board[i][j] >= 'A' && board[i][j] <= 'Z') ||
                (currPlayer == 'B' && board[i][j] >= 'a' && board[i][j] <= 'z')) {
                for (int x = 1; x <= 8; x++) {
                    for (int y = 1; y <= 8; y++) {
                        if (isValidMove(currPlayer, i, j, x, y)) {
                            char tempPiece = board[x][y];
                            board[x][y] = board[i][j];
                            board[i][j] = ' ';
                            if (!isCheck(currPlayer, board)) {
                                board[i][j] = board[x][y];
                                board[x][y] = tempPiece;
                                return 0;
                            }
                            board[i][j] = board[x][y];
                            board[x][y] = tempPiece;
                        }
                    }
                }
            }
        }
    }

    return 1;
}

int willBeCheck(char currPlayer, int currX, int currY, int targetX, int targetY) {
    char currPiece = board[currX][currY];
    
    char originalTargetPiece = board[targetX][targetY];
    char originalCurrPiece = board[currX][currY];

    board[targetX][targetY] = currPiece;
    board[currX][currY] = ' ';

    int isCheckM = isCheck(currPlayer, board);

    board[currX][currY] = originalCurrPiece;
    board[targetX][targetY] = originalTargetPiece;

    return isCheckM;
}


// =============================================================================
// Main
// =============================================================================

int main() {

    stdio_init_all();

    // =============================================================================
    // Variables & Constants
    // =============================================================================

    const int oneSquare = 2850;

    int input_count = 0;
    int move[4] = {0, 0, 0, 0};
    int distanceX = 0;
    int distanceY = 0;
    int reachedPiece = 0;
    int reachedDestination = 0;
    int kill = 0;
    int finishedKill = 0;
    int initiateKill = 0;
    int goingHome = 0;

    int freshInput = 0;
    char currPlayer = 'W';
    char currPiece = ' ';

    // =============================================================================
    // Initialize keypad
    // =============================================================================

    for (int i = 0; i < ROWS; i++) {
        gpio_init(rowPins[i]);
        gpio_set_dir(rowPins[i], GPIO_OUT);
        gpio_put(rowPins[i], 1);
    }

    for (int i = 0; i < COLS; i++) {
        gpio_init(colPins[i]);
        gpio_set_dir(colPins[i], GPIO_IN);
        gpio_pull_up(colPins[i]);
    }

    // =============================================================================
    // Initialize LED and Magnet pins
    // =============================================================================

    gpio_init(led_green);
    gpio_set_dir(led_green, GPIO_OUT);

    gpio_init(led_blue);
    gpio_set_dir(led_blue, GPIO_OUT);

    gpio_init(led_red);
    gpio_set_dir(led_red, GPIO_OUT);
    
    gpio_init(led_white);
    gpio_set_dir(led_white, GPIO_OUT);
    
    gpio_init(led_black);
    gpio_set_dir(led_black, GPIO_OUT);
    
    gpio_init(magnet);
    gpio_set_dir(magnet, GPIO_OUT);

    // =============================================================================
    // Initialize Motor pins
    // =============================================================================

    gpio_init(STEPPER_PIN_1);
    gpio_set_dir(STEPPER_PIN_1, GPIO_OUT);

    gpio_init(STEPPER_PIN_2);
    gpio_set_dir(STEPPER_PIN_2, GPIO_OUT);

    gpio_init(STEPPER_PIN_3);
    gpio_set_dir(STEPPER_PIN_3, GPIO_OUT);

    gpio_init(STEPPER_PIN_4);
    gpio_set_dir(STEPPER_PIN_4, GPIO_OUT);

    gpio_init(STEPPER_PIN_5);
    gpio_set_dir(STEPPER_PIN_5, GPIO_OUT);

    gpio_init(STEPPER_PIN_6);
    gpio_set_dir(STEPPER_PIN_6, GPIO_OUT);

    gpio_init(STEPPER_PIN_7);
    gpio_set_dir(STEPPER_PIN_7, GPIO_OUT);

    gpio_init(STEPPER_PIN_8);
    gpio_set_dir(STEPPER_PIN_8, GPIO_OUT);

    gpio_put(led_white, true);

    while (1) {

        // =============================================================================
        // Keypad Code
        // =============================================================================

        while (input_count < 4) {
            char key = '\0';
             while (key == '\0') {
                key = readKeypad();
                if (key != '1' && key != '2' && key != '3' && key != '5' && key != '6' && key != '7' && key != '9' && key != ':') {
                    key = '\0';
                }
            }
                switch (key)
                {
                case '1':
                    key = '1';
                    break;
                case '2':
                    key = '2';
                    break;
                case '3':
                    key = '3';
                    break;
                case '5':
                    key = '4';
                    break;
                case '6':
                    key = '5';
                    break;
                case '7':
                    key = '6';
                    break;
                case '9':
                    key = '7';
                    break;
                case ':':
                    key = '8';
                    break;
                default:
                    break;
                }
            printf("Key pressed: %c\n", key);
            move[input_count] = key;
            input_count++;

            if (input_count >= 4) {
                freshInput = 1;
            }
            sleep_ms(750);
        }

        // =============================================================================
        // Check if move is valid
        // =============================================================================

        if (freshInput == 1) {
            freshInput = 0;
            int isValid = isValidMove(currPlayer, move[0] - '0', move[1] - '0', move[2] - '0', move[3] - '0');
            int isCheckM = willBeCheck(currPlayer, move[0] - '0', move[1] - '0', move[2] - '0', move[3] - '0');

            char originalTargetPiece = board[move[2] - '0'][move[3] - '0'];
            char originalCurrPiece = board[move[0] - '0'][move[1] - '0'];

            board[move[2] - '0'][move[3] - '0'] = board[move[0] - '0'][move[1] - '0'];
            board[move[0] - '0'][move[1] - '0'] = ' ';

            int stillInCheck = isCheck(currPlayer, board);

            board[move[0] - '0'][move[1] - '0'] = originalCurrPiece;
            board[move[2] - '0'][move[3] - '0'] = originalTargetPiece;

            if (stillInCheck == 1) {
                blinkRed(5);
                printf("Invalid Move, %s king is still in check.\n", (currPlayer == 'W') ? "White" : "Black");
                input_count = 0;
                move[0] = 0;
                move[1] = 0;
                move[2] = 0;
                move[3] = 0;
                distanceX = 0;
                distanceY = 0;
                reachedPiece = 0;
                reachedDestination = 0;
                kill = 0;
                finishedKill = 0;
                initiateKill = 0;
                goingHome = 0;
            } else if (isValid == 1) {
                if (isCheckM == 1) {
                    blinkRed(5);
                    printf("Invalid Move, %s king will be in check.\n", (currPlayer == 'W') ? "White" : "Black");
                    input_count = 0;
                    move[0] = 0;
                    move[1] = 0;
                    move[2] = 0;
                    move[3] = 0;
                    distanceX = 0;
                    distanceY = 0;
                    reachedPiece = 0;
                    reachedDestination = 0;
                    kill = 0;
                    finishedKill = 0;
                    initiateKill = 0;
                    goingHome = 0;
                } else {
                    blinkGreen();
                    printf("Valid Move\n");
                    currPiece = board[move[0] - '0'][move[1] - '0'];
                }
            } else {
                blinkRed(5);
                printf("Invalid Move\n");
                input_count = 0;
                move[0] = 0;
                move[1] = 0;
                move[2] = 0;
                move[3] = 0;
                distanceX = 0;
                distanceY = 0;
                reachedPiece = 0;
                reachedDestination = 0;
                kill = 0;
                finishedKill = 0;
                initiateKill = 0;
                goingHome = 0;
            }
        }

        // =============================================================================
        // Check if in kill mode
        // =============================================================================

        if (finishedKill == 0 && input_count == 4 && isEnemy(currPlayer, move[0] - '0', move[1] - '0', move[2] - '0', move[3] - '0') && kill != 1) {
            printf("Enter capture code\n");
            kill = 1;
        }

        // =============================================================================
        // If destination is empty, go to the old location
        // =============================================================================

        if (input_count == 4 && kill == 0) {
            turnOffMagnet();
            printf("Took Input\n");
            input_count = 5;
            distanceX = (move[0] - '0') * (oneSquare - 250);
            distanceY = (move[1] - '0') * (oneSquare + 100);
            directionX = 1;
            directionY = 1;
            printf("Distance X: %d, ", distanceX);
            printf("In the %d direction\n", directionX);
            printf("Distance Y: %d, ", distanceY);
            printf("In the %d direction\n", directionY);
            stepsX = distanceX;
            stepsY = distanceY;
        }

        // =============================================================================
        // If destination is not empty, go to the new location to capture the other piece
        // =============================================================================

        if (input_count == 4 && kill == 1) {
            turnOffMagnet();
            printf("Took target input\n");
            input_count = 5;
            distanceX = (move[2] - '0') * (oneSquare - 250);
            distanceY = (move[3] - '0') * (oneSquare + 100);
            directionX = 1;
            directionY = 1;
            printf("Distance X: %d, ", distanceX);
            printf("In the %d direction\n", directionX);
            printf("Distance Y: %d, ", distanceY);
            printf("In the %d direction\n", directionY);
            stepsX = distanceX;
            stepsY = distanceY;
        }

        if (stepsX == 0 && stepsY == 0 && input_count == 5) {
            if (reachedPiece == 0 && kill == 0) {
                reachedPiece = 1;
                printf("reached piece\n");
                int diffX = (move[2] - '0') - (move[0] - '0');
                int diffY = (move[3] - '0') - (move[1] - '0');
                distanceX = abs(diffX) * (oneSquare - 250);
                distanceY = abs(diffY) * (oneSquare + 100);
                directionX = (diffX > 0) ? 1 : 0;
                directionY = (diffY > 0) ? 1 : 0;
                printf("Distance X: %d, ", distanceX);
                printf("In the %d direction\n", directionX);
                printf("Distance Y: %d, ", distanceY);
                printf("In the %d direction\n", directionY);
                stepsX = distanceX;
                stepsY = distanceY;
                turnOnMagnet();
            } else if (reachedDestination == 0) {
                if (kill == 0) {
                    turnOffMagnet();
                    printf("Reached destination, going home\n");
                    goingHome = 1;
                } else {
                    turnOnMagnet();
                    printf("Reached destination, initiating capture\n");
                    initiateKill = 1;
                }
                reachedDestination = 1;
                distanceX = (move[2] - '0') * (oneSquare - 250);
                distanceY = (move[3] - '0') * (oneSquare + 100);
                if (goingHome == 1) {
                    distanceY += 1.5 * oneSquare;
                }
                directionX = 0;
                directionY = 0;
                printf("Distance X: %d, ", distanceX);
                printf("In the %d direction\n", directionX);
                printf("Distance Y: %d, ", distanceY);
                printf("In the %d direction\n", directionY);
                stepsX = distanceX;
                stepsY = distanceY;
            } else if (stepsX == 0 && stepsY == 0 && initiateKill == 1 && input_count != 0) {
                printf("Finished capture, reached origin\n");
                initiateKill = 0;
                kill = 0;
                finishedKill = 1;
                input_count = 4;
                reachedDestination = 0;
                turnOffMagnet();
            } else if (goingHome) {
                printf("Reached Origin\n");

                board[move[0] - '0'][move[1] - '0'] = ' ';
                board[move[2] - '0'][move[3] - '0'] = currPiece;
                
                currPlayer = (currPlayer == 'W') ? 'B' : 'W';

                printf("New Board:\n");
                printChessboard(board);

                int isCheckBool = isCheckMate(currPlayer);
                if (isCheckBool == 1) {
                    printf("Checkmate, %s wins\n", (currPlayer == 'W') ? "Black" : "White");
                    printf("Game Over\n");
                    blinkAll();
                    break;
                }

                int isCheckM = isCheck(currPlayer, board);
                if (isCheckM == 1) {
                    printf("Check, protect the %s king\n", (currPlayer == 'W') ? "White" : "Black");
                    blinkRed(15);
                }

                lightPlayerLed(currPlayer);

                input_count = 0;
                move[0] = 0;
                move[1] = 0;
                move[2] = 0;
                move[3] = 0;
                distanceX = 0;
                distanceY = 0;
                reachedPiece = 0;
                reachedDestination = 0;
                kill = 0;
                finishedKill = 0;
                initiateKill = 0;
                goingHome = 0;

                printf("\n\nWaiting for new input...\n");

            }
        }

        // =============================================================================
        // Motor Movement based on previous calculations
        // =============================================================================
        
        moveX(directionX);
        sleep_ms(2);
        moveY(directionY);
        sleep_ms(2);        
    }

    printf("\n\n\nGame Over\n");
    return 0;
}