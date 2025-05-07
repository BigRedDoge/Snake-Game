#include <curses.h> // Required for terminal screen manipulation (ncurses library)
#include <unistd.h> // Required for usleep() function (pausing execution)
#include <stdbool.h> // Required for boolean type (true/false)

// Author: Ramsundhar Venkadesan

#include <time.h>

struct {
    int x, y;
    int value; // 1 to 9
    time_t spawn_time;
    int lifespan; // in seconds
    bool active;
} Trophy;

Trophy trophy;

// Generate new trophy at random location
void spawn_trophy(int screen_height, int screen_width, int snake_x[], int snake_y[], int length) {
    srand(time(NULL));
    bool overlap;
    do {
        overlap = false;
        trophy.x = rand() % (screen_width - 2) + 1;
        trophy.y = rand() % (screen_height - 2) + 1;
        trophy.value = (rand() % 9) + 1;
        trophy.lifespan = (rand() % 9) + 1;
        trophy.spawn_time = time(NULL);
        trophy.active = true;

        for (int i = 0; i < length; i++) {
            if (trophy.x == snake_x[i] && trophy.y == snake_y[i]) {
                overlap = true;
                break;
            }
        }
    } while (overlap);
}


#define INITIAL_SNAKE_LENGTH 5 // Define the initial length of the snake

#define MAX_SNAKE_LENGTH 100 // Define the maximum possible length of the snake (adjust as needed)

// Function Prototypes

// Renders the snake on the screen
void draw_snake(int snake_x_coords[], int snake_y_coords[], int current_length);

// Handles the snake's movement based on the current direction
void move_snake(int snake_x_coords[], int snake_y_coords[], int current_length, int direction, int screen_height, int screen_width, bool* game_over);

// Checks if the snake's head collides with its own body
bool check_self_collision(int snake_x_coords[], int snake_y_coords[], int current_length, int new_head_x, int new_head_y);

// Checks if the new direction is the exact opposite of the current direction
bool is_reverse_direction(int new_direction, int current_direction);

// --- Main Game Logic ---

int main(void) {


    initscr();          // Initialize the ncurses library
    start_color();      // Enable color capabilities
    noecho();           // Don't echo typed characters to the screen
    curs_set(0);        // Hide the cursor
    keypad(stdscr, TRUE); // Enable reading special keys (like arrow keys)
    nodelay(stdscr, TRUE); // Make getch() non-blocking (doesn't wait for input)
    timeout(100);       // Set a timeout for getch() in milliseconds (controls game speed)

    // Pair 1: Green snake on black background
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    // Pair 2: Red border on black background
    init_pair(2, COLOR_RED, COLOR_BLACK);

    // Get screen dimensions
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);


    // Arrays to store the X and Y coordinates of each snake segment
    int snake_x_coords[MAX_SNAKE_LENGTH];
    int snake_y_coords[MAX_SNAKE_LENGTH];
    int current_snake_length = INITIAL_SNAKE_LENGTH;

    // Initialize the snake's starting position (horizontal line near top-left)
    for (int i = 0; i < current_snake_length; ++i) {
        snake_x_coords[i] = 5 - i; // Head starts at x=5, body extends left
        snake_y_coords[i] = 1;     // All segments start at y=1
    }

    int current_direction = KEY_RIGHT; // Snake starts moving right
    int input_key = KEY_RIGHT;         // Stores the last valid key press
    bool game_over = false;           // Flag to control the main game loop

    // --- Game Loop ---
    // Loop continues until the user presses 'x' or the game ends (collision)
    while (input_key != 'x' && !game_over) {

        // --- Input Handling ---
        int key_pressed = getch(); // Check for user input

        // Process input only if a key was actually pressed (getch() != ERR)
        // and the new direction is not the reverse of the current one
        if (key_pressed != ERR && !is_reverse_direction(key_pressed, current_direction)) {
            // Update direction based on valid input
            switch (key_pressed) {
                case KEY_UP:
                case KEY_DOWN:
                case KEY_LEFT:
                case KEY_RIGHT:
                    current_direction = key_pressed;
                    break;
                case 'x': // Allow 'x' to quit immediately
                    input_key = key_pressed;
                    break;
                // Ignore other keys
            }
        }

        // Move the snake based on the current direction
        move_snake(snake_x_coords, snake_y_coords, current_snake_length, current_direction, screen_height, screen_width, &game_over);

        clear(); // Clear the screen before drawing the new frame

        // Draw the border
        attron(COLOR_PAIR(2)); // Use red color for border
        for (int i = 0; i < screen_width; i++) {
            mvprintw(0, i, "#"); // Top border
            mvprintw(screen_height - 1, i, "#"); // Bottom border
        }
        for (int i = 1; i < screen_height - 1; i++) {
            mvprintw(i, 0, "#"); // Left border
            mvprintw(i, screen_width - 1, "#"); // Right border
        }
        attroff(COLOR_PAIR(2)); // Turn off red color

        attron(COLOR_PAIR(1)); // Use green color for snake
        draw_snake(snake_x_coords, snake_y_coords, current_snake_length);
        attroff(COLOR_PAIR(1)); // Turn off green color

        refresh(); // Update the physical screen with the changes

        usleep(100000); // Pause to  to control game speed
    }

    endwin(); // Restore terminal settings and exit ncurses mode

    if (game_over) {
        printf("Game Over!\n");
    }

    return 0; 
}

void draw_snake(int snake_x_coords[], int snake_y_coords[], int current_length) {
    for (int i = 0; i < current_length; i++) {
        mvprintw(snake_y_coords[i], snake_x_coords[i], "*"); // Draw each segment
    }
}

void move_snake(int snake_x_coords[], int snake_y_coords[], int current_length, int direction, int screen_height, int screen_width, bool* game_over) {
    
    // Calculate the potential new coordinates for the snake's head
    int new_head_x = snake_x_coords[0];
    int new_head_y = snake_y_coords[0];

    switch (direction) {
        case KEY_UP:
            new_head_y--;
            break;
        case KEY_DOWN:
            new_head_y++;
            break;
        case KEY_LEFT:
            new_head_x--;
            break;
        case KEY_RIGHT:
            new_head_x++;
            break;
    }

    // Check for collision with the walls (screen boundaries)
    if (new_head_x <= 0 || new_head_x >= screen_width - 1 || new_head_y <= 0 || new_head_y >= screen_height - 1) {
        *game_over = true; // Collision with wall
        return;            // Stop movement processing
    }

    // Check for collision with the snake's own body
    if (check_self_collision(snake_x_coords, snake_y_coords, current_length, new_head_x, new_head_y)) {
        *game_over = true; // Collision with self
        return;            // Stop movement processing
    }

    // If no collision, move the snake: Shift all body segments one position forward (tail follows head)
    for (int i = current_length - 1; i > 0; i--) {
        snake_x_coords[i] = snake_x_coords[i - 1];
        snake_y_coords[i] = snake_y_coords[i - 1];
    }
    // Update the head's position to the new coordinates
    snake_x_coords[0] = new_head_x;
    snake_y_coords[0] = new_head_y;
}

bool check_self_collision(int snake_x_coords[], int snake_y_coords[], int current_length, int new_head_x, int new_head_y) {
    // Start checking from the second segment (index 1)
    for (int i = 1; i < current_length; i++) {
        if (snake_x_coords[i] == new_head_x && snake_y_coords[i] == new_head_y) {
            return true; // Collision detected
        }
    }
    return false; // No collision
}

bool is_reverse_direction(int input_direction, int current_direction) {
    if ((input_direction == KEY_UP && current_direction == KEY_DOWN) ||
        (input_direction == KEY_DOWN && current_direction == KEY_UP) ||
        (input_direction == KEY_LEFT && current_direction == KEY_RIGHT) ||
        (input_direction == KEY_RIGHT && current_direction == KEY_LEFT)) {
        return true; // It's a reverse move
    }
    return false; // Not a reverse move
}
