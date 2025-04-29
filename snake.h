#ifndef SNAKE_H
#define SNAKE_H

#include <ncurses.h>

#define MAX_SNAKE_LENGTH 100

struct body {
    int x;
    int y;
};

struct snake {
    int x;
    int y;
    int length;
    int direction;
    struct body *body;
};

struct food {
    int x;
    int y;
};

struct snake initializeSnake();
void playSnake(struct snake snake, int speed);
void drawSnake(struct snake *snake);
void drawFood(struct food food);
void drawBorder();
void moveSnake(struct snake *snake, int moveX, int moveY);
void spawnFood(struct food *food);
void checkIfEatFood(struct snake *snake, struct food *food);
int checkAlive(struct snake snake);


#endif