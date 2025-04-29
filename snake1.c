#include <curses.h>
#include <unistd.h>
#include <stdbool.h>

#define SNAKE_LENGTH 5

void draw_snake(int[], int[]);
void move_snake(int[], int[], int, int, int, bool*);
bool check_collision(int[], int[], int, int);
bool check_reverse(int, int);

int main() {
    // Initialize curses
    initscr();
    start_color(); // Enable color
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Define color pair for snake (green on black)
    init_pair(2, COLOR_RED, COLOR_BLACK); // Define color pair for border (blue on black)
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    timeout(100);

    // Get terminal size
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    const int WIDTH = max_x;
    const int HEIGHT = max_y;

    // Initial snake positions
    int snake_x[SNAKE_LENGTH] = {5, 4, 3, 2, 1};
    int snake_y[SNAKE_LENGTH] = {1, 1, 1, 1, 1};
    int direction = KEY_RIGHT;
    int ch = KEY_RIGHT; // Start moving right --> Random direction later
    bool game_over = false;

    while (ch != 'x' && !game_over) {
        // Change direction based on user input
        ch = getch();
        if (ch != ERR && !check_reverse(ch, direction)) {
            direction = ch;
        }

        // Move the snake
        move_snake(snake_x, snake_y, direction, WIDTH, HEIGHT, &game_over);

        // Draw the snake and border
        clear();
        attron(COLOR_PAIR(1)); // Apply green color for the snake
        draw_snake(snake_x, snake_y);
        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(2)); // Apply blue color for the border
        for (int i = 0; i < WIDTH; i++) {
            mvprintw(0, i, "_");
            mvprintw(HEIGHT - 1, i, "_");
        }
        for (int i = 1; i < HEIGHT - 1; i++) {
            mvprintw(i, 0, "|");
            mvprintw(i, WIDTH - 1, "|");
        }
        attroff(COLOR_PAIR(2));

        refresh();

        usleep(100000);
    }

    // Cleanup curses
    endwin();
    return 0;
}

void draw_snake(int x[], int y[]) {
    for (int i = 0; i < SNAKE_LENGTH; i++) {
        mvprintw(y[i], x[i], "*"); //print on screen
    }
}

void move_snake(int x[], int y[], int direction, int WIDTH, int HEIGHT, bool* game_over) {
    int new_x = x[0];
    int new_y = y[0];

    switch (direction) {
        case KEY_UP:
            new_y--;
            break;
        case KEY_DOWN:
            new_y++;
            break;
        case KEY_LEFT:
            new_x--;
            break;
        case KEY_RIGHT:
            new_x++;
            break;
    }

    // Check if the new position is within the boundaries
    if (new_x >= 1 && new_x < WIDTH - 1 && new_y >= 1 && new_y < HEIGHT - 1) {
        // Check for collision with itself
        if (check_collision(x, y, new_x, new_y)) {
            *game_over = true;
            return;
        }

        // Move the snake
        for (int i = SNAKE_LENGTH - 1; i > 0; i--) {
            x[i] = x[i - 1];
            y[i] = y[i - 1];
        }
        x[0] = new_x;
        y[0] = new_y;
    } else {
        *game_over = true;
    }
}

//these conditions check for reverse, or if the snake bumps into the border, ends the game
//if this happens.

bool check_collision(int x[], int y[], int new_x, int new_y) {
    // Check if the new position collides with any part of the snake's body
    for (int i = 1; i < SNAKE_LENGTH; i++) {
        if (x[i] == new_x && y[i] == new_y) {
            return true;
        }
    }
    return false;
}

bool check_reverse(int input_direction, int current_direction) {
    // Check if the user is attempting to reverse the snake's direction
    if ((input_direction == KEY_UP && current_direction == KEY_DOWN) ||
        (input_direction == KEY_DOWN && current_direction == KEY_UP) ||
        (input_direction == KEY_LEFT && current_direction == KEY_RIGHT) ||
        (input_direction == KEY_RIGHT && current_direction == KEY_LEFT)) {
        return true;
    }
    return false;
}
