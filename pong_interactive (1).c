#include <ncurses.h>
#include <unistd.h>  // только для usleep()

#define WIDTH 80
#define HEIGHT 25
#define PAD_SIZE 3
#define MAX_SCORE 21

typedef struct {
    int x, y;
    int dx, dy;
} Ball;

typedef struct {
    int y;
    int score;
} Paddle;

void init_game(Ball *ball, Paddle *left, Paddle *right) {
    ball->x = WIDTH / 2;
    ball->y = HEIGHT / 2;
    ball->dx = 1;
    ball->dy = 1;

    left->y = HEIGHT / 2;
    left->score = 0;

    right->y = HEIGHT / 2;
    right->score = 0;
}

void draw(Ball *ball, Paddle *left, Paddle *right) {
    clear();

    for (int x = 0; x < WIDTH; x++) {
        mvaddch(0, x, '-');
        mvaddch(HEIGHT - 1, x, '-');
    }

    for (int i = 0; i < PAD_SIZE; i++) {
        mvaddch(left->y + i, 1, '|');
        mvaddch(right->y + i, WIDTH - 2, '|');
    }

    mvaddch(ball->y, ball->x, 'O');

    mvprintw(0, WIDTH / 2 - 10, "Player 1: %02d | Player 2: %02d", left->score, right->score);

    refresh();
}

void update_ball(Ball *ball, Paddle *left, Paddle *right) {
    ball->x += ball->dx;
    ball->y += ball->dy;

    if (ball->y <= 1 || ball->y >= HEIGHT - 2) {
        ball->dy *= -1;
    }

    if (ball->x == 2 && ball->y >= left->y && ball->y < left->y + PAD_SIZE) {
        ball->dx *= -1;
    }

    if (ball->x == WIDTH - 3 && ball->y >= right->y && ball->y < right->y + PAD_SIZE) {
        ball->dx *= -1;
    }

    if (ball->x <= 0) {
        right->score++;
        ball->x = WIDTH / 2;
        ball->y = HEIGHT / 2;
        ball->dx = -1;
    }

    if (ball->x >= WIDTH - 1) {
        left->score++;
        ball->x = WIDTH / 2;
        ball->y = HEIGHT / 2;
        ball->dx = 1;
    }
}

int handle_input(Paddle *left, Paddle *right) {
    int ch = getch();

    switch (ch) {
        case 'a':
        case 'A':
            if (left->y > 1) left->y--;
            break;
        case 'z':
        case 'Z':
            if (left->y < HEIGHT - 1 - PAD_SIZE) left->y++;
            break;
        case 'k':
        case 'K':
            if (right->y > 1) right->y--;
            break;
        case 'm':
        case 'M':
            if (right->y < HEIGHT - 1 - PAD_SIZE) right->y++;
            break;
        case 'q':
        case 'Q':
            return 1;
    }
    return 0;
}

int check_game_over(Paddle *left, Paddle *right) {
    return (left->score >= MAX_SCORE || right->score >= MAX_SCORE);
}

void show_game_over(Paddle *left, Paddle *right) {
    clear();
    if (left->score > right->score) {
        mvprintw(HEIGHT / 2, WIDTH / 2 - 10, "Player 1 Wins! Score: %d-%d", left->score, right->score);
    } else {
        mvprintw(HEIGHT / 2, WIDTH / 2 - 10, "Player 2 Wins! Score: %d-%d", right->score, left->score);
    }
    mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 15, "Press any key to exit...");
    refresh();
    getch();
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);  // неблокирующий ввод для игры

    Ball ball;
    Paddle left, right;
    init_game(&ball, &left, &right);

    int game_running = 1;
    while (game_running && !check_game_over(&left, &right)) {
        draw(&ball, &left, &right);
        if (handle_input(&left, &right)) {
            game_running = 0;
        }
        update_ball(&ball, &left, &right);
        usleep(50000);  // 50ms
    }

    if (game_running) {
        nodelay(stdscr, FALSE);  // блокирующий режим
        show_game_over(&left, &right);
    }

    endwin();
    return 0;
}

// gcc pong_interactive.c -lncurses -o pong