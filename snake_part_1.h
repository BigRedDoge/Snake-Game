#ifndef SNAKE_PART_1_H
#define SNAKE_PART_1_H

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
void drawBorder();
void moveSnake(struct snake *snake, int moveX, int moveY);
int checkAlive(struct snake snake);


#endif